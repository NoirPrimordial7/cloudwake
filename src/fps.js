import * as T from "three";
import { environment, makeHands, makeFish, avatar, mesh } from "./fps-world.js";
import { POOL, STATIONS, SPECIES, height, clamp } from "../shared/game.js";
import "./fps.css";
const $ = (id) => document.getElementById(id),
  canvas = $("world");
const scene = new T.Scene();
scene.background = new T.Color("#9dcace");
scene.fog = new T.Fog("#b2ced0", 85, 225);
const camera = new T.PerspectiveCamera(68, innerWidth / innerHeight, 0.06, 700);
camera.rotation.order = "YXZ";
scene.add(camera);
const renderer = new T.WebGLRenderer({
  canvas,
  antialias: true,
  powerPreference: "high-performance",
});
renderer.setSize(innerWidth, innerHeight);
renderer.setPixelRatio(Math.min(devicePixelRatio, 1.5));
renderer.shadowMap.enabled = true;
renderer.shadowMap.type = T.PCFShadowMap;
renderer.toneMapping = T.ACESFilmicToneMapping;
renderer.toneMappingExposure = 1.08;
renderer.outputColorSpace = T.SRGBColorSpace;
const hands = makeHands(camera);
hands.root.visible = false;
let env,
  ready = false,
  started = false,
  paused = false,
  ws,
  playerId,
  roomState = null,
  self = null,
  code = "",
  yaw = 0,
  pitch = -0.06,
  time = 0,
  last = performance.now(),
  attackTime = -5,
  chargeStart = null,
  mouseHeld = false,
  drag = false,
  lastPointer = null;
const keys = new Set(),
  fishMeshes = new Map(),
  players = new Map(),
  labels = new Map();
const drawPos = new T.Vector3(2, height(2, 20) + 1.65, 20);
let focus = null,
  lastHurt = -1,
  toastTimer,
  streamTimer;
const ray = new T.Raycaster();
let line = null,
  bobber = null,
  catchDisplay;
