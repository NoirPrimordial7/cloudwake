# Bellheart scale guide

1 Unreal unit = 1 cm. Modeling dimensions below are deliberate gameplay starting dimensions; collision, reach, cameras and navigation still need blockout validation. References do not constitute tested architecture.

| Asset / space | Dimensions | Gameplay reason |
|---|---|---|
| Player | 180 cm; eye168 cm | First-person sightlines and counter visibility |
| Orin / Mira / Bram / Tavi | 174 /168 /190 /172 cm | Shared adult proportions with distinct silhouettes |
| Standard door | clear120 ×230 cm | Comfortable single-player traversal and carried objects |
| Workshop repair door | clear400 ×350 cm | Skiff hull access, separate human door |
| Main path | clear300 cm | Two players passing, carrying a catch |
| Side path | clear180 cm | Optional route; avoid mandatory co-op bottleneck |
| Stairs | rise15 cm / tread32 cm; width220 cm | Predictable movement; verify controller step settings |
| Fence | 105 cm tall | Readable boundary without hiding vistas |
| Shop counter | 95 cm high /75 cm deep | Hands and stock visible below eye level |
| Service aisle | minimum120 cm | NPC placement and equipment clearance |
| The Cloud Catch | 900 ×700 ×620 cm | Counter, display, stockroom and working aisle |
| Bram’s Forge | 1100 ×800 ×680 cm | Separate hot forge and shopping areas |
| Orin’s House | 800 ×700 ×600 cm | Quest table plus readable interior circulation |
| Workshop | 1200 ×900 ×700 cm | Skiff repair bay and loft |
| Bell Tower | 800 ×700 ×3200 cm | Island sightline landmark and accessible service deck |
| Bell diameter | 240 cm | Dominant silhouette with plausible yoke |
| Bellheart | 70 cm tall, max30 cm wide | Important two-handed carry object, fits socket |
| Giant tree | 1700 cm high /2000 cm crown /280 cm trunk | Secondary landmark; lanterns above230 cm |
| Island | approx15000 ×19000 cm | First-island walking loops, not an open-world continent |
| Pond | 4600 ×3400 cm | Multiple fishing banks and visible fish habitats |
| Bellcrab arena | clear2200 ×1800 cm | Telegraph readability, dodging and two exits |
| Bellcrab | 480 cm claw span /350 cm depth /220 cm tall | Large but navigable melee opponent |
| Dock | main width400 cm; length1900 cm | Arrival and skiff loading |
| Old / Basic rod | 190 /210 cm | Cast silhouette with tapered tip and reachable reel |
| Iron knife | 28 cm overall | Working knife, no oversized fantasy sword |
| Fish basket | 55 ×35 ×40 cm | Carry/readability; large species held separately |
| Cloudfin / Golden Cloudfin | 35 cm | Same silhouette and mesh, rare material variant |
| Pebble Carp / Lantern Minnow | 45 /18 cm | Clear body-size contrast |
| Silver Snapper / Stonejaw | 55 /75 cm | Stronger catches increase physical presence |

Terrain heights are in the style bible and `18_Blockout/layout_coordinates.json`. World origin is dock elevation; never confuse tower hill+26m with tower top+58m. Waterfall first drop ends at−18m; island rock reaches−45m. Camera target is roughly70–80° horizontal field of view, with hands kept below central aim. Final FOV remains a player setting.
