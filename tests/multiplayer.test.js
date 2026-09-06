import { test } from "node:test";
import assert from "node:assert/strict";
import { GameRoom } from "../server/simulation.js";
import { STATIONS } from "../shared/game.js";
test("one catch has one owner and can only be sold once", () => {
  const r = new GameRoom("TEST01"),
    a = r.join("a", "A"),
    b = r.join("b", "B");
  Object.assign(a, STATIONS.seller);
  Object.assign(b, STATIONS.seller);
  r.fish.set("f", {
    id: "f",
    species: 0,
    hp: 0,
    value: 8,
    x: a.x,
    z: a.z,
    owner: null,
  });
  r.action("a", { type: "pickup", id: "f" });
  r.action("b", { type: "pickup", id: "f" });
  assert.equal(a.carrying, "f");
  assert.equal(b.carrying, null);
  r.action("b", { type: "sell" });
  assert.equal(r.coins, 0);
  r.action("a", { type: "sell" });
  r.action("a", { type: "sell" });
  assert.equal(r.coins, 8);
  assert.equal(r.sold, 1);
  assert.equal(r.fish.size, 0);
});
test("fishing lands alive, combat kills, and live catches cannot be carried", () => {
  const r = new GameRoom("TEST02"),
    p = r.join("a", "A");
  p.x = -1;
  p.z = 5;
  r.action("a", { type: "cast", x: -12, z: 5 });
  assert.ok(p.fishing);
  for (let i = 0; i < 130; i++) {
    r.action("a", { type: "input", reel: true });
    r.tick(0.05);
  }
  assert.equal(r.fish.size, 1);
  const f = [...r.fish.values()][0];
  assert.equal(f.hp, 16);
  r.action("a", { type: "pickup", id: f.id });
  assert.equal(p.carrying, null);
  p.yaw = Math.atan2(-(f.x - p.x), -(f.z - p.z));
  r.action("a", { type: "attack" });
  r.tick(0.6);
  p.yaw = Math.atan2(-(f.x - p.x), -(f.z - p.z));
  r.action("a", { type: "attack" });
  assert.equal(f.hp, 0);
  r.action("a", { type: "pickup", id: f.id });
  assert.equal(p.carrying, f.id);
});
test("shop enforces proximity, price and upgrade cap", () => {
  const r = new GameRoom("TEST03"),
    p = r.join("a", "A");
  r.coins = 100;
  r.action("a", { type: "buy" });
  assert.equal(p.knife, false);
  assert.equal(r.coins, 100);
  Object.assign(p, STATIONS.knife);
  r.action("a", { type: "buy" });
  r.action("a", { type: "buy" });
  assert.equal(p.knife, true);
  assert.equal(r.coins, 76);
  Object.assign(p, STATIONS.sharpen);
  for (let i = 0; i < 3; i++) r.action("a", { type: "sharpen" });
  assert.equal(p.sharp, 2);
  assert.equal(r.coins, 36);
});
test("four player limit and disconnect releases carried catches", () => {
  const r = new GameRoom("TEST04");
  for (let i = 0; i < 4; i++) r.join(String(i), "A");
  assert.throws(() => r.join("5", "Full"));
  const p = r.players.get("0");
  p.carrying = "f";
  r.fish.set("f", { owner: "0" });
  r.leave("0");
  assert.equal(r.fish.get("f").owner, null);
  r.join("5", "New");
  assert.equal(r.players.size, 4);
});
test("Bellheart reward is granted once and unlocks chart", () => {
  const r = new GameRoom("TEST05"),
    p = r.join("a", "A");
  Object.assign(p, STATIONS.keeper);
  r.quest = 2;
  p.carrying = "boss";
  r.fish.set("boss", { species: 4, hp: 0, owner: "a" });
  r.action("a", { type: "talk" });
  r.action("a", { type: "talk" });
  assert.equal(r.quest, 3);
  assert.equal(r.coins, 40);
});
