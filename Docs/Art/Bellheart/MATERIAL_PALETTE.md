# Bellheart material palette

Reference: BH-MATERIAL-BIBLE. Approximate **sRGB base-color** values; rendered screenshots are not valid albedo samples. Avoid baking lighting into textures. Roughness ranges are starting points for later material tests, not measured engine results.

| Surface | Base color | Roughness | Construction / detail |
|---|---|---|---|
| Primary honey oak | #997043 | .65–.85 | Broad grain, peg joints, softened cuts |
| Dark timber | #483321 | .7–.9 | Protected frames; preserve shadow readability |
| Pale limestone | #D5C6A1 | .75–.95 | Broad chips, quiet planes, irregular courses |
| Weathered stone | #939482 | .8–1 | Moss only at joints and sheltered bases |
| Bronze | #B58A48 | .35–.6 | Metallic1; sparse blue-green oxidation |
| Iron | #535957 | .45–.7 | Metallic1; worn edges, no mirror chrome |
| Rope | #BFA477 | .8–1 | Broad braided strands, not hair-like noise |
| Teal fabric | #286C70 | .8–1 | Cream stitching, restrained weave |
| Cream linen | #E9DFC2 | .8–1 | Large soft folds, matte |
| Leather | #704D32 | .6–.85 | Warm creases, stitched joins |
| Grass | #778844 | .8–1 | Grouped green/gold blades, not neon |
| Leaves | #3E633D to #A8AD57 | .75–1 | Clustered broad planes, sunlit tops |
| Flowers | #F3EDD7 / #D3AD51 | .7–.95 | Cream petals, muted golden centers |
| Water | #36B2B1 | .05–.2 | Transparent shallows; restrained reflections |
| Roof shingles | #286C70 / #417C79 | .65–.85 | Overlapping rounded painted wooden tiles |
| UI backing | #173B42 | n/a | Opaque enough for legibility; cream text |

Use one material family per island. Introduce variation through value and wear, not unrelated hue families. Bronze, iron and cloth must remain visually distinct. All emissive surfaces are warm lanterns or localized creature/quest cues; no broad neon treatment.

Emblem source: `00_Style/CW_BH_STYLE_EMBLEM_VECTOR_V001.svg`. This geometry overrides small drawing variations in generated banners.
