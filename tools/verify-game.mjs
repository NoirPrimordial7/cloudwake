import { chromium } from "@playwright/test";
import fs from "node:fs/promises";
import assert from "node:assert/strict";
const browser = await chromium.connectOverCDP(
  process.env.CLOUDWAKE_CDP || "http://127.0.0.1:55041",
);
const context = browser.contexts()[0];
const page = await context.newPage();
await page.setViewportSize({ width: 1440, height: 900 });
const errors = [];
page.on("pageerror", (e) => errors.push(e.message));
await page.goto("http://127.0.0.1:4173");
await page.evaluate(() => {
  localStorage.removeItem("cloudwake-save-v1");
  localStorage.removeItem("cloudwake-layout-v1");
});
await page.reload();
await page.waitForFunction(() => !!window.cloudwake);
await page.screenshot({ path: "art/title-screen.png" });
await page.locator("#start").click();
const snap = () => page.evaluate(() => window.cloudwake.getSnapshot());
async function walkAxis(axis, target) {
  let last = Infinity,
    stuck = 0;
  for (let i = 0; i < 170; i++) {
    const s = await snap(),
      delta = target - s.position[axis];
    if (Math.abs(delta) < 0.5) break;
    const key = axis === 0 ? (delta > 0 ? "d" : "a") : delta > 0 ? "s" : "w";
    await page.keyboard.down("Shift");
    await page.keyboard.down(key);
    await page.waitForTimeout(90);
    await page.keyboard.up(key);
    await page.keyboard.up("Shift");
    if (Math.abs(delta - last) < 0.02) stuck++;
    else stuck = 0;
    last = delta;
    if (stuck > 12)
      throw Error("Movement blocked at " + JSON.stringify(await snap()));
  }
  assert.ok(
    Math.abs((await snap()).position[axis] - target) < 0.7,
    "Reached " + axis + ":" + target,
  );
}
await walkAxis(2, -10);
await walkAxis(0, -3.5);
await page.keyboard.press("e");
await page
  .getByRole("button", { name: "Take the casting bell", exact: true })
  .click();
assert.equal((await snap()).state.quest, 1);
// Walk around the shop into the western pier.
await walkAxis(0, -5);
await walkAxis(2, 12);
await walkAxis(0, -23);
for (let i = 0; i < 5; i++) {
  await page.keyboard.press("f");
  await page.waitForFunction(
    () => {
      const c = window.cloudwake.getSnapshot().catching;
      return c?.phase === "ring" && c.value > 0.62 && c.value < 0.73;
    },
    null,
    { timeout: 12000, polling: "raf" },
  );
  await page.keyboard.press("f");
  assert.equal((await snap()).state.wisps, i + 1);
}
await page.screenshot({ path: "art/catching-pier.png" });
// Return to keeper, redeem quest once.
await walkAxis(0, -5);
await walkAxis(2, -10);
await walkAxis(0, -3.5);
await page.keyboard.press("e");
await page
  .getByRole("button", { name: "Restore the wind bell", exact: true })
  .click();
assert.equal((await snap()).state.quest, 2);
assert.equal((await snap()).state.coins, 30);
await walkAxis(0, -5);
await walkAxis(2, 7);
await walkAxis(0, -11);
await page.keyboard.press("e");
await page
  .getByRole("button", { name: "Buy charm · 16 crowns", exact: true })
  .click();
assert.equal((await snap()).state.upgrade, true);
await page
  .getByRole("button", { name: "Back to adventure", exact: true })
  .click();
await walkAxis(0, 12);
await page.keyboard.press("e");
await page
  .getByRole("button", { name: "Sell spare wisps", exact: true })
  .click();
assert.equal((await snap()).state.wisps, 0);
assert.equal((await snap()).state.coins, 30);
await page
  .getByRole("button", { name: "Back to adventure", exact: true })
  .click();
await walkAxis(0, 0);
await walkAxis(2, 35);
await page.keyboard.press("e");
assert.equal((await snap()).aboard, true);
await walkAxis(2, 39);
await page.keyboard.press("e");
assert.equal((await snap()).sailing, true);
const z = (await snap()).boat[2];
await page.keyboard.down("w");
await page.waitForTimeout(2000);
await page.keyboard.up("w");
assert.ok((await snap()).boat[2] > z + 2);
await page.screenshot({ path: "art/sailing.png" });
await page.keyboard.press("r");
assert.equal((await snap()).aboard, false);
await page.keyboard.press("m");
assert.match(await page.locator("#modalContent").innerText(), /MAP DISCOVERED/);
await page
  .getByRole("button", { name: "Back to adventure", exact: true })
  .click();
await page.keyboard.press("Tab");
assert.equal((await snap()).editing, true);
await page.locator("#objectSelect").selectOption("Lantern_Tree");
await page.locator("#flagEmblem").selectOption("moon");
await page.locator("#saveLayout").click();
await page.screenshot({ path: "art/workshop.png" });
await page.locator("#closeEditor").click();
const buffer = await page.evaluate(async () =>
  Array.from(new Uint8Array(await window.cloudwake.exportGLB())),
);
await fs.writeFile("art/cloudwake-harbor.glb", Buffer.from(buffer));
assert.ok(buffer.length > 10000);
await page.reload();
await page.waitForFunction(() => !!window.cloudwake);
assert.equal((await snap()).state.quest, 2);
assert.equal((await snap()).state.upgrade, true);
assert.equal((await snap()).state.emblem, "moon");
await page.locator("#start").click();
await page.waitForTimeout(600);
await page.screenshot({ path: "art/harbor-gameplay.png" });
assert.deepEqual(errors, []);
await fs.writeFile(
  "art/verification.json",
  JSON.stringify(
    {
      passed: true,
      errors,
      snapshot: await snap(),
      checks: [
        "real keyboard navigation",
        "quest accepted",
        "5 timing catches",
        "quest turn-in",
        "upgrade purchase",
        "sell catches",
        "board",
        "helm",
        "sail",
        "return dock",
        "map unlock",
        "editor",
        "GLB export",
        "reload persistence",
      ],
    },
    null,
    2,
  ),
);
console.log(
  "PASS: full gameplay, editor, persistence, GLB export; no JS errors",
);
await page.close();
await browser.close();
