import * as T from "three";
import { mergeGeometries } from "three/addons/utils/BufferGeometryUtils.js";

export const COLORS = {
  grass: 0x93a958,
  wood: 0x785036,
  dark: 0x473f31,
  cream: 0xe6d8ac,
  teal: 0x287d82,
  gold: 0xdfb765,
  stone: 0xb3ad90,
};
let seed = 42;
const rand = () => {
  seed = (seed * 1664525 + 1013904223) >>> 0;
  return seed / 4294967296;
};
const mats = new Map();
export function mat(color) {
  if (!mats.has(color))
    mats.set(
      color,
      new T.MeshStandardMaterial({ color, roughness: 1, flatShading: true }),
    );
  return mats.get(color);
}
export function mesh(g, geo, color, x = 0, y = 0, z = 0) {
  const m = new T.Mesh(geo, mat(color));
  m.position.set(x, y, z);
  m.castShadow = true;
  m.receiveShadow = true;
  g.add(m);
  return m;
}
export const box = (g, x, y, z, w, h, d, c) =>
  mesh(g, new T.BoxGeometry(w, h, d), c, x, y, z);
export const cyl = (g, x, y, z, rt, rb, h, c, n = 8) =>
  mesh(g, new T.CylinderGeometry(rt, rb, h, n), c, x, y, z);
export const ball = (g, x, y, z, r, c, detail = 0) =>
  mesh(g, new T.IcosahedronGeometry(r, detail), c, x, y, z);
