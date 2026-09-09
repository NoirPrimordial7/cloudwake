# Bellheart Isle playable greybox

Engine: Unreal Engine 5.8.2 installed at `E:\UE_5.8`. This is a new native Unreal project, alongside the existing browser prototype.

The user's production instruction of 2026-09-08 releases the Phase 0 gate **for greybox and gameplay only**. It does not approve final environment models. The canonical dimensions and positions remain in `Art/Bellheart/18_Blockout/layout_coordinates.json`. The aerial, top-down, flow and elevation sheets guide layout; measured coordinates govern placement.

## Open and edit

Run `Open-Unreal.cmd`, open `/Game/Maps/Bellheart`, then press Play. Compile after changing C++ using `Build-Unreal.cmd` with the editor closed. The map is generated once by `tools/create-bellheart-map.py`; subsequent runs preserve designer edits. All placed cubes, path ramps, signs and stations can be moved in the editor. Keep the canonical reference coordinates updated after an approved layout revision.

Movement: WASD, mouse look, Shift sprint, Space jump. E interacts. Hold/release left mouse to cast; click during BITE to hook, hold to reel and release to reduce tension. Click a landed fish to finish it. E carries it; G drops it. 1 selects rod, 2 selects knife.

## Slice route

Dock → Orin's house → Mira's rod → lower pond bank → physically land and finish fish → sell at Mira's counter → buy knife at Bram's counter → optional sharpening → inspect tower socket → Mira's bait → east arena lure → knife fight → carry Bellheart → install at tower → Tavi.

The first fish can be finished with bare hands for 8 damage, avoiding the circular dependency of needing money for a knife before the first sale. Knife costs 24 Crowns; starting money 12; first fish sells for at least 32. Sharpening costs 12 and raises damage from 18 to 30.

## Architecture

- `UBHProgress`: player-owned currency, ownership and event-gated quest state.
- `ABHInteractable`: camera-traced interaction base and editable station properties.
- `ABHPhysicalItem`: fish AI, physical landing, damage, carry/drop and quest object.
- `ABHPlayer`: first-person character, input, cast/reel, tool presentation and health.
- `ABHBellcrab`: arena-bound combat prototype with visible slam telegraph.
- `ABHWorld`: reproducible measured greybox builder, saved as editable map actors.
- `ABHTestDriver`: command-line-only runtime integration test (`-BHTest`).

## Scope and review gate

This is a single-player greybox milestone. Multiplayer replication, persistence, full dialogue branching, six authored species, polished UI, final models, animations and audio are subsequent work. None of the primitive architecture is final environment production. The scale/layout review must distinguish automated progression tests from manual navigation and combat feel.
