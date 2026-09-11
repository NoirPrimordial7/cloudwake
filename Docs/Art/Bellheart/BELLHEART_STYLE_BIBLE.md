# Bellheart Isle — visual authority, version 001

Concept images remain production targets, never evidence of playable Unreal assets. The user authorized production on `art/bellheart-pass-01` after the greybox playtest. The first environment pass uses these references while retaining the tested gameplay coordinates. See `PRODUCTION_PASS_01.md` for implementation status and remaining visual gaps.

## Source analysis

The two supplied images are the initial visual anchors: `00_Style/CW_BH_STYLE_SUPPLIED_FORGE_V001.png` and `00_Style/CW_BH_STYLE_SUPPLIED_POND_V001.png`. The forge image establishes large warm timber members, layered tool displays, matte teal cloth, softened iron, restrained bronze, readable hands and a compact contextual shop card. The pond image establishes connected terraced terrain, clear walking banks, irregular pale rock edging, turquoise shallows, dense perimeter planting, a broad tree canopy and a dominant bell silhouette. Neither image is a measured level plan. Their exact scene layouts and emblem are not construction authority.

## Shape and construction language

- Premium stylized 3D: simplified major forms with crafted bevels, medium-scale grain and joints. No realistic grime, tiny surface noise, plastic toy specularity or faceted placeholder terrain.
- Buildings have thick timber frames over pale stone plinths. Human-scale doors, slightly oversized exposed joinery and roof overhangs. Walls remain relatively quiet against detailed counters.
- Roofs use overlapping rounded rectangular teal wooden shingles, a shallow concave sweep near the eaves, dark timber ridge and exposed rafter ends. No pagodas, onion domes or unrelated roof families.
- Stone is warm pale limestone with asymmetric large blocks and eased chipped corners; dark recesses and restrained moss in sheltered joints. Cliffs form layered fractured masses with a narrowing rocky underside.
- Wood is honey oak on lit cuts and umber in protected frames. Broad directional grain; peg joints, rope ties and occasional bronze straps. No excessively glossy varnish.
- Metal: matte dark iron tools, warm brushed bronze bells and fittings, sparse turquoise oxidation in recesses. Bell silhouette and clapper mechanism must remain mechanically readable.

## Palette and emblem

Teal #286C70, deep teal #173B42, limestone #D5C6A1, oak #997043, dark timber #483321, bronze #B58A48, iron #535957, rope #BFA477, cream #E9DFC2, leather #704D32, meadow #778844, leaf shadow #3E633D, flowers #F3EDD7, water #36B2B1. These are approximate base-color targets, not sampled final rendered pixels. Lighting changes appearance.

Original emblem: **split wind bell** — a cream inverted-U bell outline, a short central vertical clapper and two outward curved wind strokes; no star or compass rose. Single emblem throughout banners, apron patch and Bellheart fitting. Banners: teal rectangular field with one shallow V notch, cream stitched border, emblem centered. Code-native emblem drawing is geometry authority; concept render deviations are errors, not variants.

## Vegetation, paths and landscape

Giant tree: 20 m crown diameter, 17 m tall, twisted 2.8 m trunk, three primary spreading boughs, layered rounded angular leaf masses. Root buttresses merge into rock and soil; lanterns hang below clear head height. Small flowers and grasses cluster at roots and banks; no random coverage on traversal routes.

Main paths are 3 m clear, side paths 1.8 m clear; golden packed earth with irregular embedded limestone. Fences use uneven stout oak posts, two rails or rope, 1.05 m high, intentional gaps at fishing access. Planting covers roughly 60–75% of non-path soil in major views, with larger quiet lawn patches in the arena. Water is clear turquoise with readable stone bed, soft ripples and pale foam only at falls and contacts.

## People and equipment

All adult NPCs use approximately 6.5-head proportions, broad expressive faces, substantial hands/boots, believable joints and layered working clothes. Matte cream linen, teal accents, brown leather, bronze fasteners. No anime eyes or modern zippers. Orin 174 cm, Mira 168 cm, Bram 190 cm, Tavi 172 cm. First-person player eye height 168 cm at standing height 180 cm. Rod and knife remain below the central aiming region; leather fingerless gloves and rolled cream sleeves.

