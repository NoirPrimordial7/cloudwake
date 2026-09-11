# Bellheart resume validation — 2026-09-11

## Gameplay regression

Existing compiled Unreal runtime, unchanged gameplay code during this resume. All four existing regression modes completed after the initial kit import. The later roof/stone edits affect unplaced kit assets only.

```text
[2026.09.11-15.59.25:148][541]LogTemp: Display: BH_TEST_COMPLETE full arrival-to-restoration loop 15.84s
[2026.09.11-16.00.18:481][990]LogTemp: Display: BH_WALK_COMPLETE route=dock-pond-village-elder-tower 46.91s
[2026.09.11-16.01.17:549][455]LogTemp: Display: BH_WALK_COMPLETE route=waterfall-hidden-tree-shops-workshop 50.63s
[2026.09.11-16.01.27:911][426]LogTemp: Display: BH_SAVE_TEST_COMPLETE persistence and recovery
```

## Asset import

The corrected 16-piece kit reimport completed with zero commandlet errors and zero warnings. See `architecture_import_report.json` for actual Unreal centimeter bounds. Blender review images show the real .blend models, not generated concept art.

## Render review

Nine photographic stations ran in the real game renderer. No renderer errors or ensures were found in `Saved/Logs/ArtShotsResume.log`. These camera stations do not constitute manual playtesting. The render confirmed that provisional foliage, terrain shoulders, building silhouettes and Cloudsea still need substantial visual refinement.

A cloud material wiring issue was traced to UE 5.8 source: `VolumetricCloud.usf` reads extinction from subsurface data; `Material.cpp` maps volume extinction to `MP_SubsurfaceColor`. The importer now uses that mapping. Saving over the material while the editor held it open failed, so the repair is being checked in a separate `/Game/Cloudwake/Art/Reviews/Bellheart_CloudReview` map. Main-map promotion remains pending; do not claim the repair is already live there.

## External asset gate

The StyleHex forest sample is acquired under the Standard License. Unreal Fab opens successfully but requires its own browser sign-in. Download, mesh/material inventory, in-world foliage comparison and performance checks remain pending that step. No paid purchase was made.

## Cloud review outcome

The isolated map completed all nine photographic stations with synchronous shader compilation (bAllowAsynchronousShaderCompiling=False for that launch). No renderer errors/ensures were logged. Correct extinction wiring produces spatial volume, but the aerial remains grainy and visually flat. **Visual acceptance: rejected; do not promote to the main map yet.** Refine sampling, cloud lighting, density and composition before the next review. Actual captures: [Cloudsea aerial](Reviews/Cloudsea_ActualUnreal_Review_V001.png) and [pond](Reviews/Pond_ActualUnreal_Review_V001.png).
