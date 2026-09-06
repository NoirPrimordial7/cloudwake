import * as T from "three";
import { GLTFLoader } from "three/addons/loaders/GLTFLoader.js";
import { Sky } from "three/addons/objects/Sky.js";
import { mergeGeometries } from "three/addons/utils/BufferGeometryUtils.js";
import { height, POOL, SPECIES } from "../shared/game.js";
const cache = new Map();
export function material(c, metal = 0) {
  const key = c + metal;
  if (!cache.has(key))
    cache.set(
      key,
      new T.MeshStandardMaterial({
        color: c,
        roughness: 0.82,
        metalness: metal,
      }),
    );
  return cache.get(key);
}
export function mesh(parent, geo, c, x = 0, y = 0, z = 0) {
  const m = new T.Mesh(geo, typeof c === "object" ? c : material(c));
  m.position.set(x, y, z);
  m.castShadow = true;
  m.receiveShadow = true;
  parent.add(m);
  return m;
}
export const box = (p, x, y, z, w, h, d, c) =>
  mesh(p, new T.BoxGeometry(w, h, d), c, x, y, z);
export function ellipsoid(p, x, y, z, w, h, d, c) {
  const m = mesh(p, new T.SphereGeometry(1, 16, 10), c, x, y, z);
  m.scale.set(w, h, d);
  return m;
}
export function beam(p, a, b, r, c) {
  const av = new T.Vector3(...a),
    bv = new T.Vector3(...b);
  const m = mesh(
    p,
    new T.CylinderGeometry(r * 0.8, r, av.distanceTo(bv), 9),
    c,
  );
  m.position.copy(av.clone().add(bv).multiplyScalar(0.5));
  m.quaternion.setFromUnitVectors(
    new T.Vector3(0, 1, 0),
    bv.sub(av).normalize(),
  );
  return m;
}
export function avatar(color = "#377f7d") {
  const g = new T.Group();
  g.name = "Sailor";
  ellipsoid(g, 0, 0.95, 0, 0.29, 0.43, 0.19, color);
  ellipsoid(g, 0, 1.58, 0.02, 0.23, 0.27, 0.23, "#d4a77d");
  ellipsoid(g, 0, 1.78, 0, 0.25, 0.13, 0.24, "#4b3223");
  box(g, 0, 1.22, 0.015, 0.58, 0.075, 0.39, "#dab267");
  const limbs = [];
  for (const side of [-1, 1]) {
    const leg = beam(
      g,
      [side * 0.14, 0.64, 0],
      [side * 0.14, 0.13, 0],
      0.085,
      "#37433b",
    );
    limbs.push(leg);
    ellipsoid(g, side * 0.14, 0.1, 0.08, 0.1, 0.09, 0.17, "#50382a");
    const arm = beam(
      g,
      [side * 0.3, 1.18, 0],
      [side * 0.39, 0.73, 0.02],
      0.083,
      color,
    );
    limbs.push(arm);
    ellipsoid(g, side * 0.39, 0.7, 0.02, 0.08, 0.105, 0.08, "#a2744e");
  }
  g.userData.limbs = limbs;
  return g;
}
export function makeFish(species) {
  const d = SPECIES[species],
    g = new T.Group();
  g.name = d.name;
  if (species === 2) {
    ellipsoid(g, 0, 0.12, 0, 0.45, 0.22, 0.38, d.color);
    for (const side of [-1, 1])
      for (let i = 0; i < 3; i++)
        beam(
          g,
          [side * 0.25, 0.1, (i - 1) * 0.19],
          [side * 0.65, -0.1, (i - 1) * 0.34],
          0.045,
          "#6c7552",
        );
    for (const side of [-1, 1]) {
      ellipsoid(g, side * 0.38, 0.08, 0.5, 0.14, 0.09, 0.18, "#bd9c58");
      ellipsoid(g, side * 0.15, 0.34, 0.24, 0.06, 0.08, 0.06, "#182c28");
    }
  } else {
    const length = species === 3 ? 1.35 : 0.66,
      s = species === 4 ? 2.4 : 1;
    ellipsoid(g, 0, 0, 0, 0.24 * s, 0.36 * s, length * s, d.color);
    ellipsoid(
      g,
      0,
      -0.1 * s,
      0.08 * s,
      0.205 * s,
      0.2 * s,
      0.51 * s,
      "#dbd5b1",
    );
    const tail = mesh(
      g,
      new T.ConeGeometry(0.34 * s, 0.46 * s, 3),
      species === 0 ? "#d1b464" : d.color,
      0,
      0,
      -length * s - 0.17 * s,
    );
    tail.rotation.x = -Math.PI / 2;
    tail.scale.y = 0.7;
    g.userData.tail = tail;
    for (const side of [-1, 1]) {
      const fin = mesh(
        g,
        new T.ConeGeometry(0.18 * s, 0.4 * s, 3),
        "#bf9652",
        side * 0.27 * s,
        -0.03 * s,
        -0.02,
      );
      fin.rotation.z = side * 1.2;
      fin.scale.z = 0.15;
      ellipsoid(
        g,
        side * 0.18 * s,
        0.13 * s,
        length * 0.7 * s,
        0.075 * s,
        0.075 * s,
        0.06 * s,
        "#ede2b8",
      );
      ellipsoid(
        g,
        side * 0.205 * s,
        0.13 * s,
        length * 0.75 * s,
        0.04 * s,
        0.045 * s,
        0.03 * s,
        "#172f30",
      );
    }
    const dorsal = mesh(
      g,
      new T.ConeGeometry(0.23 * s, 0.45 * s, 3),
      "#b99f5b",
      0,
      0.32 * s,
      -0.15,
    );
    dorsal.scale.x = 0.1;
    if (species === 4)
      mesh(
        g,
        new T.OctahedronGeometry(0.25),
        new T.MeshStandardMaterial({
          color: "#79ead5",
          emissive: "#29b89e",
          emissiveIntensity: 2,
        }),
        0,
        0.45,
        0.4,
      );
  }
  return g;
}
export function makeHands(camera) {
  const root = new T.Group();
  camera.add(root);
  const right = new T.Group();
  root.add(right);
  right.position.set(0.32, -0.35, -0.55);
  right.rotation.set(-0.2, 0.05, 0.12);
  beam(right, [0.07, -0.28, 0.23], [0, 0.03, 0], 0.09, "#476b64");
  beam(right, [0, 0.02, 0], [0, 0.17, -0.12], 0.075, "#8b5a35");
  ellipsoid(right, 0, 0.16, -0.12, 0.085, 0.11, 0.07, "#775034");
  for (let i = 0; i < 4; i++)
    beam(
      right,
      [-0.055 + i * 0.035, 0.19, -0.15],
      [-0.055 + i * 0.035, 0.1, -0.19],
      0.017,
      "#ac7a4c",
    );
  const rod = new T.Group();
  right.add(rod);
  beam(rod, [0, 0.1, -0.15], [-0.15, 1.23, -1.4], 0.025, "#79522d");
  beam(rod, [-0.15, 1.23, -1.4], [-0.22, 1.5, -2.15], 0.009, "#956d3b");
  for (let i = 0; i < 4; i++) {
    const ring = mesh(
      rod,
      new T.TorusGeometry(0.023, 0.007, 5, 10),
      "#c8ae72",
      -0.04 * i,
      0.29 + i * 0.31,
      -0.35 - i * 0.44,
    );
    ring.rotation.x = 0.5;
  }
  const reel = mesh(
    rod,
    new T.CylinderGeometry(0.065, 0.065, 0.04, 16),
    "#bfa16a",
    -0.07,
    0.27,
    -0.26,
  );
  reel.rotation.z = Math.PI / 2;
  const knife = new T.Group();
  right.add(knife);
  beam(knife, [0, 0.12, -0.13], [0, 0.31, -0.17], 0.037, "#694329");
  const blade = mesh(
    knife,
    new T.ConeGeometry(0.048, 0.4, 3),
    material("#adb9b1", 0.65),
    0,
    0.5,
    -0.17,
  );
  blade.scale.z = 0.2;
  box(knife, 0, 0.31, -0.17, 0.14, 0.025, 0.065, "#b9944f");
  knife.visible = false;
  return { root, right, rod, knife };
}
export async function environment(scene, progress) {
  const sky = new Sky();
  sky.scale.setScalar(800);
  const u = sky.material.uniforms;
  u.turbidity.value = 3;
  u.rayleigh.value = 1.2;
  u.mieCoefficient.value = 0.004;
  u.mieDirectionalG.value = 0.85;
  u.sunPosition.value.set(-0.45, 0.7, 0.3);
  scene.add(sky);
  const sun = new T.DirectionalLight("#ffdfa5", 3);
  sun.position.set(-35, 65, 35);
  sun.castShadow = true;
  sun.shadow.mapSize.set(2048, 2048);
  Object.assign(sun.shadow.camera, {
    left: -43,
    right: 43,
    top: 43,
    bottom: -43,
    near: 1,
    far: 130,
  });
  sun.shadow.normalBias = 0.06;
  sun.shadow.bias = -0.00025;
  scene.add(sun, new T.HemisphereLight("#c0dedd", "#8b8051", 1.65));
  const gltf = await new GLTFLoader().loadAsync("/models/harbor-v2.glb", (e) =>
    progress?.(e.loaded / e.total),
  );
  const root = gltf.scene;
  root.name = "Harbor Blender environment";
  root.traverse((o) => {
    if (o.isMesh) {
      o.castShadow = true;
      o.receiveShadow = true;
      for (const m of Array.isArray(o.material) ? o.material : [o.material]) {
        if (m.map) m.map.anisotropy = 4;
        m.side = T.DoubleSide;
      }
    }
  });
  scene.add(root);
  const geos = [];
  let seed = 10;
  function rand() {
    seed = (seed * 1664525 + 1013904223) >>> 0;
    return seed / 4294967296;
  }
  for (let i = 0; i < 110; i++) {
    const a = rand() * 6.28,
      r = 45 + rand() * 160;
    const geo = new T.IcosahedronGeometry(5 + rand() * 5, 2);
    geo.scale(1.8, 0.6, 1.1);
    geo.translate(Math.cos(a) * r, -14 - rand() * 13, Math.sin(a) * r);
    geos.push(geo);
  }
  scene.add(new T.Mesh(mergeGeometries(geos), material("#f3f1df")));
  geos.forEach((g) => g.dispose());
  const wg = new T.CircleGeometry(1, 96);
  wg.rotateX(-Math.PI / 2);
  wg.scale(POOL.rx, 1, POOL.rz);
  const waterMat = new T.ShaderMaterial({
    uniforms: { time: { value: 0 } },
    transparent: true,
    depthWrite: false,
    side: T.DoubleSide,
    vertexShader: `varying vec3 pos;void main(){pos=position;gl_Position=projectionMatrix*modelViewMatrix*vec4(position,1.);}`,
    fragmentShader: `uniform float time;varying vec3 pos;void main(){float r=sin(pos.x*2.8+time*.6+sin(pos.z*2.1))*sin(pos.z*3.2-time*.45+sin(pos.x));float light=pow(max(0.,r),12.);float wave=sin(pos.x*.4+time*.25)+cos(pos.z*.5-time*.3);vec3 color=mix(vec3(.035,.34,.35),vec3(.15,.62,.58),.5+wave*.14);color+=vec3(.35,.48,.40)*light;gl_FragColor=vec4(color,.87);}`,
  });
  const water = new T.Mesh(wg, waterMat);
  water.position.set(POOL.x, POOL.y, POOL.z);
  scene.add(water);
  const keeper = avatar("#807b4c");
  keeper.position.set(2, height(2, -17), -17);
  scene.add(keeper);
  mesh(
    keeper,
    new T.CylinderGeometry(0.33, 0.34, 0.08, 14),
    "#8a7047",
    0,
    1.86,
    0,
  );
  mesh(
    keeper,
    new T.CylinderGeometry(0.17, 0.23, 0.21, 12),
    "#8a7047",
    0,
    1.98,
    0,
  );
  const smith = avatar("#835e3f");
  smith.scale.setScalar(1.14);
  smith.position.set(13, height(13, -1.5), 1);
  scene.add(smith);
  box(smith, 0, 0.95, 0.2, 0.39, 0.6, 0.03, "#563f2a");
  const trader = avatar("#5e8862");
  trader.position.set(-5, height(-5, -10), -6.4);
  scene.add(trader);
  const bell = new T.Group();
  bell.position.set(3, height(3, -24) + 5.7, -24);
  mesh(bell, new T.CylinderGeometry(0.35, 0.67, 1, 20), "#b59750");
  mesh(
    bell,
    new T.CylinderGeometry(0.76, 0.76, 0.13, 20),
    "#b59750",
    0,
    -0.55,
    0,
  );
  scene.add(bell);
  return { root, water, waterMat, bell, keeper };
}
