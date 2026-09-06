import test from "node:test";
import assert from "node:assert/strict";
import {
  freshState,
  sanitizeSave,
  catchWisp,
  restoreBell,
  sellWisps,
  buyCharm,
} from "../src/state.js";
test("quest requires three catches and rewards only once", () => {
  const s = freshState();
  s.quest = 1;
  assert.equal(restoreBell(s), false);
  for (let i = 0; i < 3; i++) catchWisp(s);
  assert.equal(restoreBell(s), true);
  assert.equal(s.wisps, 0);
  assert.equal(s.coins, 30);
  assert.equal(s.quest, 2);
  assert.equal(restoreBell(s), false);
  assert.equal(s.coins, 30);
});
test("trader protects required quest wisps and sells extras", () => {
  const s = { ...freshState(), quest: 1, wisps: 5 };
  assert.equal(sellWisps(s), 2);
  assert.equal(s.wisps, 3);
  assert.equal(s.coins, 16);
  assert.equal(sellWisps(s), 0);
});
test("upgrade cannot overspend or be purchased twice", () => {
  const s = freshState();
  assert.equal(buyCharm(s), false);
  s.coins = 20;
  assert.equal(buyCharm(s), true);
  assert.equal(s.coins, 4);
  assert.equal(buyCharm(s), false);
});
test("corrupt save cannot introduce invalid state", () => {
  const s = sanitizeSave({
    version: 1,
    quest: 99,
    coins: -20,
    wisps: Infinity,
    flag: "<script>",
    emblem: "bad",
  });
  assert.deepEqual(s, freshState());
});