function beam(g, a, b, r, c) {
  const va = new T.Vector3(...a),
    vb = new T.Vector3(...b),
    m = cyl(
      g,
      ...va.clone().add(vb).multiplyScalar(0.5).toArray(),
      r,
      r,
      va.distanceTo(vb),
      c,
      6,
    );
  m.quaternion.setFromUnitVectors(
    new T.Vector3(0, 1, 0),
    vb.sub(va).normalize(),
  );
  return m;
}
function group(parent, name, x = 0, y = 0, z = 0) {
  const g = new T.Group();
  g.name = name;
  g.position.set(x, y, z);
  parent.add(g);
  return g;
}
function sign(g, text, x, y, z, w = 4, h = 0.75) {
  const c = document.createElement("canvas");
  c.width = 768;
  c.height = 128;
  const ctx = c.getContext("2d");
  ctx.fillStyle = "#544837";
  ctx.fillRect(0, 0, 768, 128);
  ctx.strokeStyle = "#bdaa76";
  ctx.lineWidth = 6;
  ctx.strokeRect(8, 8, 752, 112);
  ctx.fillStyle = "#f5e3af";
  ctx.font = "bold 48px Georgia";
  ctx.textAlign = "center";
  ctx.textBaseline = "middle";
  ctx.fillText(text, 384, 68, 720);
  const texture = new T.CanvasTexture(c);
  texture.colorSpace = T.SRGBColorSpace;
  const m = new T.Mesh(
    new T.PlaneGeometry(w, h),
    new T.MeshStandardMaterial({ map: texture, roughness: 1 }),
  );
  m.position.set(x, y, z);
  g.add(m);
  return m;
}
function lantern(g, x, y, z) {
  cyl(g, x, y, z, 0.19, 0.24, 0.55, 0xefc26b, 6);
  cyl(g, x, y + 0.32, z, 0.31, 0.05, 0.15, COLORS.dark, 6);
  cyl(g, x, y - 0.3, z, 0.28, 0.28, 0.08, COLORS.dark, 6);
}
export function makeCharacter(parent, name, color = COLORS.teal) {
  const g = group(parent, name);
  box(g, 0, 1.05, 0, 0.67, 0.78, 0.42, color);
  ball(g, 0, 1.75, 0, 0.36, 0xe5b985, 1);
  const hair = ball(g, 0, 1.96, -0.07, 0.37, 0x533d2c, 1);
  hair.scale.set(1, 0.65, 1);
  box(g, 0, 1.4, 0.02, 0.76, 0.13, 0.51, 0xe0ad5b);
  box(g, 0.1, 1.17, -0.3, 0.24, 0.55, 0.08, 0xe0ad5b);
  const legs = [
    box(g, -0.19, 0.37, 0, 0.24, 0.7, 0.26, 0x43483f),
    box(g, 0.19, 0.37, 0, 0.24, 0.7, 0.26, 0x43483f),
  ];
  box(g, -0.19, 0.09, 0.12, 0.27, 0.19, 0.43, COLORS.dark);
  box(g, 0.19, 0.09, 0.12, 0.27, 0.19, 0.43, COLORS.dark);
  const arms = [
    box(g, -0.45, 1, 0, 0.21, 0.65, 0.24, color),
    box(g, 0.45, 1, 0, 0.21, 0.65, 0.24, color),
  ];
  ball(g, -0.45, 0.66, 0, 0.14, 0xe5b985);
  ball(g, 0.45, 0.66, 0, 0.14, 0xe5b985);
  box(g, -0.12, 1.78, 0.32, 0.045, 0.045, 0.035, 0x35382e);
  box(g, 0.12, 1.78, 0.32, 0.045, 0.045, 0.035, 0x35382e);
  g.userData.limbs = [...legs, ...arms];
  return g;
}
function shop(root, name, x, z, color) {
  const g = group(root, name, x, 0, z);
  box(g, 0, 1.8, 0, 6, 3.6, 4.7, COLORS.cream);
  for (const px of [-2.95, 0, 2.95])
    box(g, px, 1.8, 2.4, 0.19, 3.8, 0.18, COLORS.wood);
  for (const py of [0.2, 3.5]) box(g, 0, py, 2.42, 6.2, 0.2, 0.2, COLORS.wood);
  box(g, 0, 1.1, 2.48, 1.15, 2.2, 0.1, 0x574b35);
  for (const px of [-1.9, 1.9]) {
    box(g, px, 1.8, 2.45, 1.1, 1.2, 0.1, 0x486966);
    box(g, px, 1.8, 2.52, 1.2, 0.09, 0.1, COLORS.wood);
    box(g, px, 1.8, 2.52, 0.09, 1.2, 0.1, COLORS.wood);
  }
  for (const side of [-1, 1]) {
    const roof = box(g, side * 1.68, 4.3, 0, 3.9, 0.2, 5.5, color);
    roof.rotation.z = side * -0.46;
    for (let i = 0; i < 8; i++) {
      const tile = box(
        g,
        side * 1.7,
        4.43,
        -2.5 + i * 0.71,
        3.85,
        0.11,
        0.62,
        color,
      );
      tile.rotation.z = side * -0.46;
    }
  }
  box(g, 0, 5.15, 0, 0.2, 0.22, 5.7, COLORS.wood);
  box(g, 1.7, 4.8, -1, 0.8, 2, 0.8, COLORS.stone);
  for (const px of [-2.8, 2.8]) {
    box(g, px, 1.5, 4.3, 0.15, 3, 0.15, COLORS.wood);
    lantern(g, px, 2.3, 4.3);
  }
  for (let i = 0; i < 6; i++) {
    let aw = box(
      g,
      -2.5 + i,
      2.98,
      3.43,
      1,
      0.12,
      2.7,
      i % 2 ? COLORS.cream : color,
    );
    aw.rotation.x = 0.16;
    box(g, -2.5 + i, 2.69, 4.75, 1, 0.32, 0.1, i % 2 ? COLORS.cream : color);
  }
  box(g, 0, 0.8, 3.8, 4.8, 1.25, 0.85, COLORS.wood);
  for (let i = 0; i < 9; i++) {
    cyl(
      g,
      -2 + i * 0.5,
      1.58,
      3.8,
      0.14,
      0.18,
      0.35,
      [0x87b5a0, 0xd8b661, 0xc18462][i % 3],
      6,
    );
  }
  sign(g, name.toUpperCase(), 0, 3.36, 2.56, 4.8, 0.65);
  return g;
}
export function makeWorld(scene) {
  const root = group(scene, "Wind_Bell_Harbor");
  const editable = [];
  const colliders = [];
  const add = (obj, r) => {
    editable.push(obj);
    if (r) colliders.push({ obj, r });
    return obj;
  };
  const terrain = group(root, "Island_Terrain");
  cyl(terrain, 0, -1.5, 0, 28, 25, 3, 0x889563, 14);
  cyl(terrain, 0, -5, 0, 25, 17, 5, 0x8a8d77, 14);
  cyl(terrain, 0, -10, 0, 17, 5, 6, 0x747e72, 11);
  cyl(terrain, 0, -14, 0, 5, 0, 3, 0x68776f, 7);
  cyl(terrain, 0, 0.01, 0, 27.9, 27.9, 0.18, COLORS.grass, 48);
  for (let i = 0; i < 32; i++) {
    let a = rand() * Math.PI * 2,
      r = 26 + rand() * 1.8;
    const rock = ball(
      terrain,
      Math.cos(a) * r,
      -1 - rand() * 5,
      Math.sin(a) * r,
      1.6 + rand() * 2.8,
      [0x9c9e7c, 0x929680, 0x7e8a76][i % 3],
    );
    rock.scale.y = 1.5;
  }
  // Broad central square and branching paths.
  cyl(root, 0, 0.13, 4, 8, 8, 0.1, 0xd4c591, 20);
  box(root, 0, 0.12, 17, 4, 0.12, 19, 0xd4c591);
  box(root, 0, 0.13, -7, 3.7, 0.14, 15, 0xd4c591);
  for (const x of [-7, 7]) box(root, x, 0.13, 3, 13, 0.12, 3, 0xd4c591);
  for (let i = 0; i < 65; i++) {
    const x = (rand() - 0.5) * 8,
      z = -15 + rand() * 41;
    if (Math.abs(x) < 1.8 || z < 10) {
      const p = box(
        root,
        x,
        0.23,
        z,
        0.4 + rand() * 0.8,
        0.06,
        0.35 + rand() * 0.6,
        0xe0d4a7,
      );
      p.rotation.y = rand() * 3;
    }
  }
  add(shop(root, "Cloud Charms", -12, 1, 0x327b7a), 4.2);
  add(shop(root, "Windward Trading", 12, 1, 0xb7734c), 4.2);
  add(shop(root, "Mapmaker", 11, -10, 0x367d84), 4.2);
  const tower = add(group(root, "Wind_Bell_Tower", 0, 0, -17), 3.1);
  cyl(tower, 0, 0.3, 0, 4.4, 4.6, 0.6, 0xbab495, 8);
  for (let i = 0; i < 5; i++) {
    for (const x of [-2, 2]) {
      box(
        tower,
        x,
        0.8 + i * 1.3,
        0,
        1.2,
        1.2,
        1.6,
        i % 2 ? 0xc9c2a4 : 0xbcb79b,
      );
      box(
        tower,
        x,
        0.8 + i * 1.3,
        -1.5,
        1.2,
        1.2,
        1.6,
        i % 2 ? 0xbcb79b : 0xc9c2a4,
      );
    }
  }
  box(tower, 0, 7, -0.6, 5.6, 0.7, 2.8, 0xd0c6a1);
  box(tower, 0, 7.9, -0.6, 3.7, 0.9, 2.4, 0xc3b894);
  box(tower, 0, 8.7, -0.6, 2, 0.65, 1.9, 0xd0c6a1);
  beam(tower, [-2, 6.4, 0], [2, 6.4, 0], 0.16, COLORS.wood);
  const bell = group(tower, "Sleeping_Bell", 0, 5.4, 0);
  cyl(bell, 0, 0, 0, 0.45, 0.8, 1.3, COLORS.gold, 12);
  cyl(bell, 0, -0.68, 0, 0.95, 0.95, 0.14, COLORS.gold, 12);
  ball(bell, 0, -0.85, 0, 0.15, COLORS.dark);
  beam(tower, [1.4, 5.2, 0.3], [1.4, 1.3, 0.3], 0.025, COLORS.wood);
  sign(tower, "THE WIND BELL", 0, 1.2, 1.02, 2.6, 0.5);
  const keeper = add(makeCharacter(root, "Bellkeeper", 0x9b7950));
  keeper.position.set(-3.6, 0, -12);
  cyl(keeper, 0, 2.08, 0, 0.46, 0.46, 0.14, 0x917951);
  cyl(keeper, 0, 2.25, 0, 0.22, 0.3, 0.3, 0x917951);
  const fairy = add(makeCharacter(root, "Mallow the trader", 0x7d9870));
  fairy.position.set(-10, 0, 6.4);
  for (const side of [-1, 1]) {
    const wing = ball(fairy, side * 0.55, 1.35, -0.35, 0.5, 0xbddfbe, 1);
    wing.scale.set(0.6, 1, 0.13);
    wing.rotation.z = side * 0.6;
  }
  const tree = add(group(root, "Lantern_Tree", -13, 0, -13), 2);
  cyl(tree, 0, 3.5, 0, 0.8, 1.6, 7, 0x796045, 7);
  beam(tree, [0, 3, 0], [-2, 7, 0], 0.45, 0x796045);
  beam(tree, [0, 4, 0], [3, 7, -1], 0.5, 0x796045);
  for (let i = 0; i < 11; i++) {
    const a = i * 2.4,
      r = rand() * 3;
    const b = ball(
      tree,
      Math.cos(a) * r,
      7 + rand() * 2,
      Math.sin(a) * r,
      2.6 + rand(),
      [0x799747, 0x8ba94f, 0xa5b65c, 0xb6bb65][i % 4],
      1,
    );
    b.scale.y = 0.7;
  }
  for (let i = 0; i < 5; i++) {
    let x = -3 + i * 1.5;
    beam(tree, [x, 7, 1], [x, 4.7, 1], 0.015, COLORS.dark);
    lantern(tree, x, 4.5, 1);
  }
  // Dock planks and rope fence.
  const dock = group(root, "Airship_Dock", 0, 0, 31);
  for (let i = 0; i < 25; i++)
    box(
      dock,
      0,
      0.17,
      -6 + i * 0.5,
      5,
      0.26,
      0.46,
      i % 3 ? 0x9e784b : 0xb18c58,
    );
  for (const x of [-2.6, 2.6])
    for (let z = -5; z <= 6; z += 3) {
      box(dock, x, 0.2, z, 0.23, 2.7, 0.23, COLORS.wood);
      if (z < 6) beam(dock, [x, 1.15, z], [x, 1.15, z + 3], 0.04, 0xd3bd85);
    }
  for (const x of [-2.6, 2.6]) lantern(dock, x, 1.8, 5.8);
  const pier = group(root, "Cloud_Catching_Pier", -21, 0, 12);
  for (let i = 0; i < 12; i++)
    box(pier, -i * 0.47, 0.13, 0, 0.44, 0.24, 3.8, 0x9b7a50);
  sign(pier, "CLOUDS ARE LISTENING", -0.2, 1.3, -1.6, 2.5, 0.5);
  beam(pier, [-1, 0, -1.6], [-1, 1.8, -1.6], 0.08, COLORS.wood);
  // Flowers and grass: limited, deterministic and individually editable in Blender.
  const details = group(root, "Meadow_Details");
  for (let i = 0; i < 220; i++) {
    const x = (rand() - 0.5) * 54,
      z = (rand() - 0.5) * 54;
    if (
      x * x + z * z > 700 ||
      Math.abs(x) < 3 ||
      Math.abs(z - 3) < 2 ||
      colliders.some(
        (c) => Math.hypot(c.obj.position.x - x, c.obj.position.z - z) < c.r + 1,
      )
    )
      continue;
    const grass = cyl(details, x, 0.3, z, 0, 0.15, 0.5, 0x7d984b, 3);
    grass.rotation.z = (rand() - 0.5) * 0.4;
    if (i % 3 === 0) {
      const flower = ball(
        details,
        x,
        0.5,
        z,
        0.13,
        i % 2 ? 0xf0dfac : 0xeac586,
      );
      flower.scale.y = 0.4;
    }
  }
  for (let i = 0; i < 15; i++) {
    const a = (i / 15) * Math.PI * 2;
    if (Math.sin(a) > 0.86) continue;
    let x = Math.cos(a) * 26,
      z = Math.sin(a) * 26;
    const fence = group(root, "Fence_" + i, x, 0, z);
    fence.rotation.y = -a;
    for (const zz of [-1.5, 1.5])
      box(fence, 0, 0.55, zz, 0.17, 1.3, 0.17, COLORS.wood);
    box(fence, 0, 0.85, 0, 0.13, 0.15, 3.2, 0xa78c59);
  }
  for (let i = 0; i < 9; i++) {
    const crate = add(
      group(root, "Crate_" + i, i < 5 ? -17 : 17, 0, (i % 5) * 2 - 1),
    );
    box(crate, 0, 0.4, 0, 0.85, 0.8, 0.85, 0xb08a57);
    for (const yy of [0.08, 0.7])
      box(crate, 0, yy, 0.44, 0.92, 0.1, 0.07, COLORS.wood);
  }
  const well = add(group(root, "Wishing_Well", 5, 0, -5), 1.4);
  cyl(well, 0, 0.4, 0, 1.45, 1.6, 0.8, 0xbab696, 10);
  cyl(well, 0, 0.83, 0, 1.2, 1.2, 0.08, 0x537c74, 10);
  for (let i = 0; i < 10; i++) {
    const a = (i * Math.PI) / 5;
    const stone = box(
      well,
      Math.cos(a) * 1.4,
      1,
      Math.sin(a) * 1.4,
      0.7,
      0.42,
      0.4,
      0xc6bba0,
    );
    stone.rotation.y = -a;
  }
  for (const x of [-1.65, 1.65])
    box(well, x, 1.7, 0, 0.16, 3.4, 0.16, COLORS.wood);
  beam(well, [-1.75, 3.25, 0], [1.75, 3.25, 0], 0.1, COLORS.wood);
  lantern(well, 0, 2.8, 0);
  const garden = group(root, "Harbor_Gardens");
  for (const [x, z] of [
    [-7, 17],
    [8, 17],
    [-19, -6],
    [19, -6],
    [-5, -21],
    [6, -21],
    [20, 10],
  ]) {
    for (let i = 0; i < 5; i++) {
      const b = ball(
        garden,
        x + Math.cos(i * 2) * 0.7,
        0.5,
        z + Math.sin(i * 2) * 0.6,
        0.65,
        [0x82984b, 0x8da653, 0xa8b568][i % 3],
        1,
      );
      b.scale.y = 0.8;
    }
    for (let i = 0; i < 6; i++)
      ball(
        garden,
        x + (rand() - 0.5) * 2,
        0.85,
        z + (rand() - 0.5) * 1.5,
        0.12,
        i % 2 ? 0xf2da9d : 0xd8a483,
      );
  }
  for (const x of [-7, 7]) {
    const bench = group(root, "Bench_" + x, x, 0, 11);
    for (const px of [-0.8, 0.8])
      box(bench, px, 0.3, 0, 0.13, 0.6, 0.7, COLORS.wood);
    box(bench, 0, 0.63, 0, 2.2, 0.12, 0.8, 0xae8a54);
    box(bench, 0, 1.1, -0.35, 2.2, 0.45, 0.1, 0xae8a54);
  }
  const pennants = group(root, "Harbor_Bunting");
  for (const x of [-7, 7])
    cyl(pennants, x, 2.9, 14, 0.06, 0.09, 5.8, COLORS.wood, 6);
  const points = [];
  for (let i = 0; i <= 20; i++) {
    const x = -7 + i * 0.7,
      y = 5.8 - Math.sin((i / 20) * Math.PI) * 1.2;
    points.push(new T.Vector3(x, y, 14));
    if (i % 2) {
      const flag = mesh(
        pennants,
        new T.ConeGeometry(0.24, 0.6, 3),
        i % 4 === 1 ? COLORS.teal : COLORS.gold,
        x,
        y - 0.28,
        14,
      );
      flag.rotation.z = Math.PI;
      flag.rotation.y = Math.PI;
    }
  }
  const rope = new T.Line(
    new T.BufferGeometry().setFromPoints(points),
    new T.LineBasicMaterial({ color: 0x8b7550 }),
  );
  pennants.add(rope);
  tree.scale.setScalar(1.2);
  const boat = makeBoat(scene);
  // Batch static geometry by material; named movable landmarks remain separate.
  for (const node of root.children) {
    if (node === keeper || node === fairy || node === tower) continue;
    if (node.isGroup) batchStatic(node, true);
  }
  batchStatic(root, false);
  return {
    root,
    terrain,
    editable,
    colliders,
    tower,
    bell,
    keeper,
    fairy,
    boat,
    pier,
  };
}
export function makeBoat(scene) {
  const g = group(scene, "Cloudwake_Skiff", 0, 0.1, 42);
  const hull = new T.Shape();
  hull.moveTo(-2.1, -3.7);
  hull.lineTo(2.1, -3.7);
  hull.lineTo(2.5, 1.7);
  hull.lineTo(1.5, 4);
  hull.lineTo(0, 5);
  hull.lineTo(-1.5, 4);
  hull.lineTo(-2.5, 1.7);
  hull.closePath();
  const geo = new T.ExtrudeGeometry(hull, {
    depth: 1.15,
    bevelEnabled: true,
    bevelThickness: 0.3,
    bevelSize: 0.3,
    bevelSegments: 1,
    steps: 1,
  });
  geo.rotateX(Math.PI / 2);
  mesh(g, geo, 0x785435, 0, -0.1, 0);
  for (let i = 0; i < 15; i++) {
    const z = -3.5 + i * 0.52;
    const w = z > 1.7 ? 4.5 - (z - 1.7) * 1.05 : 4.2;
    box(g, 0, 0.05, z, w, 0.18, 0.46, i % 3 ? 0xad8654 : 0xbb925d);
  }
  for (const x of [-2.2, 2.2]) {
    box(g, x, 0.58, -0.7, 0.15, 0.2, 6.4, COLORS.teal);
    for (const z of [-3.5, -1.5, 0.5, 2.2])
      box(g, x, 0.33, z, 0.18, 0.85, 0.18, COLORS.wood);
  }
  beam(g, [-2.2, 0.58, 2.2], [0, 0.58, 4.7], 0.09, COLORS.teal);
  beam(g, [2.2, 0.58, 2.2], [0, 0.58, 4.7], 0.09, COLORS.teal);
  box(g, 0, 0.5, -3.7, 4.5, 0.18, 0.16, COLORS.teal);
  cyl(g, 0, 3.7, 0.7, 0.11, 0.18, 7.4, COLORS.wood);
  beam(g, [-2.8, 6.5, 0.7], [2.8, 6.5, 0.7], 0.1, COLORS.wood);
  beam(g, [-2.5, 2.5, 0.7], [2.5, 2.5, 0.7], 0.09, COLORS.wood);
  const sailGeo = new T.PlaneGeometry(5.2, 4, 6, 5),
    pos = sailGeo.attributes.position;
  for (let i = 0; i < pos.count; i++) {
    let x = pos.getX(i),
      y = pos.getY(i);
    pos.setZ(
      i,
      Math.sin((x / 5.2 + 0.5) * Math.PI) *
        Math.sin((y / 4 + 0.5) * Math.PI) *
        0.8,
    );
  }
  sailGeo.computeVertexNormals();
  const sail = new T.Mesh(
    sailGeo,
    new T.MeshStandardMaterial({
      color: 0xf0dfb4,
      side: T.DoubleSide,
      roughness: 1,
      flatShading: true,
    }),
  );
  sail.position.set(0, 4.5, 0.8);
  sail.castShadow = true;
  g.add(sail);
  beam(g, [-2.8, 6.5, 0.7], [-2.2, 0.5, -3.5], 0.018, 0xcab68b);
  beam(g, [2.8, 6.5, 0.7], [2.2, 0.5, -3.5], 0.018, 0xcab68b);
  const wheel = group(g, "Helm", 0, 1, -2.5);
  cyl(wheel, 0, -0.45, 0, 0.11, 0.16, 1.2, COLORS.wood);
  const torus = mesh(
    wheel,
    new T.TorusGeometry(0.45, 0.065, 5, 12),
    COLORS.wood,
    0,
    0.2,
    0.12,
  );
  for (let i = 0; i < 8; i++) {
    let a = (i * Math.PI) / 4;
    beam(
      wheel,
      [0, 0.2, 0.12],
      [Math.cos(a) * 0.65, 0.2 + Math.sin(a) * 0.65, 0.12],
      0.038,
      COLORS.wood,
    );
  }
  ball(wheel, 0, 0.2, 0.12, 0.13, COLORS.gold);
  lantern(g, 0, 1.2, 4.7);
  box(g, -1.2, 0.4, -2, 0.65, 0.7, 0.7, 0x8f6c40);
  const flag = mesh(
    g,
    new T.PlaneGeometry(1.7, 0.95, 8, 2),
    COLORS.teal,
    0.84,
    7,
    0.75,
  );
  flag.material = new T.MeshStandardMaterial({
    color: COLORS.teal,
    side: T.DoubleSide,
    roughness: 1,
  });
  g.userData.flag = flag;
  g.userData.wheel = wheel;
  return g;
}
export function makeWisp(parent, color = 0xe9f4cf) {
  const g = group(parent, "Cloud_Wisp");
  ball(g, 0, 0, 0, 0.43, color, 1);
  for (let i = 0; i < 4; i++) {
    const a = i * 1.8;
    ball(g, Math.cos(a) * 0.34, Math.sin(a) * 0.17, 0, 0.23, color);
  }
  ball(g, -0.13, 0.06, 0.38, 0.045, 0x264a4d);
  ball(g, 0.13, 0.06, 0.38, 0.045, 0x264a4d);
  const mouth = mesh(
    g,
    new T.TorusGeometry(0.08, 0.018, 4, 8, Math.PI),
    0x264a4d,
    0,
    -0.06,
    0.41,
  );
  mouth.rotation.z = Math.PI;
  return g;
}
export function makeClouds(scene) {
  const g = group(scene, "Atmosphere");
  for (let i = 0; i < 75; i++) {
    const a = rand() * Math.PI * 2,
      r = 38 + rand() * 190;
    const cloud = group(
      g,
      "Cloud_" + i,
      Math.cos(a) * r,
      -12 - rand() * 12,
      Math.sin(a) * r,
    );
    for (let j = 0; j < 4; j++) {
      const b = ball(
        cloud,
        (j - 1.5) * 4,
        rand() * 2,
        rand() * 3,
        4 + rand() * 4,
        0xe5eee1,
        1,
      );
      b.scale.set(1.8, 0.65, 1.2);
      b.castShadow = false;
    }
  }
  return g;
}
export function batchStatic(node, recursive = true) {
  node.updateWorldMatrix(true, true);
  const inverse = node.matrixWorld.clone().invert(),
    groups = new Map();
  const collect = (o) => {
    if (!o.isMesh || Array.isArray(o.material)) return;
    const key = o.material.uuid;
    let item = groups.get(key);
    if (!item) {
      item = {
        material: o.material,
        meshes: [],
        geos: [],
        shadow: o.castShadow,
      };
      groups.set(key, item);
    }
    let geo = o.geometry.clone();
    if (geo.index) geo = geo.toNonIndexed();
    geo.applyMatrix4(new T.Matrix4().multiplyMatrices(inverse, o.matrixWorld));
    item.geos.push(geo);
    item.meshes.push(o);
  };
  if (recursive) node.traverse(collect);
  else [...node.children].forEach(collect);
  for (const entry of groups.values()) {
    if (entry.meshes.length < 2) {
      entry.geos.forEach((g) => g.dispose());
      continue;
    }
    const combined = mergeGeometries(entry.geos, false);
    if (!combined) continue;
    const merged = new T.Mesh(combined, entry.material);
    merged.name = node.name + "_material";
    merged.castShadow = entry.shadow;
    merged.receiveShadow = true;
    entry.meshes.forEach((o) => o.removeFromParent());
    node.add(merged);
    entry.geos.forEach((g) => g.dispose());
  }
}