function send(m) {
  if (ws?.readyState === 1) ws.send(JSON.stringify(m));
}
function toast(text) {
  $("toast").textContent = text;
  $("toast").classList.add("show");
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => $("toast").classList.remove("show"), 5500);
}
let audio;
function sound(freq = 440) {
  if (!audio) return;
  const o = audio.createOscillator(),
    g = audio.createGain();
  o.type = "sine";
  o.frequency.value = freq;
  g.gain.setValueAtTime(0.03, audio.currentTime);
  g.gain.exponentialRampToValueAtTime(0.001, audio.currentTime + 0.18);
  o.connect(g);
  g.connect(audio.destination);
  o.start();
  o.stop(audio.currentTime + 0.2);
}
function lock() {
  try {
    canvas
      .requestPointerLock()
      ?.catch(() =>
        toast(
          "Drag to look here. Open the game in a full browser window for mouse lock.",
        ),
      );
  } catch {}
}
function escapeHTML(text) {
  return String(text).replace(
    /[&<>"']/g,
    (c) =>
      ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" })[
        c
      ],
  );
}
function connect(joinCode = "") {
  if (!ready || ws?.readyState === 1) return;
  $("loadText").textContent = "Preparing your crew…";
  $("start").disabled = true;
  $("join").disabled = true;
  ws = new WebSocket(
    `${location.protocol === "https:" ? "wss:" : "ws:"}//${location.host}/game`,
  );
  ws.onopen = () =>
    send({ type: "join", name: $("name").value || "Sailor", code: joinCode });
  ws.onmessage = (e) => {
    const m = JSON.parse(e.data);
    if (m.type === "welcome") {
      playerId = m.id;
      code = m.code;
      started = true;
      paused = false;
      $("welcome").classList.add("hidden");
      $("hud").classList.remove("hidden");
      $("reconnect").classList.add("hidden");
      $("crewCode").textContent = code;
      try {
        audio ??= new AudioContext();
        audio.resume();
      } catch {}
      toast("Welcome to Wind Bell Harbor. Visit Orin, or try the spring pond.");
      lock();
    } else if (m.type === "state") {
      roomState = m;
      self = m.players.find((p) => p.id === playerId);
      updateHUD();
    } else if (m.type === "notice") {
      toast(m.text);
      sound(540);
    } else if (m.type === "error") {
      if (!started) {
        $("loadText").textContent = m.text;
        $("start").disabled = false;
        $("join").disabled = false;
        ws.close();
      } else toast(m.text);
    }
  };
  ws.onclose = () => {
    if (started) {
      keys.clear();
      mouseHeld = false;
      $("reconnect").classList.remove("hidden");
      document.exitPointerLock?.();
    } else {
      $("start").disabled = false;
      $("join").disabled = false;
    }
  };
  clearInterval(streamTimer);
  streamTimer = setInterval(() => {
    if (!self) return;
    send({
      type: "input",
      x: paused ? 0 : Number(keys.has("d")) - Number(keys.has("a")),
      z: paused ? 0 : Number(keys.has("w")) - Number(keys.has("s")),
      run: keys.has("shift"),
      yaw,
      pitch,
      reel: !paused && mouseHeld,
    });
  }, 50);
}
function updateHUD() {
  if (!self) return;
  $("money").textContent = roomState.coins;
  $("hp").textContent = self.hp;
  $("healthFill").style.width = self.hp + "%";
  $("crew").innerHTML = roomState.players
    .filter((p) => p.id !== playerId)
    .map(
      (p) =>
        `<span>${escapeHTML(p.name)} · ${p.hp > 0 ? p.hp + " HP" : "DOWNED"}</span>`,
    )
    .join("");
  const q = [
    ["The Missing Bellheart", "Find Orin at the wind bell."],
    [
      "A lure for the culprit",
      `Sell ordinary catches · ${Math.min(roomState.sold, 3)} / 3`,
    ],
    [
      "Something in the spring",
      "Rod equipped? Aim at the pond, then B to call Bellmaw.",
    ],
    ["The wind returns", "Fairy Lantern Grove chart unlocked at the dock."],
  ][roomState.quest];
  $("quest").innerHTML = `<strong>${q[0]}</strong>${q[1]}`;
  for (const b of $("slots").querySelectorAll("button"))
    b.classList.toggle("active", b.dataset.tool === self.tool);
  $("knifeSlot").style.opacity = self.knife ? "1" : ".45";
  $("downed").classList.toggle("hidden", self.hp > 0);
  if (self.hp <= 0) document.exitPointerLock?.();
  $("fishing").classList.toggle("hidden", !self.fishing);
  if (self.fishing) {
    $("fishTitle").textContent =
      self.fishing.phase === "waiting"
        ? "Listen to the water…"
        : "A bite. Bring it ashore.";
    $("fishHelp").textContent =
      self.fishing.phase === "waiting"
        ? "Your lure is settling."
        : "Hold left mouse to reel · release to ease tension";
    $("reelProgress").style.width = self.fishing.progress * 100 + "%";
    $("tension").style.width = self.fishing.tension * 100 + "%";
    $("tension").style.background =
      self.fishing.tension > 0.8 ? "#e75b4c" : "#e5b579";
  }
  const hurt = roomState.events.find(
    (e) => e.type === "hurt" && e.id === playerId && e.at > lastHurt,
  );
  if (hurt) {
    lastHurt = hurt.at;
    $("damage").style.opacity = ".65";
    setTimeout(() => ($("damage").style.opacity = "0"), 180);
    sound(130);
  }
}
function aimPool() {
  ray.setFromCamera(new T.Vector2(), camera);
  const hit = new T.Vector3();
  if (
    ray.ray.intersectPlane(new T.Plane(new T.Vector3(0, 1, 0), -POOL.y), hit) &&
    hit.distanceTo(camera.position) < 28
  )
    return { x: hit.x, z: hit.z };
  camera.getWorldDirection(hit);
  return { x: self.x + hit.x * 12, z: self.z + hit.z * 12 };
}
function cast(boss = false) {
  if (!self || paused) return;
  send({ type: "cast", ...aimPool(), boss });
  sound(660);
}
function equip(tool) {
  if (tool === "knife" && !self?.knife) {
    toast("Buy the iron knife at Bram’s Forge.");
    return;
  }
  send({ type: "equip", tool });
}
function attack() {
  if (!self || paused || self.hp <= 0) return;
  if (self.tool === "rod") {
    if (!self.fishing) cast();
    return;
  }
  attackTime = time;
  send({ type: "attack" });
  sound(230);
}
function showMenu(title, body) {
  paused = true;
  keys.clear();
  mouseHeld = false;
  send({ type: "cancel" });
  document.exitPointerLock?.();
  $("menuTitle").textContent = title;
  $("menuBody").innerHTML = body;
  $("menu").classList.remove("hidden");
}
function closeMenu() {
  paused = false;
  $("menu").classList.add("hidden");
  lock();
}
function interact() {
  if (!focus || paused || !self) return;
  if (focus.kind === "fish") send({ type: "pickup", id: focus.id });
  else if (focus.kind === "player") send({ type: "revive", id: focus.id });
  else if (focus.key === "chart")
    showMenu(
      "A sky worth wandering",
      `<p><strong>01 · Wind Bell Harbor</strong><br>Your crew’s home port.</p><p><strong>02 · Fairy Lantern Grove</strong><br>${roomState.quest === 3 ? "Chart discovered. This island is the next chapter, not playable in this build." : "Restore the Wind Bell to reveal this chart."}</p>`,
    );
  else
    send({
      type: {
        seller: "sell",
        knife: "buy",
        sharpen: "sharpen",
        keeper: "talk",
        rest: "rest",
      }[focus.key],
    });
}
function chooseFocus() {
  if (!self) return null;
  const forward = new T.Vector3();
  camera.getWorldDirection(forward);
  let best = null,
    score = -2;
  const consider = (item, x, y, z, range) => {
    const diff = new T.Vector3(x, y, z).sub(camera.position),
      d = diff.length();
    if (d > range) return;
    const dot = diff.normalize().dot(forward);
    if (dot < 0.45) return;
    const weight = dot - d * 0.025;
    if (weight > score) {
      score = weight;
      best = item;
    }
  };
  for (const [key, s] of Object.entries(STATIONS))
    consider(
      { kind: "station", key, label: s.label },
      s.x,
      height(s.x, s.z) + (key === "keeper" ? 1.45 : 1.2),
      s.z,
      s.range + 1,
    );
  for (const f of roomState.fish) {
    if (f.owner) continue;
    consider(
      { kind: "fish", id: f.id, label: SPECIES[f.species].name, fish: f },
      f.x,
      height(f.x, f.z) + 0.45,
      f.z,
      4.2,
    );
  }
  for (const p of roomState.players)
    if (p.id !== playerId && p.hp === 0)
      consider(
        { kind: "player", id: p.id, label: "Help " + p.name },
        p.x,
        height(p.x, p.z) + 0.5,
        p.z,
        3,
      );
  return best;
}
function updateFocus() {
  focus = chooseFocus();
  $("crosshair").classList.toggle("target", !!focus);
  $("focus").classList.toggle("hidden", !focus || !!self?.fishing);
  $("itemCard").classList.add("hidden");
  $("targetHealth").classList.add("hidden");
  if (!focus) return;
  const f = focus.fish,
    sub =
      focus.kind === "fish"
        ? f.hp > 0
          ? "Left mouse · Attack"
          : self.carrying
            ? "Already carrying a catch"
            : "E · Carry catch"
        : focus.kind === "player"
          ? "E · Revive"
          : "E · Interact";
  $("focus").innerHTML =
    `<strong>${escapeHTML(focus.label)}</strong><small>${sub}</small>`;
  if (f?.hp > 0) {
    $("targetHealth").classList.remove("hidden");
    $("targetHealth").innerHTML =
      `${escapeHTML(SPECIES[f.species].name)}<div><i style="width:${(f.hp / f.maxHp) * 100}%"></i></div>`;
  }
  if (["knife", "sharpen", "seller"].includes(focus.key)) {
    let title, desc, price, action;
    if (focus.key === "knife") {
      title = "IRON KNIFE";
      desc = "A dependable harbor blade.<br>Damage 8 → 18";
      price = self.knife ? "Owned" : "24 crowns";
      action = self.knife ? "3 · Equip" : "E · Buy";
    }
    if (focus.key === "sharpen") {
      title = "THE WHETSTONE";
      desc = self.knife
        ? `Damage ${[18, 24, 32][self.sharp]} → ${[24, 32, 32][self.sharp]}`
        : "Bring an iron knife to sharpen.";
      price =
        self.sharp === 2
          ? "Fully sharpened"
          : (self.sharp ? 28 : 12) + " crowns";
      action = "E · Sharpen";
    }
    if (focus.key === "seller") {
      const f = roomState.fish.find((f) => f.id === self.carrying);
      title = "WEIGH YOUR CATCH";
      desc = f
        ? SPECIES[f.species].name
        : "Defeat a catch, carry it here,<br>then place it on the scales.";
      price = f ? f.value + " crowns" : "No catch carried";
      action = "E · Sell carried catch";
    }
    $("itemCard").classList.remove("hidden");
    $("itemCard").innerHTML =
      `<span class="eyebrow">WIND BELL HARBOR</span><h3>${title}</h3><p>${desc}</p><div class="price">${price}</div><small>${action}</small>`;
  }
}
function renderActors(dt) {
  for (const f of roomState.fish) {
    let obj = fishMeshes.get(f.id);
    if (!obj) {
      obj = makeFish(f.species);
      scene.add(obj);
      fishMeshes.set(f.id, obj);
      obj.position.set(f.x, height(f.x, f.z) + 0.5, f.z);
    }
    const carried = f.owner,
      y =
        height(f.x, f.z) +
        (carried
          ? 1.15
          : f.hp > 0
            ? 0.55 + Math.abs(Math.sin(time * 4 + f.born)) * 0.15
            : 0.24);
    obj.visible = f.owner !== playerId;
    obj.position.lerp(new T.Vector3(f.x, y, f.z), 1 - Math.exp(-dt * 18));
    obj.rotation.y = f.angle;
    obj.rotation.z = f.hp > 0 ? Math.sin(time * 5) * 0.1 : Math.PI / 2;
    obj.scale.setScalar(carried ? 0.65 : 1);
    if (obj.userData.tail)
      obj.userData.tail.rotation.y = Math.sin(time * 8) * 0.24;
    if (f.owner === playerId) {
      if (!catchDisplay || catchDisplay.userData.species !== f.species) {
        if (catchDisplay) camera.remove(catchDisplay);
        catchDisplay = makeFish(f.species);
        catchDisplay.userData.species = f.species;
        camera.add(catchDisplay);
      }
      catchDisplay.position.set(0, -0.38, -0.9);
      catchDisplay.rotation.set(0, Math.PI / 2, 0);
      catchDisplay.scale.setScalar(0.55);
      catchDisplay.visible = true;
    }
  }
  if (catchDisplay && !self.carrying) catchDisplay.visible = false;
  for (const [id, obj] of fishMeshes)
    if (!roomState.fish.some((f) => f.id === id)) {
      scene.remove(obj);
      fishMeshes.delete(id);
    }
  for (const p of roomState.players) {
    if (p.id === playerId) continue;
    let obj = players.get(p.id);
    if (!obj) {
      obj = avatar("#4e8177");
      players.set(p.id, obj);
      scene.add(obj);
      const el = document.createElement("div");
      el.className = "world-label";
      el.textContent = p.name;
      $("worldLabels").append(el);
      labels.set(p.id, el);
    }
    obj.position.lerp(
      new T.Vector3(p.x, height(p.x, p.z), p.z),
      1 - Math.exp(-dt * 17),
    );
    obj.rotation.y = p.yaw + Math.PI;
    obj.rotation.z = p.hp > 0 ? 0 : Math.PI / 2;
    const point = obj.position
        .clone()
        .add(new T.Vector3(0, 2.1, 0))
        .project(camera),
      el = labels.get(p.id);
    el.style.display = point.z > 1 ? "none" : "block";
    el.style.left = (point.x * 0.5 + 0.5) * innerWidth + "px";
    el.style.top = (-point.y * 0.5 + 0.5) * innerHeight + "px";
  }
  for (const [id, obj] of players)
    if (!roomState.players.some((p) => p.id === id)) {
      scene.remove(obj);
      players.delete(id);
      labels.get(id)?.remove();
      labels.delete(id);
    }
  const f = self.fishing;
  if (f) {
    if (!line) {
      line = new T.Line(
        new T.BufferGeometry(),
        new T.LineBasicMaterial({ color: "#eadbb3" }),
      );
      scene.add(line);
      bobber = mesh(scene, new T.SphereGeometry(0.09, 12, 8), "#cf6651");
    }
    const from = camera.localToWorld(new T.Vector3(0.1, 0.4, -1.7)),
      to = new T.Vector3(f.x, POOL.y + 0.06 + Math.sin(time * 4) * 0.025, f.z),
      mid = from.clone().lerp(to, 0.5);
    mid.y -= 0.5;
    const curve = new T.QuadraticBezierCurve3(from, mid, to);
    line.geometry.dispose();
    line.geometry = new T.BufferGeometry().setFromPoints(curve.getPoints(30));
    bobber.position.copy(to);
    line.visible = true;
    bobber.visible = true;
  } else {
    if (line) line.visible = false;
    if (bobber) bobber.visible = false;
  }
}
function frame() {
  requestAnimationFrame(frame);
  const now = performance.now(),
    dt = Math.min((now - last) / 1000, 0.05);
  last = now;
  time += dt;
  if (env) {
    env.waterMat.uniforms.time.value = time;
    env.bell.rotation.z =
      roomState?.quest === 3 ? Math.sin(time * 2) * 0.12 : 0;
  }
  if (!started) {
    camera.position.set(21 + Math.sin(time * 0.07) * 2, 12, 26);
    camera.lookAt(-5, 2, -7);
  } else if (self) {
    drawPos.lerp(
      new T.Vector3(self.x, height(self.x, self.z) + 1.65, self.z),
      1 - Math.exp(-dt * 20),
    );
    camera.position.copy(drawPos);
    camera.rotation.set(pitch, yaw, 0, "YXZ");
    hands.root.visible = !self.carrying && self.hp > 0;
    hands.rod.visible = self.tool === "rod";
    hands.knife.visible = self.tool === "knife";
    const swing = Math.max(0, 1 - (time - attackTime) / 0.4);
    hands.right.rotation.x = -0.2 - Math.sin(swing * Math.PI) * 1.1;
    hands.right.rotation.z = 0.12 + Math.sin(swing * Math.PI) * 0.5;
    hands.root.position.y = Math.sin(time * 6) * 0.003;
    renderActors(dt);
    if (!paused) updateFocus();
  }
  renderer.render(scene, camera);
}
frame();
environment(scene, (p) => {
  if (Number.isFinite(p))
    $("loadText").textContent =
      "Building the harbor… " + Math.round(p * 100) + "%";
})
  .then((r) => {
    env = r;
    ready = true;
    $("loadText").textContent = "Harbor ready · First-person co-op prototype";
    $("start").disabled = false;
    $("join").disabled = false;
  })
  .catch((e) => {
    $("loadText").textContent = "Could not load the island: " + e.message;
    console.error(e);
  });
