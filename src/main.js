import * as T from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";
import { TransformControls } from "three/addons/controls/TransformControls.js";
import { GLTFExporter } from "three/addons/exporters/GLTFExporter.js";
import { GLTFLoader } from "three/addons/loaders/GLTFLoader.js";
import {
  makeWorld,
  makeClouds,
  makeCharacter,
  makeWisp,
  COLORS,
  ball,
  batchStatic,
} from "./world.js";
import {
  freshState,
  sanitizeSave,
  SAVE_KEY,
  catchWisp,
  restoreBell,
  sellWisps,
  buyCharm,
} from "./state.js";
import "./style.css";

const $ = (id) => document.getElementById(id);
let state;
try {
  state = sanitizeSave(JSON.parse(localStorage.getItem(SAVE_KEY)));
} catch {
  state = freshState();
}
let storageWarning = false;
function save() {
  try {
    localStorage.setItem(SAVE_KEY, JSON.stringify(state));
  } catch {
    if (!storageWarning) {
      toast("Browser storage unavailable. Progress lasts for this session.");
      storageWarning = true;
    }
  }
  updateHUD();
}
const scene = new T.Scene();
scene.background = new T.Color(0x9bc9ce);
scene.fog = new T.Fog(0xadcdd0, 90, 235);
const camera = new T.PerspectiveCamera(48, innerWidth / innerHeight, 0.1, 500);
let renderer;
try {
  renderer = new T.WebGLRenderer({
    canvas: $("world"),
    antialias: true,
    powerPreference: "high-performance",
  });
} catch (err) {
  $("welcome").innerHTML =
    '<div class="intro"><h2>WebGL is unavailable</h2><p>Please open Cloudwake in a browser with hardware acceleration enabled.</p></div>';
  throw err;
}
renderer.setSize(innerWidth, innerHeight);
renderer.setPixelRatio(Math.min(devicePixelRatio, 1.7));
renderer.shadowMap.enabled = true;
renderer.shadowMap.type = T.PCFShadowMap;
renderer.outputColorSpace = T.SRGBColorSpace;
renderer.toneMapping = T.ACESFilmicToneMapping;
renderer.toneMappingExposure = 1.18;
scene.add(new T.HemisphereLight(0xd6f0ed, 0xa49b70, 2.5));
const sun = new T.DirectionalLight(0xffe0a1, 3.4);
sun.position.set(-30, 55, 24);
sun.castShadow = true;
sun.shadow.mapSize.set(2048, 2048);
Object.assign(sun.shadow.camera, {
  left: -45,
  right: 45,
  top: 45,
  bottom: -45,
  near: 1,
  far: 130,
});
sun.shadow.bias = -0.0005;
sun.shadow.normalBias = 0.035;
scene.add(sun);
const world = makeWorld(scene),
  clouds = makeClouds(scene);
const player = makeCharacter(scene, "Player");
player.position.set(0, 0.2, 20);
player.rotation.y = Math.PI;
batchStatic(clouds, true);
const wildWisps = [];
for (let i = 0; i < 9; i++) {
  let w = makeWisp(scene, [0xe9f4cf, 0xe5d69a, 0xc4e2df][i % 3]);
  w.position.set(-30 - i * 2, -1 + Math.sin(i), 6 + i * 2);
  w.userData.base = w.position.clone();
  wildWisps.push(w);
}
const deckWisps = [];
const orbit = new OrbitControls(camera, renderer.domElement);
orbit.enabled = false;
orbit.enableDamping = true;
orbit.maxDistance = 110;
orbit.minDistance = 6;
orbit.maxPolarAngle = Math.PI * 0.48;
const transform = new TransformControls(camera, renderer.domElement);
scene.add(transform.getHelper());
transform.addEventListener(
  "dragging-changed",
  (e) => (orbit.enabled = !e.value && editing),
);
let started = false,
  paused = false,
  editing = false,
  aboard = false,
  sailing = false,
  yaw = 0,
  pitch = 0.38,
  zoom = 12,
  jump = 0,
  vy = 0,
  speed = 0;
let time = 0,
  catching = null,
  nearby = null,
  toastTimer,
  dragging = false,
  dragX = 0,
  dragY = 0;
