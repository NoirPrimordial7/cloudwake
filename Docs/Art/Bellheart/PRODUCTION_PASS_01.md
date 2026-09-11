# Bellheart environment pass 01

Branch: `art/bellheart-pass-01`. This is an initial integrated environment pass, not final art approval. The high-detail reference renders remain the quality target.

## What is built

96 editable Blender sources and corresponding FBX exports: modular terrain patches and cliff wedges, Lantern Tree, four village structures, Bell Tower, dock, pond, waterfall, path network and a small reusable kit. Repeated planting, shoreline rocks and fences use hierarchical instancing. Architecture remains separately editable from terrain and gameplay actors.

The real playable map is `/Game/Maps/Bellheart`. Its original station actors, quest socket, boss arena and tested path collision are retained. Hidden greybox collision is intentional during this first art review; it is not the final building collision pass. NPCs, equipment, fish and Bellcrab still use their existing gameplay prototypes.

`tools/production/art_manifest.json` records each asset's reference, editable source, export and all placements. `PRODUCTION_ASSET_INDEX.md` lists the concrete Unreal destinations. Dock authoring uses the island aerial because dedicated dock images were not present. Lighting derives from the supplied pond and island master because separate lighting sheets were not present.

## Rebuild

1. Run Blender 4.2 with `--background --threads 4 --python tools/production/build_bellheart.py`.
2. Compile `CloudwakeEditor` using `Build-Unreal.ps1` or Unreal Build Tool.
3. Run UnrealEditor-Cmd with the project, `-run=pythonscript -script=E:/Try/tools/production/import_bellheart.py -BHReimport -unattended -nullrhi`.
4. Run `Test-Unreal.ps1` before accepting placement/collision edits.
5. Launch `/Game/Maps/Bellheart -game -BHTest -BHArtShots` for eight player-level shots and an aerial review. These are photographic stations, not proof of manual traversal. The separate walking regressions exercise CharacterMovement through the routes.

FBX import uses explicit legacy settings and 0.01 import compensation for the authoring export. Placement reflects the imported Y axis so the authored coordinates match the established Unreal layout. Reimport preserves these settings. Do not remove that correction without revalidating terrain, shops and pond positions.

## Visual review

The first render exposed oversized import units and mirrored terrain; corrected before accepting traversal. Subsequent review corrected reversed sign lettering, missing instanced-material usage flags, hard path shoulders and the tower's overly open lower silhouette. Material and cloud shaders must be validated in the renderer, not only with a successful null-RHI import.

Still below the reference target: authored terrain transitions and shoreline composition, rounded canopy and bark detail, architectural asymmetry and interior dressing, richer cliff layering, Cloudsea composition, polished water/foam/mist, and integrated NPC presentation. The first pass must not be described as premium-quality finished environment art. Keep the existing gameplay review gate before replacing collision or narrowing paths.

## Construction kit update — September 11

An additional 16-piece architecture kit is imported and dimension-checked. See [architecture kit review](ARCHITECTURE_KIT.md) and [import bounds](architecture_import_report.json). The existing 96-piece environment assembly remains separate. The acquired StyleHex sample is pending the Unreal Fab plugin's separate sign-in and download; it is not yet integrated foliage.
