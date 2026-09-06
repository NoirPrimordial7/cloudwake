export const SAVE_KEY = "cloudwake-save-v1";
export const freshState = () => ({
  version: 1,
  quest: 0,
  wisps: 0,
  coins: 0,
  caught: 0,
  upgrade: false,
  flag: "#287d82",
  emblem: "star",
});
export function sanitizeSave(raw) {
  const s = freshState();
  if (!raw || raw.version !== 1) return s;
  for (const k of ["wisps", "coins", "caught"])
    s[k] = Number.isInteger(raw[k]) ? Math.max(0, Math.min(99999, raw[k])) : 0;
  s.quest = [0, 1, 2].includes(raw.quest) ? raw.quest : 0;
  s.upgrade = raw.upgrade === true;
  if (/^#[0-9a-f]{6}$/i.test(raw.flag)) s.flag = raw.flag;
  if (["star", "moon", "sun"].includes(raw.emblem)) s.emblem = raw.emblem;
  return s;
}
export function catchWisp(s) {
  s.wisps++;
  s.caught++;
}
export function restoreBell(s) {
  if (s.quest !== 1 || s.wisps < 3) return false;
  s.wisps -= 3;
  s.quest = 2;
  s.coins += 30;
  return true;
}
export function sellWisps(s) {
  const available = Math.max(0, s.wisps - (s.quest === 1 ? 3 : 0));
  s.wisps -= available;
  s.coins += available * 8;
  return available;
}
export function buyCharm(s) {
  if (s.upgrade || s.coins < 16) return false;
  s.coins -= 16;
  s.upgrade = true;
  return true;
}