const keys = new Set();
const boat = world.boat;
let audio;
function chime(freq = 440) {
  if (!audio) return;
  const o = audio.createOscillator(),
    g = audio.createGain();
  o.type = "sine";
  o.frequency.value = freq;
  g.gain.setValueAtTime(0.055, audio.currentTime);
  g.gain.exponentialRampToValueAtTime(0.001, audio.currentTime + 0.65);
  o.connect(g);
  g.connect(audio.destination);
  o.start();
  o.stop(audio.currentTime + 0.7);
}
function toast(s) {
  $("toast").textContent = s;
  $("toast").classList.add("visible");
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => $("toast").classList.remove("visible"), 4200);
}
const quests = [
  ["A harbor without wind", "Find the bellkeeper beside the tower."],
  ["Wake the Wind Bell", "Catch 3 cloud wisps, then return to the bellkeeper."],
  ["A new wind is rising", "Your next map is ready. Visit the mapmaker."],
];
function updateHUD() {
  $("wallet").textContent = state.coins + " crowns";
  $("inventory").textContent = state.wisps + " wisps";
  $("questTitle").textContent = quests[state.quest][0];
  $("questText").textContent = quests[state.quest][1];
  $("questFill").style.width =
    (state.quest === 2
      ? 100
      : state.quest === 1
        ? 15 + (Math.min(state.wisps, 3) / 3) * 70
        : 5) + "%";
  $("mode").textContent = sailing
    ? "AT THE HELM"
    : aboard
      ? "ON DECK"
      : "ON FOOT";
  $("area").textContent = sailing ? "The Open Clouds" : "Wind Bell Harbor";
  world.bell.children.forEach((m) => {
    if (m.material && m.material.color) {
      if (!m.userData.uniqueBellMaterial) {
        m.material = m.material.clone();
        m.userData.uniqueBellMaterial = true;
      }
      m.material.emissive = new T.Color(0x735721);
      m.material.emissiveIntensity = state.quest === 2 ? 0.3 : 0;
    }
  });
}
function closeModal() {
  $("modal").classList.add("hidden");
  paused = false;
  keys.clear();
}
function modal(title, body, actions = []) {
  if (catching) cancelCatch();
  paused = true;
  keys.clear();
  $("modalContent").innerHTML =
    `<span class="eyebrow">CLOUDWAKE · WIND BELL HARBOR</span><h2>${title}</h2>${body}<div id="actions"></div>`;
  $("modal").classList.remove("hidden");
  for (const [label, fn] of actions) {
    const b = document.createElement("button");
    b.textContent = label;
    b.onclick = fn;
    $("actions").append(b);
  }
  const b = document.createElement("button");
  b.textContent = "Back to adventure";
  b.onclick = closeModal;
  $("actions").append(b);
}
function keeper() {
  if (state.quest === 0)
    modal(
      "The sky has gone quiet.",
      `<p>“Ah, a new captain! That storm scattered the wisps that used to sing in our wind bell.”</p><p>“Take this casting bell. Bring me <strong>three cloud wisps</strong>, and I’ll wake the harbor’s wind. The old route to the fairy grove should appear again.”</p><p class="muted">Find the little catching pier on the west edge, or cast from your boat.</p>`,
      [
        [
          "Take the casting bell",
          () => {
            state.quest = 1;
            save();
            closeModal();
            toast("Casting bell received · Head to the western cloud pier");
            chime(660);
          },
        ],
      ],
    );
  else if (state.quest === 1 && state.wisps < 3)
    modal(
      "A little more wind…",
      `<p>“You have ${state.wisps} of the three wisps. Cast from the western pier or from your boat. Ring again when the marker reaches the gold band.”</p>`,
    );
  else if (state.quest === 1)
    modal(
      "You brought the wind home.",
      `<p>“Three little voices. One very big song. Shall we?”</p><p>Reward: <strong>30 crowns + the Fairy Lantern Grove map</strong>.</p>`,
      [
        [
          "Restore the wind bell",
          () => {
            if (restoreBell(state)) {
              save();
              closeModal();
              chime(523);
              setTimeout(() => chime(659), 170);
              setTimeout(() => chime(784), 340);
              toast("The wind bell sings again! A new map is waiting.");
            }
          },
        ],
      ],
    );
  else
    modal(
      "Listen. That’s your doing.",
      `<p>“The harbor has its wind again. The mapmaker has marked a route to Fairy Lantern Grove for you.”</p><p>For now, enjoy the harbor. The grove is the next chapter we’ll build.</p>`,
    );
}
function shop(type) {
  if (type === "trade") {
    const extras = Math.max(0, state.wisps - (state.quest === 1 ? 3 : 0));
    modal(
      "Windward Trading",
      `<p>“Cloud wisps keep our lanterns bright. I’ll give you eight crowns for each spare one.”</p><div class="shop-item"><div>Cloud wisps<small>${extras} available · Quest wisps are kept safe</small></div><strong>8 crowns each</strong></div>`,
      [
        [
          "Sell spare wisps",
          () => {
            const n = sellWisps(state);
            save();
            toast(
              n
                ? `Sold ${n} wisps for ${n * 8} crowns`
                : "No spare wisps to sell yet",
            );
            shop("trade");
          },
        ],
      ],
    );
  } else
    modal(
      "Mallow’s Cloud Charms",
      `<p>“A softer ring makes a wisp much easier to coax aboard.”</p><div class="shop-item"><div>Gentle bell charm<small>Widens the catching window permanently</small></div><strong>${state.upgrade ? "Owned" : "16 crowns"}</strong></div>`,
      [
        [
          state.upgrade ? "Charm equipped" : "Buy charm · 16 crowns",
          () => {
            if (buyCharm(state)) {
              save();
              toast("Gentle bell equipped");
              chime(880);
            } else
              toast(
                state.upgrade
                  ? "You already own this charm"
                  : "You need 16 crowns",
              );
            shop("charms");
          },
        ],
        [
          "Customize your flag",
          () => {
            closeModal();
            openEditor();
          },
        ],
      ],
    );
}
function map() {
  modal(
    "A sky worth wandering",
    `<div class="map-route"><h3>01 · Wind Bell Harbor</h3><p>Your home port · ${state.quest === 2 ? "Wind restored" : "Restore the sleeping wind bell"}</p><h3>02 · Fairy Lantern Grove</h3><p>${state.quest === 2 ? "MAP DISCOVERED · Next chapter in development" : "LOCKED · Complete the bellkeeper’s story"}</p></div><p class="muted">This first playable build contains Wind Bell Harbor and its surrounding sky. The next island isn’t playable yet.</p>`,
  );
}
function menu() {
  if (catching) cancelCatch();
  modal(
    "Take a breath, captain.",
    `<p>Your story and purchases save automatically in this browser.</p><p><strong>WASD</strong> walk / steer · <strong>Drag mouse</strong> look<br><strong>E</strong> interact / board / helm · <strong>F</strong> cast / catch<br><strong>Space</strong> jump · <strong>Shift</strong> run · <strong>M</strong> map<br><strong>R</strong> return to dock · <strong>Tab</strong> workshop</p>`,
    [
      [
        "Harbor workshop",
        () => {
          closeModal();
          openEditor();
        },
      ],
      [
        "Return to dock",
        () => {
          closeModal();
          returnDock();
        },
      ],
      [
        "Start a new voyage",
        () =>
          modal(
            "Start fresh?",
            `<p>This resets your story, catches, crowns, and charm. Your edited island layout is kept.</p>`,
            [
              [
                "Reset story",
                () => {
                  state = freshState();
                  deckWisps.forEach((w) => boat.remove(w));
                  deckWisps.length = 0;
                  save();
                  applyFlag();
                  returnDock();
                  closeModal();
                },
              ],
            ],
          ),
      ],
    ],
  );
}
function returnDock() {
  aboard = false;
  sailing = false;
  speed = 0;
  boat.position.set(0, 0.1, 42);
  boat.rotation.y = 0;
  player.position.set(0, 0.2, 32);
  yaw = 0;
  cancelCatch();
  save();
  toast("Back at Wind Bell Harbor");
}
function distance(obj) {
  return Math.hypot(
    player.position.x - obj.position.x,
    player.position.z - obj.position.z,
  );
}
function chooseInteraction() {
  if (sailing)
    return {
      text: "Leave the helm",
      fn: () => {
        sailing = false;
        speed = 0;
        updateHUD();
      },
    };
  if (aboard) {
    const local = boat.worldToLocal(player.position.clone());
    if (local.z < -1.3)
      return {
        text: "Take the helm",
        fn: () => {
          sailing = true;
          yaw = boat.rotation.y + Math.PI + 0.65;
          pitch = 0.58;
          updateHUD();
          toast("W / S sail · A / D steer · E leave helm · R return to dock");
        },
      };
    if (boat.position.distanceTo(new T.Vector3(0, boat.position.y, 42)) < 8)
      return {
        text: "Step onto the dock",
        fn: () => {
          aboard = false;
          player.position.set(0, 0.2, 35);
          updateHUD();
        },
      };
    return null;
  }
  if (distance(world.keeper) < 4)
    return { text: "Talk to the bellkeeper", fn: keeper };
  const shops = world.editable;
  for (const [name, type] of [
    ["Cloud Charms", "charms"],
    ["Windward Trading", "trade"],
  ]) {
    const o = shops.find((o) => o.name === name);
    if (
      player.position.distanceTo(
        o.position.clone().add(new T.Vector3(0, 0, 5.6)),
      ) < 4.5
    )
      return { text: "Visit " + name, fn: () => shop(type) };
  }
  const mm = shops.find((o) => o.name === "Mapmaker");
  if (
    player.position.distanceTo(
      mm.position.clone().add(new T.Vector3(0, 0, 5)),
    ) < 4.5
  )
    return { text: "Talk to the mapmaker", fn: map };
  if (
    Math.abs(player.position.x) < 4 &&
    player.position.z > 33 &&
    distance(boat) < 13
  )
    return {
      text: "Board your skyboat",
      fn: () => {
        aboard = true;
        player.position.copy(boat.localToWorld(new T.Vector3(0, 0.2, 1.8)));
        updateHUD();
        toast("Walk toward the rear wheel and press E to sail");
      },
    };
  return null;
}
function canCast() {
  return (
    state.quest > 0 &&
    (aboard ||
      (player.position.x < -21 && Math.abs(player.position.z - 12) < 3.5))
  );
}
let castLine, castSpirit;
function cancelCatch() {
  catching = null;
  $("catchUI").classList.add("hidden");
  if (castLine) {
    scene.remove(castLine);
    castLine.geometry.dispose();
    castLine.material.dispose();
    castLine = null;
  }
  if (castSpirit) {
    scene.remove(castSpirit);
    castSpirit = null;
  }
}
function cast() {
  if (paused || editing || !started) return;
  if (!canCast()) {
    toast(
      state.quest === 0
        ? "Talk to the bellkeeper to get your casting bell"
        : "Cast from the western pier or your boat",
    );
    return;
  }
  if (!catching) {
    catching = { start: time, phase: "wait", value: 0 };
    $("catchUI").classList.remove("hidden");
    $("catchTitle").textContent = "Listen to the clouds…";
    $("catchHelp").textContent = "Your bell is calling. Wait for a wisp.";
    $("needle").style.left = "0%";
    chime(523);
    return;
  }
  if (catching.phase === "wait") return;
  const left = state.upgrade ? 0.44 : 0.57,
    right = 0.8;
  if (catching.value >= left && catching.value <= right) {
    catchWisp(state);
    save();
    chime(880);
    const w = makeWisp(boat, [0xe9f4cf, 0xe5d69a, 0xc4e2df][state.caught % 3]);
    w.position.set(
      (state.caught % 2 ? 1 : -1) * 1.2,
      0.8,
      (state.caught % 3) - 1,
    );
    w.userData.home = w.position.clone();
    deckWisps.push(w);
    if (deckWisps.length > 6) boat.remove(deckWisps.shift());
    toast(
      `A ${["Pufflet", "Suncrumb", "Mistling"][state.caught % 3]}! ${state.wisps}/3 wisps for the bellkeeper.`,
    );
    cancelCatch();
  } else {
    toast("Almost! The wisp slipped away. Cast again.");
    cancelCatch();
  }
}
function updateCatch() {
  if (!catching) return;
  const age = time - catching.start;
  if (age > 1.2 && catching.phase === "wait") {
    catching.phase = "ring";
    $("catchTitle").textContent = "A wisp answered!";
    $("catchHelp").textContent =
      "Press F when the marker is inside the gold band";
    chime(740);
  }
  if (age > 11) {
    toast("The wisp drifted away. Try another cast.");
    cancelCatch();
    return;
  }
  catching.value = (Math.sin((age - 1.2) * 2.6) + 1) / 2;
  $("needle").style.left = catching.value * 99 + "%";
  $("catchZone").style.left = (state.upgrade ? 44 : 57) + "%";
  $("catchZone").style.width = (state.upgrade ? 36 : 23) + "%";
  const origin = player.position.clone().add(new T.Vector3(0.4, 1.2, 0)),
    end = origin.clone().add(new T.Vector3(-5, -3, 1));
  if (!castLine) {
    castLine = new T.Line(
      new T.BufferGeometry(),
      new T.LineBasicMaterial({ color: 0xffe3a2 }),
    );
    scene.add(castLine);
    castSpirit = makeWisp(scene);
    scene.add(castSpirit);
  }
  const mid = origin
    .clone()
    .lerp(end, 0.5)
    .add(new T.Vector3(0, 2, 0));
  const curve = new T.QuadraticBezierCurve3(origin, mid, end);
  castLine.geometry.dispose();
  castLine.geometry = new T.BufferGeometry().setFromPoints(curve.getPoints(20));
  castSpirit.position.copy(end);
  castSpirit.position.y += Math.sin(time * 4) * 0.3;
}
function applyFlag() {
  const canvas = document.createElement("canvas");
  canvas.width = 256;
  canvas.height = 144;
  const ctx = canvas.getContext("2d");
  ctx.fillStyle = state.flag;
  ctx.fillRect(0, 0, 256, 144);
  ctx.fillStyle = "#f4dfaa";
  ctx.beginPath();
  if (state.emblem === "star") {
    for (let i = 0; i < 10; i++) {
      const a = (i * Math.PI) / 5 - Math.PI / 2,
        r = i % 2 ? 20 : 43;
      ctx.lineTo(128 + Math.cos(a) * r, 72 + Math.sin(a) * r);
    }
    ctx.closePath();
    ctx.fill();
  } else {
    ctx.arc(128, 72, 37, 0, Math.PI * 2);
    ctx.fill();
    if (state.emblem === "moon") {
      ctx.fillStyle = state.flag;
      ctx.beginPath();
      ctx.arc(146, 59, 34, 0, Math.PI * 2);
      ctx.fill();
    } else
      for (let i = 0; i < 8; i++) {
        const a = (i * Math.PI) / 4;
        ctx.fillRect(
          128 + Math.cos(a) * 47 - 3,
          72 + Math.sin(a) * 47 - 3,
          6,
          6,
        );
      }
  }
  const tex = new T.CanvasTexture(canvas);
  tex.colorSpace = T.SRGBColorSpace;
  boat.userData.flag.material.map?.dispose();
  boat.userData.flag.material.map = tex;
  boat.userData.flag.material.color.set(0xffffff);
  boat.userData.flag.material.needsUpdate = true;
}
function layout() {
  return {
    version: 1,
    objects: world.editable.map((o) => ({
      name: o.name,
      position: o.position.toArray(),
      rotation: o.rotation.toArray().slice(0, 3),
      scale: o.scale.toArray(),
    })),
  };
}
function applyLayout(data) {
  if (
    data?.version !== 1 ||
    !Array.isArray(data.objects) ||
    data.objects.length > 100
  )
    throw Error("Not a Cloudwake layout");
  for (const row of data.objects) {
    const o = world.editable.find((o) => o.name === row.name);
    if (!o) continue;
    for (const field of ["position", "rotation", "scale"])
      if (
        !Array.isArray(row[field]) ||
        row[field].length !== 3 ||
        !row[field].every(Number.isFinite)
      )
        throw Error("Invalid transform");
    if (
      row.position.some((v) => Math.abs(v) > 100) ||
      row.scale.some((v) => v < 0.1 || v > 5)
    )
      throw Error("Transform outside supported bounds");
  }
  for (const row of data.objects) {
    const o = world.editable.find((o) => o.name === row.name);
    if (o) {
      o.position.fromArray(row.position);
      o.rotation.set(...row.rotation);
      o.scale.fromArray(row.scale);
    }
  }
}
try {
  const data = JSON.parse(localStorage.getItem("cloudwake-layout-v1"));
  if (data) applyLayout(data);
} catch {
  console.warn("Ignored invalid saved layout");
}
function download(blob, name) {
  const url = URL.createObjectURL(blob),
    a = document.createElement("a");
  a.href = url;
  a.download = name;
  a.click();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}
