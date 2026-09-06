# Cloudwake — first-person multiplayer prototype

Play locally at http://127.0.0.1:4175 after running `Start-Cloudwake.cmd` (`npm run dev`). Node.js is required. For a production server: `npm run build`, then `npm start`. Set PORT to change the port. The server listens on all interfaces.

## This build

One editable floating island, first-person keyboard/mouse controls, 1–4 player room codes, shared money and quest progress. Fish the spring, defeat the living catch, carry it to the buyer and sell it. Buy an iron knife for 24 crowns; sharpen it for 12, then 28 crowns. Rest at the bench to heal. Sell three catches to unlock the Bellmaw lure, defeat Bellmaw, carry it to Orin and reveal the next island chart.

The next island is not built yet. Guns, sailing, gambling, hunger, persistent saves, accounts, character animation and final art are unfinished. The earlier third-person boat/editor prototype source remains in src/main.js and src/world.js, but is not used by this build. Concept images are visual targets, not screenshots of this game. This is Three.js with Blender-authored assets, not an Unreal project or Steam release.

## Controls

- WASD move; Shift run; mouse look. Click the game for mouse lock.
- 1 rod, 2 fists, 3 purchased knife.
- Aim into the pond and click/release to cast (F also casts).
- On a bite, hold left mouse to reel; release to reduce tension.
- Left mouse attacks with fists or knife. E carries defeated catches and interacts with shops/NPCs. G drops a catch.
- B casts the boss lure after three sales. E returns the defeated boss to Orin.
- E revives a downed friend; R respawns after four seconds. Escape opens the menu; other players keep playing.

## Friends

Host a voyage, then share the same server URL and six-character crew code. Friends choose Join crew. Money, catches and quest state are server-owned. A room closes and its progress disappears when the last player disconnects. Refreshing does not restore your personal equipment.

A temporary Cloudflare preview is recorded in server/tunnel.log. It works while this computer, production server and tunnel remain running. It is not permanent hosting. Only production dist assets and the game endpoint are exposed by that server. Cloudflare quick tunnel documentation: https://developers.cloudflare.com/cloudflare-one/networks/connectors/cloudflare-tunnel/do-more-with-tunnels/trycloudflare/

## Editing

Open `art/v2/Cloudwake-FirstPerson.blend` with `Open-Blender.cmd`. Objects remain separated into collections, and textures are packed. The portable Blender executable is in tools/blender/blender-4.2.9-windows-x64.

Save your edits, then run `Export-Harbor.cmd` to export a render-optimized GLB using a separate background Blender process. This preserves the saved source objects. Reload the game; production needs `npm run build` again. Moving gameplay stations also requires updating shared/game.js and matching interaction/collision positions.

`tools/build-harbor-v2.py` regenerates the entire source scene and replaces hand edits: use it only when intentionally rebuilding. Runtime fish, players, water, equipment and bell are in src/fps-world.js. Interface and controls: src/fps.js and src/fps.css. Rules, prices, species and colliders: shared/game.js. Authoritative room simulation: server/simulation.js. Network/static server: server/index.js.

## Verification

`npm test` runs five multiplayer simulation tests and four legacy tests. `npm run build` creates dist. Two real Chrome clients were checked for joining, movement replication and JavaScript errors. tools/verify-v2.mjs records that check (requires a live Chrome CDP session; update its port). This does not constitute latency/load testing or final multiplayer QA.
