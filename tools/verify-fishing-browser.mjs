import {chromium} from '@playwright/test';
import assert from 'node:assert/strict';
const browser=await chromium.connectOverCDP('http://127.0.0.1:50356');
const page=browser.contexts()[0].pages()[0];await page.bringToFront();
await page.reload();await page.waitForFunction(()=>window.cloudwake?.getSnapshot().ready);await page.locator('#start').click();await page.waitForFunction(()=>window.cloudwake.getSnapshot().self);
await page.keyboard.down('w');await page.waitForTimeout(3500);await page.keyboard.up('w');
await page.keyboard.down('a');await page.waitForTimeout(650);await page.keyboard.up('a');
await page.locator('canvas').click();
// Exercise the actual mouse-look handler, without changing game state.
await page.evaluate(()=>{const c=window.cloudwake.getCamera();document.dispatchEvent(new MouseEvent('mousemove',{movementX:(c.yaw-1.3)/.0024,movementY:(c.pitch+.23)/.0024}));}); await page.waitForTimeout(300);
await page.keyboard.press('f');await page.waitForFunction(()=>window.cloudwake.getSnapshot().self.fishing?.phase==='hooked');
await page.mouse.down();await page.waitForFunction(()=>window.cloudwake.getSnapshot().room.fish.length>0);await page.mouse.up();
assert.equal(await page.evaluate(()=>window.cloudwake.getSnapshot().room.fish[0].hp),16);
await page.screenshot({path:'E:/Try/art/v2/live-catch.png'});
console.log(JSON.stringify({browserFishingLanded:true,state:await page.evaluate(()=>({player:window.cloudwake.getSnapshot().self,fish:window.cloudwake.getSnapshot().room.fish}))}));
await browser.close();