async function exportGLB() {
  const exportRoot = new T.Scene();
  exportRoot.add(world.root.clone(true), boat.clone(true));
  return new GLTFExporter().parseAsync(exportRoot, {
    binary: true,
    onlyVisible: true,
  });
}
function openEditor() {
  closeModal();
  cancelCatch();
  editing = true;
  paused = true;
  keys.clear();
  $("editor").classList.remove("hidden");
  $("hud").classList.add("hidden");
  orbit.enabled = true;
  orbit.target.set(0, 0, 2);
  camera.position.set(43, 42, 63);
  const select = $("objectSelect");
  select.innerHTML = "";
  world.editable.forEach((o) => {
    const opt = document.createElement("option");
    opt.value = o.name;
    opt.textContent = o.name.replaceAll("_", " ");
    select.append(opt);
  });
  transform.attach(world.editable[0]);
  $("flagColor").value = state.flag;
  $("flagEmblem").value = state.emblem;
}
function closeEditor() {
  editing = false;
  paused = false;
  orbit.enabled = false;
  transform.detach();
  $("editor").classList.add("hidden");
  $("hud").classList.remove("hidden");
}
$("objectSelect").onchange = (e) =>
  transform.attach(world.editable.find((o) => o.name === e.target.value));
for (const mode of ["translate", "rotate", "scale"])
  $(mode).onclick = () => transform.setMode(mode);