$("startForm").onsubmit = (e) => {
  e.preventDefault();
  connect();
};
$("join").onclick = () => connect($("roomInput").value);
$("roomInput").value = new URLSearchParams(location.search).get("crew") || "";
$("crewButton").onclick = () =>
  showMenu(
    "Your crew",
    `<p>Invite up to three friends using this room code:</p><h2>${code}</h2><p>Everyone must open this same server address.<br>Shared money, catches and story. The room closes when the last player leaves.</p>`,
  );
$("copyInvite").onclick = async () => {
  try {
    await navigator.clipboard.writeText(location.origin + "/?crew=" + code);
    toast("Crew invite copied.");
  } catch {
    toast("Crew code: " + code);
  }
};
$("menuButton").onclick = () =>
  showMenu(
    "A moment ashore",
    `<p><strong>WASD</strong> move · <strong>Mouse</strong> look · <strong>E</strong> interact<br><strong>1</strong> rod · <strong>2</strong> fists · <strong>3</strong> knife<br><strong>Left mouse</strong> cast / reel / attack<br><strong>G</strong> drop catch · <strong>B</strong> boss lure<br><strong>Shift</strong> run · <strong>Esc</strong> menu</p><p>The shared world keeps running while menus are open. Rest on the bench to restore health.</p>`,
  );
