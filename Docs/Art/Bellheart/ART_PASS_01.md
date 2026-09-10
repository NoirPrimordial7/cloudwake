# Bellheart Art Pass 01

Branch: `art/bellheart-pass-01`. Baseline/fallback: main at `05e596c`.

Current stage: **post-playtest narrative and interaction corrections**. On 2026-09-10 the user reported enjoying the gameplay and requested clearer motivation, Cloudsea/fall rules and speaker-attached dialogue. Preserve the tested footprints and routes; resolve these corrections before held-asset production. This is positive gameplay feedback, not a final environment-art approval.

## Production order

1. Playtest and lock greybox.
2. Player-held assets: rod, knife, bobber, hook, Bellheart.
3. Fish: Cloudfin quality benchmark, then remaining species.
4. Bellcrab.
5. Modular environment kit: beams, walls, roofs, stone, windows, doors, fences.
6. Fishing Shop.
7. Bram's Forge.
8. Elder House.
9. Workshop.
10. Bell Tower.
11. Dock.
12. Giant Tree.
13. NPC characters.
14. Props.
15. Materials refinement.
16. Foliage.
17. Water, clouds and lighting.
18. Animations.
19. UI.
20. Audio and VFX.
21. Final Bellheart polish.

Material assignments needed to review individual models are created with those assets; the later materials stage is the island-wide refinement pass.

Every modeled asset needs its named reference, editable Blender source, export, Unreal destination and in-game scale/collision review. Read ASSET_REFERENCE_MAP.md before modeling. Some listed image references do not yet exist: notably the player-equipment sheets. Establish and inspect those references before creating the corresponding meshes. Do not substitute an invented design or claim a planned path is an existing asset.

Do not add Island 2, co-op or unrelated systems during this pass. Preserve footprints and interaction positions after lock. Integrate and test each replacement before progressing; preserve the main greybox fallback. Do not merge this branch into main before visual/gameplay review.

## Playtest preparation

`Playtest-Bellheart.cmd` starts a fresh standalone Unreal session using a separate playtest checkpoint slot. `Docs/Testing/MANUAL_PLAYTEST.md` provides the route and feedback checklist. The technical side-route check was added because the previous main-route test did not visit the tree/hidden path/workshop. Its initial run found the giant-tree trunk blocking the route. Corrections route around the trunk and approach shop fronts without passing through walls. Building and landmark positions are unchanged.

Layout status: **awaiting user playtest**, not locked for detailed environment modeling.
