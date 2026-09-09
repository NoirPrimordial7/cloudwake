# Development log

## 2026-09-08 — Unreal greybox and first playable loop

User released Phase 0 for greybox/gameplay production only. Found installed Unreal 5.8.2 at E:\UE_5.8. Installed Visual Studio 2022 Build Tools, MSVC and Windows/.NET SDK prerequisites. Created a native C++ project alongside the browser game, compiled it, generated an editable Bellheart map and pressed Play in Unreal Editor.

Implemented player movement, camera interaction, fish investigation/bite/hook/reel/physical landing, fish damage/carry/drop/sale, Crowns, knife purchase/sharpening, NPC text dialogue, ordered quest events, Bellcrab slam combat, physical Bellheart pickup and restoration sound/color feedback. The rendered runtime integration test passed. A separate grounded walking test found and drove fixes for the elder-house path and tower landing, then passed. Evidence and limitations are in Docs/Testing/BELLHEART_SCALE_LAYOUT_REVIEW.md.

Environment assets remain primitives. No final architecture, giant tree or cliff models were created. Co-op, persistence, full dialogue branching and final presentation remain future work.

## 2026-09-06 — Phase 0 supersedes Unreal production

User redirected work to a comprehensive Bellheart Isle visual bible before detailed production. Existing browser prototype remains intact. Unreal launcher inspection found no installed engine; no Unreal game build has been compiled or tested. Blender4.2.9 and Git LFS3.7.1 are available.

Created the Bellheart art directory, supplied-reference archive, written style bible, canonical15-location coordinate plan, material palette and scale guide. Plan uses60 purposeful sheets; multi-view building, equipment, UI and gameplay sheets consolidate the much larger list of individual requested angles. Original split-wind-bell emblem replaces compass/star emblems.

Aerial candidate initially lacked the workshop. A targeted image edit added it while preserving the remaining island. Measured top-down, flow and elevation diagrams use one coordinate source. Generated building and character masters are under review, not yet production-approved.

No source-art asset is marked implemented merely because a concept exists. Manifest and gallery distinguish planned, candidate and reviewed states.