$("resume").onclick = closeMenu;
$("leave").onclick = () => (location.href = location.pathname);
$("reload").onclick = () => location.reload();
$("respawn").onclick = () => send({ type: "respawn" });
for (const b of $("slots").querySelectorAll("button"))
  b.onclick = () => {
    equip(b.dataset.tool);
    lock();
  };
document.addEventListener("keydown", (e) => {
  if (["INPUT", "TEXTAREA"].includes(document.activeElement.tagName)) return;
  const k = e.key.toLowerCase();
  if (e.repeat) return;
  if (k === "escape") {
    if (started) {
      if (paused) closeMenu();
      else $("menuButton").click();
    }
    return;
  }
  if (!started || paused) return;
  keys.add(k);
  if (k === "e") interact();
  if (k === "1") equip("rod");
  if (k === "2") equip("fists");
  if (k === "3") equip("knife");
  if (k === "g") send({ type: "drop" });
  if (k === "b") cast(true);
  if (k === "f") cast();
  if (k === "r") send({ type: "respawn" });
});
document.addEventListener("keyup", (e) => keys.delete(e.key.toLowerCase()));
canvas.addEventListener("mousedown", (e) => {
  if (!started || paused || e.button !== 0) return;
  if (document.pointerLockElement !== canvas) {
    lock();
    drag = true;
    lastPointer = { x: e.clientX, y: e.clientY };
    return;
  }
  mouseHeld = true;
  if (self?.tool === "rod" && !self.fishing) chargeStart = time;
  else if (self?.tool !== "rod") attack();
});
document.addEventListener("mouseup", () => {
  if (chargeStart !== null) {
    cast();
    chargeStart = null;
  }
  mouseHeld = false;
  drag = false;
});
document.addEventListener("mousemove", (e) => {
  if (!started || paused) return;
  let dx, dy;
  if (document.pointerLockElement === canvas) {
    dx = e.movementX;
    dy = e.movementY;
  } else if (drag && lastPointer) {
    dx = e.clientX - lastPointer.x;
    dy = e.clientY - lastPointer.y;
    lastPointer = { x: e.clientX, y: e.clientY };
  } else return;
  yaw -= dx * 0.0024;
  pitch = clamp(pitch - dy * 0.0024, -1.25, 1.25);
});
canvas.addEventListener("contextmenu", (e) => e.preventDefault());
window.addEventListener("blur", () => {
  keys.clear();
  mouseHeld = false;
  chargeStart = null;
  send({ type: "cancel" });
});
window.addEventListener("resize", () => {
  camera.aspect = innerWidth / innerHeight;
  camera.updateProjectionMatrix();
  renderer.setSize(innerWidth, innerHeight);
});
window.cloudwake = {
  getSnapshot: () => ({
    ready,
    started,
    paused,
    id: playerId,
    code,
    self,
    room: roomState,
    focus,
    drawCalls: renderer.info.render.calls,
  }),
  getCamera: () => ({ yaw, pitch, position: camera.position.toArray() }),
};
