import { randomUUID } from "node:crypto";
import {
  POOL,
  STATIONS,
  SPECIES,
  clamp,
  inPool,
  walkable,
  movePlayer,
} from "../shared/game.js";
const dist = (a, b) => Math.hypot(a.x - b.x, a.z - b.z);
export class GameRoom {
  constructor(code, send = () => {}) {
    this.code = code;
    this.send = send;
    this.players = new Map();
    this.fish = new Map();
    this.coins = 0;
    this.sold = 0;
    this.quest = 0;
    this.time = 0;
    this.tickId = 0;
    this.events = [];
  }
  join(id, name) {
    if (this.players.size >= 4) throw Error("This crew is full.");
    const p = {
      id,
      name: name.slice(0, 18),
      x: 2 + this.players.size,
      z: 20,
      yaw: 0,
      pitch: 0,
      hp: 100,
      tool: "rod",
      knife: false,
      sharp: 0,
      carrying: null,
      fishing: null,
      catches: 0,
      attackAt: -10,
      downAt: 0,
      input: {},
      lastInput: 0,
    };
    this.players.set(id, p);
    return p;
  }
  leave(id) {
    const p = this.players.get(id);
    if (p?.carrying) {
      const f = this.fish.get(p.carrying);
      if (f) f.owner = null;
    }
    this.players.delete(id);
  }
  notify(id, text) {
    this.send(id, { type: "notice", text });
  }
  near(p, key) {
    return dist(p, STATIONS[key]) <= STATIONS[key].range;
  }
  action(id, m) {
    const p = this.players.get(id);
    if (!p) return;
    if (m.type === "input") {
      p.yaw = Number.isFinite(m.yaw) ? m.yaw % (Math.PI * 2) : p.yaw;
      p.pitch = Number.isFinite(m.pitch) ? clamp(m.pitch, -1.3, 1.3) : p.pitch;
      p.input = {
        x: clamp(Number(m.x) || 0, -1, 1),
        z: clamp(Number(m.z) || 0, -1, 1),
        run: m.run === true,
        reel: m.reel === true,
      };
      p.lastInput = this.time;
      return;
    }
    if (m.type === "respawn") {
      if (p.hp <= 0 && this.time - p.downAt > 4) {
        p.hp = 100;
        p.x = 2;
        p.z = 20;
        this.notify(id, "Back at the harbor. Your equipment is safe.");
      }
      return;
    }
    if (p.hp <= 0) return;
    if (m.type === "equip" && ["rod", "fists", "knife"].includes(m.tool)) {
      p.tool = m.tool === "knife" && !p.knife ? "fists" : m.tool;
      p.fishing = null;
      return;
    }
    if (m.type === "cancel") {
      p.fishing = null;
      p.input = {};
      return;
    }
    if (m.type === "cast") {
      if (p.tool !== "rod" || p.carrying || p.fishing) return;
      const target = { x: Number(m.x), z: Number(m.z) };
      if (
        !Number.isFinite(target.x) ||
        !Number.isFinite(target.z) ||
        !inPool(target.x, target.z) ||
        dist(p, target) > 23
      ) {
        this.notify(id, "Aim your rod into the spring pond.");
        return;
      }
      if (this.fish.size >= 20) {
        this.notify(id, "Clear some catches before casting again.");
        return;
      }
      const boss = this.quest === 2 && m.boss === true;
      if (
        boss &&
        [...this.fish.values()].some((f) => f.species === 4 && f.hp > 0)
      ) {
        this.notify(id, "Bellmaw is already ashore!");
        return;
      }
      p.fishing = {
        phase: "waiting",
        elapsed: 0,
        progress: 0,
        tension: 0.15,
        x: target.x,
        z: target.z,
        boss,
      };
      return;
    }
    if (m.type === "attack") {
      if (p.tool === "rod" || p.carrying || this.time - p.attackAt < 0.55)
        return;
      p.attackAt = this.time;
      let nearest = null,
        range = 2.65;
      for (const f of this.fish.values()) {
        if (f.hp <= 0 || f.owner) continue;
        const d = dist(p, f);
        const dot =
          d < 0.1
            ? 1
            : ((f.x - p.x) * -Math.sin(p.yaw) +
                (f.z - p.z) * -Math.cos(p.yaw)) /
              d;
        if (d < range && dot > 0.4) {
          nearest = f;
          range = d;
        }
      }
      this.events.push({ type: "swing", id, at: this.time });
      if (nearest) {
        const damage =
          p.tool === "knife" && p.knife ? [18, 24, 32][p.sharp] : 8;
        nearest.hp = Math.max(0, nearest.hp - damage);
        nearest.hitAt = this.time;
        nearest.x += -Math.sin(p.yaw) * 0.3;
        nearest.z += -Math.cos(p.yaw) * 0.3;
        this.events.push({
          type: "hit",
          id: nearest.id,
          damage,
          at: this.time,
        });
        if (!nearest.hp) {
          nearest.state = "dead";
          this.notify(
            id,
            nearest.species === 4
              ? "Bellmaw defeated! Pick up the Bellheart."
              : SPECIES[nearest.species].name + " defeated · E to carry",
          );
        }
      }
      return;
    }
    if (m.type === "pickup") {
      if (p.carrying) return;
      const f = this.fish.get(m.id);
      if (f && f.hp === 0 && !f.owner && dist(p, f) < 3) {
        f.owner = id;
        p.carrying = f.id;
        p.fishing = null;
      }
      return;
    }
    if (m.type === "drop") {
      const f = this.fish.get(p.carrying);
      if (f) {
        f.owner = null;
        f.x = p.x - Math.sin(p.yaw) * 1.5;
        f.z = p.z - Math.cos(p.yaw) * 1.5;
        if (!walkable(f.x, f.z)) {
          f.x = p.x;
          f.z = p.z;
        }
        p.carrying = null;
      }
      return;
    }
    if (m.type === "sell") {
      if (!this.near(p, "seller")) return;
      const f = this.fish.get(p.carrying);
      if (!f || f.hp > 0 || f.owner !== id) {
        this.notify(id, "Carry a defeated catch to the weighing tray.");
        return;
      }
      if (f.species === 4) {
        this.notify(id, "The Bellheart belongs with the bellkeeper.");
        return;
      }
      this.coins += f.value;
      this.sold++;
      this.fish.delete(f.id);
      p.carrying = null;
      if (this.sold >= 3 && this.quest < 2) {
        this.quest = 2;
        this.notify(
          null,
          "Resonant lure unlocked · Press B with your rod to call Bellmaw.",
        );
      }
      this.notify(id, `Sold ${SPECIES[f.species].name} · +${f.value} crowns`);
      return;
    }
    if (m.type === "buy") {
      if (!this.near(p, "knife")) return;
      if (p.knife) {
        this.notify(id, "You already own an iron knife.");
        return;
      }
      if (this.coins < 24) {
        this.notify(id, "The crew needs 24 crowns.");
        return;
      }
      this.coins -= 24;
      p.knife = true;
      p.tool = "knife";
      this.notify(id, "Iron knife equipped · 18 damage");
      return;
    }
    if (m.type === "sharpen") {
      if (!this.near(p, "sharpen")) return;
      if (!p.knife) {
        this.notify(id, "Buy the iron knife first.");
        return;
      }
      if (p.sharp === 2) {
        this.notify(id, "Your blade is fully sharpened.");
        return;
      }
      const price = p.sharp === 0 ? 12 : 28;
      if (this.coins < price) {
        this.notify(id, `The crew needs ${price} crowns.`);
        return;
      }
      this.coins -= price;
      p.sharp++;
      this.notify(id, `Blade sharpened · ${[18, 24, 32][p.sharp]} damage`);
      return;
    }
    if (m.type === "rest" && this.near(p, "rest")) {
      p.hp = 100;
      this.notify(id, "A breath of fresh air. Health restored.");
      return;
    }
    if (m.type === "talk" && this.near(p, "keeper")) {
      const f = this.fish.get(p.carrying);
      if (f?.species === 4 && f.owner === id) {
        this.fish.delete(f.id);
        p.carrying = null;
        this.quest = 3;
        this.coins += 40;
        this.notify(
          null,
          "The wind bell sings! Fairy Lantern Grove chart unlocked.",
        );
      } else if (this.quest === 0) {
        this.quest = 1;
        this.notify(
          id,
          "Orin: Sell three catches. Their scales will make a lure for the creature that swallowed our Bellheart.",
        );
      } else
        this.notify(
          id,
          this.quest === 3
            ? "Orin: You brought the wind home. The next chart is at the dock."
            : "Orin: Fish the spring, defeat your catches, and sell them. The buyer will prepare our special lure.",
        );
      return;
    }
    if (m.type === "revive") {
      const other = this.players.get(m.id);
      if (other && other.hp === 0 && dist(p, other) < 2.7) {
        other.hp = 50;
        this.notify(other.id, p.name + " helped you up.");
      }
    }
  }
  tick(dt) {
    this.time += dt;
    this.tickId++;
    this.events = this.events.filter((e) => this.time - e.at < 0.4);
    for (const p of this.players.values()) {
      if (p.hp <= 0) continue;
      if (this.time - p.lastInput > 0.35) p.input = {};
      movePlayer(p, p.input, dt);
      if (p.carrying) {
        const f = this.fish.get(p.carrying);
        if (f) {
          f.x = p.x - Math.sin(p.yaw) * 1.2;
          f.z = p.z - Math.cos(p.yaw) * 1.2;
        }
      }
      if (p.fishing) {
        const c = p.fishing;
        c.elapsed += dt;
        if (c.phase === "waiting" && c.elapsed > 1.8) {
          c.phase = "hooked";
          this.notify(
            p.id,
            "A bite! Hold left mouse to reel. Ease off when tension rises.",
          );
        }
        if (c.phase === "hooked") {
          const pull = p.input.reel === true;
          c.progress = clamp(c.progress + dt * (pull ? 0.26 : -0.018), 0, 1);
          c.tension = clamp(
            c.tension +
              dt * (pull ? 0.14 + 0.12 * Math.sin(c.elapsed * 3) : -0.48),
            0,
            1,
          );
          if (c.tension >= 1) {
            p.fishing = null;
            this.notify(
              p.id,
              "The line snapped! Your rod is safe. Try easing off sooner.",
            );
          } else if (c.progress >= 1) {
            const roll = Math.random();
            const species = c.boss
              ? 4
              : p.catches < 2
                ? 0
                : roll < 0.5
                  ? 0
                  : roll < 0.8
                    ? 1
                    : roll < 0.95
                      ? 2
                      : 3;
            const data = SPECIES[species];
            const f = {
              id: randomUUID(),
              species,
              hp: data.hp,
              maxHp: data.hp,
              value: data.value,
              x: p.x - Math.sin(p.yaw) * 2,
              z: p.z - Math.cos(p.yaw) * 2,
              owner: null,
              state: "alive",
              born: this.time,
              attackAt: this.time + 1.5,
              hitAt: -1,
              angle: p.yaw,
            };
            if (!walkable(f.x, f.z)) {
              f.x = p.x + Math.cos(p.yaw) * 1.8;
              f.z = p.z - Math.sin(p.yaw) * 1.8;
            }
            if (!walkable(f.x, f.z)) {
              f.x = p.x;
              f.z = p.z;
            }
            this.fish.set(f.id, f);
            p.catches++;
            p.fishing = null;
            p.tool = p.knife ? "knife" : "fists";
            this.notify(
              p.id,
              `${data.name} landed alive! Attack with left mouse.`,
            );
          }
        }
      }
    }
    for (const f of this.fish.values()) {
      if (f.hp <= 0 || f.owner) continue;
      const data = SPECIES[f.species];
      let target = null,
        d = 14;
      for (const p of this.players.values()) {
        const distance = dist(p, f);
        if (p.hp > 0 && distance < d) {
          target = p;
          d = distance;
        }
      }
      if (!target || this.time - f.born < 1) continue;
      f.angle = Math.atan2(target.x - f.x, target.z - f.z);
      if (d > 1.3) {
        const nx = f.x + ((target.x - f.x) / d) * data.speed * dt,
          nz = f.z + ((target.z - f.z) / d) * data.speed * dt;
        if (walkable(nx, nz)) {
          f.x = nx;
          f.z = nz;
        }
      }
      if (d < 1.8 && this.time > f.attackAt) {
        target.hp = Math.max(0, target.hp - data.damage);
        f.attackAt = this.time + 1.45;
        this.events.push({ type: "hurt", id: target.id, at: this.time });
        if (!target.hp) {
          target.downAt = this.time;
          target.fishing = null;
          const carried = this.fish.get(target.carrying);
          if (carried) carried.owner = null;
          target.carrying = null;
          this.notify(
            target.id,
            "You are down! A friend can help, or press R after 4 seconds.",
          );
        }
      }
    }
  }
  snapshot() {
    return {
      type: "state",
      code: this.code,
      time: this.time,
      tick: this.tickId,
      coins: this.coins,
      sold: this.sold,
      quest: this.quest,
      players: [...this.players.values()].map(
        ({ input, lastInput, ...p }) => p,
      ),
      fish: [...this.fish.values()],
      events: this.events,
    };
  }
}