Fishing rods are tapered oak with wrapped cord grip, dark iron/brass reel and clear line guides. Knives are working tools with short wide steel blades and leather-bound handles. Fish have readable eyes, broad fins and selective scale planes, not microscopic scales or comic googly eyes. Bellcrab is a friendly-dangerous fantasy crustacean, never grotesque.

## Light, sky and UI

One lighting family: late morning, warm sunlight from southwest, cool open-sky fill, readable soft shadows. Large layered white cumulus and blue atmospheric distance; no sunset, heavy bloom or neon magic. Lanterns add warm local pools inside shaded rooms without turning the scene into night.

HUD: small health lower-left, four equipment slots lower-center, Crowns upper-right and quest beneath. Deep teal backing, restrained bronze corners, cream legible text, generous contrast. Original split-bell icon and simple crown; no exact tracing of supplied UI. Context cards show one interaction with price/stat, not an RPG grid. All gameplay shots use a plausible first-person camera, not oversized arms or third-person cutscene framing.

## Canonical spatial plan

Coordinates are meters, +Y north toward tower, +X east. Dock approach begins south. Island approximately 150 m east–west by 190 m north–south, tapering toward the arrival dock. Rock underside descends 45 m below dock. Do not mirror layouts between shots.

| ID | Site | X | Y | Ground Z |
|---|---|---:|---:|---:|
| BH-01 | Dock | 0 | -86 | 0 |
| BH-02 | Arrival Path | 0 | -65 | 3 |
| BH-03 | Central Pond | 0 | -20 | water 6 |
| BH-04 | Giant Tree | -34 | 5 | 10 |
| BH-05 | The Cloud Catch / Mira | -24 | 30 | 14 |
| BH-06 | Bram's Forge | 27 | 30 | 14 |
| BH-07 | Bellkeeper's House / Orin | -20 | 58 | 20 |
| BH-08 | Tinker's Workshop | 46 | 10 | 10 |
| BH-09 | Village Plaza | 0 | 30 | 14 |
| BH-10 | Bell Tower | 0 | 76 | 26 |
| BH-11 | Waterfall Fishing Area | -47 | -24 | 6 to -18 |
| BH-12 | Bellcrab Arena | 26 | -11 | 7 |
| BH-13 | Lower Fishing Bank | -10 | -43 | 7 |
| BH-14 | Hidden Path | -51 | 17 | 11 |
| BH-15 | Cloud Skiff Area | 22 | -83 | 0 |

Pond is an irregular 46 × 34 m basin, flowing west into a narrow stream and over the cliff. Main route: dock → rising arrival path → bridge across narrow southern pond inlet → east bank → plaza → Orin → tower stairs. Arena is a flat 22 × 18 m bay east of pond with at least two escape routes. Upper ruins terrace reaches 31 m north of tower; tower itself reaches 58 m total elevation. Shops open south toward plaza/pond. Workshop is east behind the arena, accessed from plaza, never placed inside combat space.

## Fixed asset identities

- Bell Tower: 8 × 7 m footprint, 32 m tall above hill, four pale-stone corner piers, front pointed stone arch, single bronze bell hung between upper piers, teal banner beneath, exterior stair on east, bronze clapper socket reached from a timber interior service deck. Broken west parapet; no redesign between views.
- The Cloud Catch: 9 × 7 m, 6.2 m ridge; one teal gable, west stone chimney, south covered counter, east side door, north stockroom window; oval hanging fish sign and rope-wrapped posts.
- Bram's Forge: 11 × 8 m, 6.8 m ridge; teal asymmetric gable, broad south counter, west stone forge chimney, east grindstone, rear anvil bay. Dense tools on rear timber wall; safe walking lane behind counter.
- Orin's house: 8 × 7 m, 6 m ridge; teal gable, east stone chimney, south arched door, west round study window.
- Workshop: 12 × 9 m, 7 m ridge; teal sawtooth annex, wide south skiff repair doors, external east pulley crane and west loft window.

## Approval and change policy

Manifest distinguishes supplied anchor, candidate, reviewed master, derived reference and rejected. Generation success does not equal review. Lock only after comparing image identity, layout, material family and scale. Maps and dimensional tables override ambiguous perspective. Reference sheets guide form; final mesh measurements come from SCALE_GUIDE.md. Record any difference and regenerate important violations before modeling. Never silently overwrite a locked version: increment V### and record the replacement decision.
