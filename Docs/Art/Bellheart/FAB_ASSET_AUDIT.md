# Fab / licensed nature audit

Checked 2026-09-11. The StyleHex free forest sample is acquired in the signed-in Fab library under the Standard License; the user accepted the EULA. Download and Unreal import remain pending. No paid package has been purchased. Listing availability is not proof of project ownership. Style assessments below are provisional until the actual meshes/materials can be reviewed in Bellheart.

## Machine and account evidence

- Cloudwake uses `E:/UE_5.8`. Fab was installed under `C:/Program Files/Epic Games/UE_5.8/Engine/Plugins/Fab`, outside that engine. Plugin 0.0.15 targets UE 5.8 and its module BuildId `55116800` matches the active editor. A local project copy now exists in `Plugins/Fab`; vendor binaries are excluded from Git.
- The read-only Fab cache at `C:/ProgramData/Epic/EpicGamesLauncher/VaultCache/FabLibrary/listings_v1.db` has zero listing, acquisition and download records. This does **not** prove that the online account owns nothing.
- The user signed in to Fab. The forest sample page now reports Saved in My Library and Standard License (listing updated September 8, 2026). No authentication stores were read.
- Existing project content consists of authored Cloudwake assets, greybox, maps and audio. No installed licensed natural-content pack was identified in the locations checked.
- Quixel Bridge is not used as the acquisition workflow.

## Candidate audit

| Category | Product / source | License and access | Fit and adaptation | Decision |
|---|---|---|---|---|
| TREES | [FREE Stylized Forest Sample — StyleHex Studio](https://www.fab.com/listings/3c1a31b6-e523-4a0f-97d0-26b0db69b6bc) | Acquired free; user accepted Fab EULA; page confirms Standard License and Saved in My Library. Unreal package pending download. | Stylized fantasy family is promising for secondary trees. Warm leaves, reduce anime saturation, test wind/LOD transitions. Never replace the custom Lantern Tree trunk. | First acquisition candidate; review samples before broad use. |
| SMALL VEGETATION | Same StyleHex sample, ground foliage and variations | Same acquired package; mesh inventory pending import. | Keep plants below sightlines; cluster beside paths and around pond/shops. Verify individual species supplied. | Trial in a small review patch. |
| FLOWERS | [Stylized Environment](https://www.fab.com/listings/6ae05e13-e0ee-468b-a6b0-07404b73d978) | Public listing; price/entitlement not confirmed. No purchase authorization. | Listing advertises flower/grass interaction. Require cream/yellow/pink flowers with large readable petals and restrained wind. | Conditional alternative; inspect account ownership first. |
| GRASS | StyleHex sample; Stylized Environment as alternative | StyleHex forest sample acquired; alternative not acquired. | Replace provisional handmade triangle tufts only after density, wind and GPU comparison. Retain HISM/foliage batching and combat/path exclusions. | Trial; do not blend competing material families unchanged. |
| ROCKS | [Stylized Rocks FREE Pack — Markus-3D](https://www.fab.com/listings/baff54d1-d43b-4924-982e-10dd910fa544) | Listed as free in Fab category and product description; product exposes license selection. Not acquired. | Seven meshes, LODs and collision advertised. Retint to pale warm limestone, dampen normal contrast, fit shoreline scale. | Second acquisition candidate for small rocks. |
| CLIFFS | Same rock pack as possible surface accents | Pending license/access. | Small rock kit cannot establish Bellheart's island silhouette. Custom Blender cliff shells remain authoritative. | Accent only after scale review; no generic replacement of island geometry. |
| GROUND SURFACES | [Stylized Environment](https://www.fab.com/listings/6ae05e13-e0ee-468b-a6b0-07404b73d978) | Unverified package contents/access for specific terrain materials. | Need warm soil and quiet macro variation. Require actual material inventory before adoption. | Hold; no validated surface selected yet. |
| MOSS | [Forest European Broadleaf Rock Moss — Quixel Megascans](https://www.fab.com/listings/b39199a7-abae-40a2-8721-2df03f173988) | License selection required; entitlement/price unknown. Do not assume old Megascans free access applies. | Photorealistic scan. Only consider heavily simplified color/roughness detail, not unmodified albedo. | Low priority; reject unchanged. |
| WOODLAND DETAILS | [Stylized Forest Floor Scatter Pack — UpDraft Art](https://www.fab.com/listings/03f3fc1b-6c02-4505-a668-78bf9e8531ad) | Fab ownership/price not verified. Publisher describes a separate free sampler, but it has not been downloaded. | Branches, leaves and small debris could help terrain transitions; select sparse clusters and consistent palette. | Inspect owned content first; no acquisition yet. |

Project Nature's public search results list grass, fern and flower products, but its directly opened seller page returned no products. Treat those results as unresolved availability, not a usable owned library.

## Acquisition progress

The forest sample is acquired. No further license acceptance is needed for that completed acquisition. Import through the project-local Fab plugin next. The publisher also offers a [free foliage pack](https://www.fab.com/listings/b494d1c5-8d02-48c4-9038-54bb8970a26f) and [free rock pack](https://www.fab.com/listings/3edeea52-d490-4ffd-8b51-70376f5ec82d); these are coherent-family candidates, not acquired assets. Prefer inspecting the acquired sample before adding more packs.

## Foliage technology decision

[Epic's UE 5.8 documentation](https://dev.epicgames.com/documentation/unreal-engine/nanite-foliage?lang=en-US) identifies Nanite Foliage as experimental. Keep conventional instanced foliage as the current baseline. Actual asset comparisons are pending acquisition; no measured claim about Nanite performance or wind quality is made yet.

Test each approach in an isolated copy with the same camera path, plant coverage, texture resolution and lighting. Record GPU milliseconds, frame-time distribution, VRAM, wind silhouette, LOD/voxel transitions and stability across repeated launches. Use Nanite selectively only if it improves those results. A public feature description is not a project benchmark.

## Integration constraints

Use 1–2 coherent nature families. Adapt leaf colors, roughness, scale, wind strength and density against the style bible. Keep third-party originals separate from Cloudwake material instances and record provenance; do not redistribute raw assets in a public repository. Existing generated generic foliage is provisional and should not be expanded while acquisition is pending. Custom architecture, island structure and Lantern Tree hero geometry remain Blender-authored.
