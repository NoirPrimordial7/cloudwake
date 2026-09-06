export const POOL = { x: -12, z: 5, rx: 9, rz: 10, y: 0.5 };
export const STATIONS = {
  seller: { x: -4, z: -4, label: "Sell carried catch", range: 3.8 },
  knife: { x: 12.2, z: 4.6, label: "Iron knife", range: 3 },
  sharpen: { x: 16.7, z: 4.6, label: "Sharpen knife", range: 3 },
  keeper: { x: 2, z: -17, label: "Bellkeeper Orin", range: 3.4 },
  rest: { x: 5, z: 10, label: "Rest & recover", range: 2.8 },
  chart: { x: 2, z: 27, label: "Sky charts", range: 3 },
};
export const SPECIES = [
  {
    name: "Silverfin",
    hp: 16,
    value: 8,
    color: "#b4d2c5",
    speed: 1.35,
    damage: 6,
  },
  {
    name: "Copper Bream",
    hp: 30,
    value: 12,
    color: "#d9a759",
    speed: 1.65,
    damage: 9,
  },
  {
    name: "Mossback Crab",
    hp: 42,
    value: 18,
    color: "#6c8751",
    speed: 1.05,
    damage: 12,
  },
  {
    name: "Lantern Eel",
    hp: 56,
    value: 24,
    color: "#648f9d",
    speed: 2.1,
    damage: 14,
  },
  {
    name: "Bellmaw",
    hp: 180,
    value: 0,
    color: "#c69e55",
    speed: 1.7,
    damage: 20,
  },
];
export const clamp = (v, a, b) => Math.max(a, Math.min(b, v));
export function height(x, z) {
  return 0.8 + 3 / (1 + Math.exp((z + 15) / 3.5)) + Math.sin(x * 0.15) * 0.13;
}
export function inPool(x, z, padding = 0) {
  return (
    ((x - POOL.x) / (POOL.rx + padding)) ** 2 +
      ((z - POOL.z) / (POOL.rz + padding)) ** 2 <
    1
  );
}
export const OBSTACLES = [
  { x: 13, z: -1.5, rx: 5, rz: 4 },
  { x: -5, z: -10, rx: 4.5, rz: 4 },
  { x: 3, z: -24, rx: 3, rz: 3 },
  { x: -19, z: -17, rx: 1.7, rz: 1.7 },
];
export function walkable(x, z) {
  if (!Number.isFinite(x) || !Number.isFinite(z)) return false;
  const dock = Math.abs(x) < 3 && z > 25 && z < 37;
  const island = (x / 36) ** 2 + ((z + 2) / 32) ** 2 < 0.96;
  if (!(dock || island) || inPool(x, z, 0.55)) return false;
  return !OBSTACLES.some(
    (o) => Math.abs(x - o.x) < o.rx + 0.35 && Math.abs(z - o.z) < o.rz + 0.35,
  );
}
export function movePlayer(p, input, dt) {
  const ax = clamp(Number(input.x) || 0, -1, 1),
    az = clamp(Number(input.z) || 0, -1, 1);
  const len = Math.max(1, Math.hypot(ax, az)),
    speed = input.run ? 6.5 : 4.2;
  const dx = ((Math.cos(p.yaw) * ax - Math.sin(p.yaw) * az) / len) * speed * dt;
  const dz =
    ((-Math.sin(p.yaw) * ax - Math.cos(p.yaw) * az) / len) * speed * dt;
  if (walkable(p.x + dx, p.z)) p.x += dx;
  if (walkable(p.x, p.z + dz)) p.z += dz;
}
