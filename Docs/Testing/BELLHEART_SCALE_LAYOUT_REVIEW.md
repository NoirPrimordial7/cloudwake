# Bellheart greybox review — 2026-09-08

Status: playable baseline verified; final environment art remains deferred.

## Evidence

- Unreal 5.8.2 Editor target compiled successfully using MSVC 14.44.35228 and Windows SDK 10.0.22621.0.
- Map generation finished with zero errors and zero warnings.
- Opened the saved map in Unreal Editor and pressed Play. First-person camera, HUD and arrival geometry displayed correctly.
- Rendered integration test completed the gameplay loop in 15.89 simulated seconds. It uses camera traces for interactions, real fish AI ticks, reeling, physics landing, damage, pickup, sale, purchase, sharpening, boss damage, physical quest-object pickup and restoration. It teleports between stations to isolate gameplay progression; this is not a navigation test.
- Separate traversal test used CharacterMovement, without teleporting, through 15 waypoints from dock to pond, village, elder approach and tower. Completed in 46.92 simulated seconds. Every waypoint asserted ground contact and intended player-center elevation within 55 cm, allowing the approach slope and arrival radius.
- In-engine screenshots inspected: fishing and prototype Bellcrab. These are actual greybox captures, not concept renders.

Filtered logs are saved beside this document. Full local logs remain in `Saved/Logs` and are not committed.

## Scale decisions

Player capsule is 180 cm high and 68 cm wide, camera at 168 cm. Walk speed is 500 cm/s, sprint 800. Main paths are 300 cm wide; side paths 180 cm. Counter tops are approximately 95 cm. Buildings retain the reference footprints. Tower rises 32 m above its 26 m hill. Pond is 46×34 m; arena 22×18 m. NPC markers are 180 cm tall.

Terrain and paths are separate editable primitives. The main path uses ramps for this phase; final stair/terrain transitions are not modeled. Four approach adjustments and a flat tower landing eliminate terrace-edge collisions and a fall near the elder house. See the greybox layout revision note in the art bible.

## Remaining playtest questions

- Main traversal and progression are verified. Side-path traversal, intentional edge falls, prolonged fishing, fish exhaustion and every boss dodge direction still need broader player testing.
- The broad pond and 500 cm/s movement make the island readable at first-person scale. Whether the return trips feel too long needs human playtesting.
- Shop/NPC text markers and HUD are functional debug presentation. Dialogue choices, a polished four-slot inventory, full fish species, boss animation, multiplayer and save/load are not represented as finished features.
- This first prototype uses one boss slam cycle, primitive body/claws/legs, and one reusable fish actor with two value variants. Final fish anatomy and boss turnaround fidelity remain future asset work.

No final environment models were produced. Do not treat this initial review as a final art lock or a complete multiplayer QA pass.