$("flagColor").oninput = (e) => {
  state.flag = e.target.value;
  applyFlag();
  save();
};
$("flagEmblem").onchange = (e) => {
  state.emblem = e.target.value;
  applyFlag();
  save();
};
$("saveLayout").onclick = () => {
  try {
    localStorage.setItem("cloudwake-layout-v1", JSON.stringify(layout()));
    toast("Island layout saved");
    $("saveLayout").textContent = "Layout saved";
  } catch {
    toast("Could not save layout. Use Export layout JSON.");
  }
};
$("exportLayout").onclick = () =>
  download(
    new Blob([JSON.stringify(layout(), null, 2)], { type: "application/json" }),
    "cloudwake-layout.json",
  );
$("importLayout").onchange = async (e) => {
  try {
    applyLayout(JSON.parse(await e.target.files[0].text()));
    $("saveLayout").textContent = "Save imported layout";
  } catch (err) {
    alert("Could not import layout: " + err.message);
  }
  e.target.value = "";
};
$("exportGLB").onclick = async () => {
  const b = $("exportGLB");
  b.disabled = true;
  b.textContent = "Exporting meshes…";
  try {
    download(
      new Blob([await exportGLB()], { type: "model/gltf-binary" }),
      "cloudwake-harbor.glb",
    );
  } catch (err) {
    alert("Export failed: " + err.message);
  } finally {
    b.disabled = false;
    b.textContent = "Export scene for Blender";
  }
};
$("closeEditor").onclick = closeEditor;
$("start").onclick = () => {
  started = true;
  $("welcome").classList.add("hidden");
  $("hud").classList.remove("hidden");
  try {
    audio = new AudioContext();
    audio.resume();
  } catch {}
  camera.position.set(0, 6, 31);
  updateHUD();
  toast("Welcome, captain · Walk toward the bell tower to begin");
};
$("settingsBtn").onclick = menu;
$("mapBtn").onclick = map;
$("catchBtn").onclick = cast;
window.addEventListener("keydown", (e) => {
  if (["INPUT", "SELECT", "TEXTAREA"].includes(document.activeElement.tagName))
    return;
  const k = e.key.toLowerCase();
  if ([" ", "tab", "arrowup", "arrowdown"].includes(k)) e.preventDefault();
  if (e.repeat) return;
  if (k === "escape") {
    if (editing) closeEditor();
    else if (!$("modal").classList.contains("hidden")) closeModal();
    else if (started) menu();
    return;
  }
  if (k === "tab" && started) {
    editing ? closeEditor() : openEditor();
    return;
  }
  if (paused || !started) return;
  keys.add(k);
  if (k === "e" && !catching) {
    nearby = chooseInteraction();
    nearby?.fn();
  }
  if (k === "f") cast();
  if (k === "m") map();
  if (k === "r") returnDock();
  if (k === " " && !jump && !sailing) {
    vy = 5.5;
    jump = 0.001;
  }
});
window.addEventListener("keyup", (e) => keys.delete(e.key.toLowerCase()));
window.addEventListener("blur", () => {
  keys.clear();
  dragging = false;
});
renderer.domElement.addEventListener("pointerdown", (e) => {
  if (editing) return;
  dragging = true;
  dragX = e.clientX;
  dragY = e.clientY;
  renderer.domElement.setPointerCapture(e.pointerId);
});
renderer.domElement.addEventListener("pointermove", (e) => {
  if (!dragging || editing) return;
  yaw -= (e.clientX - dragX) * 0.006;
  pitch = T.MathUtils.clamp(pitch + (e.clientY - dragY) * 0.004, 0.1, 1.15);
  dragX = e.clientX;
  dragY = e.clientY;
});
renderer.domElement.addEventListener("pointerup", () => (dragging = false));
renderer.domElement.addEventListener(
  "wheel",
  (e) => {
    if (!editing) zoom = T.MathUtils.clamp(zoom + e.deltaY * 0.012, 5, 24);
  },
  { passive: true },
);
renderer.domElement.addEventListener("contextmenu", (e) => e.preventDefault());
function walkable(x, z) {
  return (
    x * x + z * z < 27 * 27 ||
    (Math.abs(x) < 2.25 && z > 24 && z < 37.3) ||
    (x > -27.3 && x < -20 && Math.abs(z - 12) < 1.65)
  );
}
function move(dt) {
  if (sailing) {
    const thrust = (keys.has("w") ? 1 : 0) - (keys.has("s") ? 1 : 0);
    speed = T.MathUtils.damp(speed, thrust * 8, 1.5, dt);
    const turn = ((keys.has("a") ? 1 : 0) - (keys.has("d") ? 1 : 0)) * dt * 0.8;
    boat.rotation.y += turn;
    yaw += turn;
    let next = boat.position
      .clone()
      .add(
        new T.Vector3(
          Math.sin(boat.rotation.y),
          0,
          Math.cos(boat.rotation.y),
        ).multiplyScalar(speed * dt),
      );
    if (Math.hypot(next.x, next.z) > 35 && Math.hypot(next.x, next.z) < 110)
      boat.position.copy(next);
    else speed = 0;
    player.position.copy(boat.localToWorld(new T.Vector3(0, 0.25, -2.3)));
    player.rotation.y = boat.rotation.y;
    return;
  }
  const forward = new T.Vector3(-Math.sin(yaw), 0, -Math.cos(yaw)),
    right = new T.Vector3(Math.cos(yaw), 0, -Math.sin(yaw)),
    v = new T.Vector3();
  if (keys.has("w")) v.add(forward);
  if (keys.has("s")) v.sub(forward);
  if (keys.has("d")) v.add(right);
  if (keys.has("a")) v.sub(right);
  const moving = v.lengthSq() > 0;
  if (moving && !catching) {
    v.normalize().multiplyScalar(dt * (keys.has("shift") ? 7 : 4.3));
    const next = player.position.clone().add(v);
    if (aboard) {
      const local = boat.worldToLocal(next.clone());
      local.x = T.MathUtils.clamp(local.x, -1.7, 1.7);
      local.z = T.MathUtils.clamp(local.z, -3.2, 3.4);
      player.position.copy(boat.localToWorld(local));
    } else {
      const blocked = world.colliders.some(
        (c) =>
          Math.hypot(next.x - c.obj.position.x, next.z - c.obj.position.z) <
          c.r * Math.max(c.obj.scale.x, c.obj.scale.z),
      );
      if (walkable(next.x, next.z) && !blocked) player.position.copy(next);
      else {
        const nx = player.position.x + v.x,
          nz = player.position.z + v.z;
        if (
          walkable(nx, player.position.z) &&
          !world.colliders.some(
            (c) =>
              Math.hypot(
                nx - c.obj.position.x,
                player.position.z - c.obj.position.z,
              ) < c.r,
          )
        )
          player.position.x = nx;
        else if (
          walkable(player.position.x, nz) &&
          !world.colliders.some(
            (c) =>
              Math.hypot(
                player.position.x - c.obj.position.x,
                nz - c.obj.position.z,
              ) < c.r,
          )
        )
          player.position.z = nz;
      }
    }
    player.rotation.y = Math.atan2(v.x, v.z);
  }
  player.userData.limbs.forEach(
    (l, i) =>
      (l.rotation.x = moving
        ? Math.sin(time * 11 + (i % 2) * Math.PI) * 0.45
        : 0),
  );
  if (jump > 0) {
    vy -= 14 * dt;
    jump = Math.max(0, jump + vy * dt);
  }
  player.position.y = (aboard ? boat.position.y + 0.18 : 0.2) + jump;
}
let lastFrame = performance.now();
function frame() {
  requestAnimationFrame(frame);
  const now = performance.now(),
    dt = Math.min((now - lastFrame) / 1000, 0.05);
  lastFrame = now;
  time += dt;
  boat.position.y = 0.1 + Math.sin(time * 1.3) * 0.13;
  boat.rotation.z = Math.sin(time * 0.9) * 0.018;
  wildWisps.forEach((w, i) => {
    w.position.y = w.userData.base.y + Math.sin(time * 1.4 + i) * 0.4;
    w.rotation.y = time * 0.2;
  });
  deckWisps.forEach((w, i) => {
    w.position.y = 0.85 + Math.sin(time * 3 + i) * 0.2;
    w.rotation.y = Math.sin(time + i) * 0.4;
  });
  world.fairy.position.y = Math.sin(time * 2) * 0.09;
  world.bell.rotation.z = state.quest === 2 ? Math.sin(time * 2) * 0.12 : 0;
  const f = boat.userData.flag.geometry.attributes.position;
  for (let i = 0; i < f.count; i++)
    f.setZ(i, Math.sin(time * 3 + f.getX(i) * 3) * 0.07);
  f.needsUpdate = true;
  if (!started) {
    const a = 0.5 + Math.sin(time * 0.035) * 0.12;
    camera.position.set(Math.sin(a) * 78, 46, Math.cos(a) * 78);
    camera.lookAt(0, 2, 0);
  } else if (editing) orbit.update();
  else {
    if (!paused) {
      move(dt);
      updateCatch();
      nearby = chooseInteraction();
      $("prompt").classList.toggle("hidden", !nearby || !!catching);
      if (nearby) $("prompt").innerHTML = "<kbd>E</kbd> " + nearby.text;
    }
    const target = player.position.clone().add(new T.Vector3(0, 1.3, 0));
    const desired = target
      .clone()
      .add(
        new T.Vector3(
          Math.sin(yaw) * Math.cos(pitch),
          Math.sin(pitch),
          Math.cos(yaw) * Math.cos(pitch),
        ).multiplyScalar(sailing ? zoom + 6 : zoom),
      );
    desired.y = Math.max(desired.y, 2.1);
    camera.position.lerp(desired, 1 - Math.exp(-dt * 7));
    camera.lookAt(target);
  }
  renderer.render(scene, camera);
}
window.addEventListener("resize", () => {
  camera.aspect = innerWidth / innerHeight;
  camera.updateProjectionMatrix();
  renderer.setSize(innerWidth, innerHeight);
});
applyFlag();
updateHUD();
frame();
// Optional artist override: export your Blender scene here with matching coordinates.
fetch("/models/harbor.glb", { method: "HEAD" })
  .then((r) => {
    if (r.ok && !r.headers.get("content-type")?.includes("text/html"))
      new GLTFLoader().load("/models/harbor.glb", (gltf) => {
        const imported = gltf.scene.getObjectByName("Wind_Bell_Harbor");
        if (!imported) return;
        imported.traverse((object) => {
          if (!object.isMesh) return;
          object.castShadow = true;
          object.receiveShadow = true;
          for (const material of Array.isArray(object.material)
            ? object.material
            : [object.material]) {
            material.flatShading = true;
            material.needsUpdate = true;
          }
        });
        // Keep gameplay object references, so interactions and editor handles
        // follow artist-authored landmark positions. Animated actors stay live.
        const actors = [world.keeper, world.fairy, world.tower];
        const normalize = (name) => name.replaceAll(" ", "_");
        for (const actor of actors) {
          imported.children
            .find((c) => normalize(c.name) === normalize(actor.name))
            ?.removeFromParent();
        }
        for (const object of world.editable) {
          if (actors.includes(object)) continue;
          const replacement = imported.children.find(
            (c) => normalize(c.name) === normalize(object.name),
          );
          if (!replacement) continue;
          object.clear();
          object.position.copy(replacement.position);
          object.quaternion.copy(replacement.quaternion);
          object.scale.copy(replacement.scale);
          while (replacement.children.length)
            object.add(replacement.children[0]);
          replacement.removeFromParent();
        }
        for (const child of [...world.root.children]) {
          if (!world.editable.includes(child)) child.removeFromParent();
        }
        while (imported.children.length) world.root.add(imported.children[0]);
        try {
          const saved = JSON.parse(localStorage.getItem("cloudwake-layout-v1"));
          if (saved) applyLayout(saved);
        } catch {
          /* An invalid layout must not prevent the artist model loading. */
        }
        world.root.updateMatrixWorld(true);
        world.root.userData.artistModelLoaded = true;
      });
  })
  .catch(() => {});
// Read-only inspection plus editor/export APIs for QA and authoring.
window.cloudwake = {
  getSnapshot: () => ({
    state: { ...state },
    position: player.position.toArray(),
    boat: boat.position.toArray(),
    aboard,
    sailing,
    started,
    paused,
    editing,
    artistModelLoaded: world.root.userData.artistModelLoaded === true,
    catching: catching ? { ...catching } : null,
    interaction: nearby?.text,
    drawCalls: renderer.info.render.calls,
  }),
  exportGLB,
  getLayout: layout,
};
