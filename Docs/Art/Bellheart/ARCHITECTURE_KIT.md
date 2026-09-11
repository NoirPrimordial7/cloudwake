# Bellheart architecture kit — first construction pass

16 editable Blender sources, FBX exports and imported Unreal static meshes. This kit is ready for assembly trials, not final art approval. The playable level was not modified by the kit import.

![Actual Blender geometry review](Reviews/ArchitectureKit_ActualBlender_V001.png)

White markers in the render are 180 cm tall. Stone joints were staggered and the roof corner was corrected to two triangular hip planes after the first visual review. The wall, door and window meshes are decorative geometry; opening/collision assembly still needs a doorway traversal review. No collision replacement is authorized by this import alone.

## Imported dimensions

| Asset | Bounds X × Y × Z (cm) |
|---|---|
| SM_BH_WoodBeam_A | 30.46 × 29 × 300.22 |
| SM_BH_WoodBeam_B | 137.15 × 20 × 137.15 |
| SM_BH_Wall_Wood_A | 325.45 × 25.5 × 301.07 |
| SM_BH_Wall_Stone_A | 297 × 35 × 285 |
| SM_BH_Foundation_Stone_A | 297 × 60 × 141 |
| SM_BH_Roof_Teal_A | 320 × 273.77 × 207.95 |
| SM_BH_Roof_Teal_Corner | 316.5 × 316.5 × 225.36 |
| SM_BH_Window_A | 146 × 20.5 × 169 |
| SM_BH_Door_A | 171 × 27.38 × 249 |
| SM_BH_Stairs_A | 200 × 182 × 96 |
| SM_BH_Railing_A | 319.4 × 17 × 105.31 |
| SM_BH_Fence_A | 319.4 × 17 × 105.31 |
| SM_BH_Awning_A | 315 × 162.5 × 278.5 |
| SM_BH_Banner_A | 134 × 13 × 169.5 |
| SM_BH_DockModule_A | 300 × 318 × 137 |
| SM_BH_Bridge_A | 300 × 618 × 137 |

## Source and rebuild

References: fishing shop master, forge master and material bible. Each mapping is recorded in `tools/production/architecture_manifest.json`. Editable sources are in `tools/blender/Source`, exports in `Content/Cloudwake/Art/ArchitectureKit/Source`, and Unreal packages in `/Game/Cloudwake/Art/ArchitectureKit`.

Run `build_architecture_kit.py` in Blender, then `import_architecture_kit.py` through Unreal Python. `render_architecture_review.py` generates this actual-geometry review. Imports use the same unit/axis conversion as the existing environment; place with scale `(1,-1,1)` until the authoring pipeline is deliberately migrated and retested.

The 3 m wall module has boundary posts outside the nominal board width. Assembly should share or overlap boundary posts intentionally; do not use mesh bounds as automatic grid spacing. Window and door are insert modules, not boolean-cut wall sections.

## Remaining acceptance work

Assemble a small shop corner in a separate review level; inspect roof seams, shared posts, doorway clearance, UV density, shading under Unreal lighting and collision. Keep individual source objects editable. Surface wear and architectural storytelling remain a later refinement, guided by the locked masters.
