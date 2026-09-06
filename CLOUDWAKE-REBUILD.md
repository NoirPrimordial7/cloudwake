# Cloudwake — first-person fantasy fishing rebuild

Research and design brief · 6 September 2026

Status: proposed replacement design, not implemented gameplay. The existing playable project remains the earlier single-player, third-person prototype. New reference images are art targets, not screenshots of implemented features.

## 1. What the reference establishes

Dazed Games’ official Steam page confirms a physics-based fishing game for 1–4 players, with catching, killing, selling, equipment progression, quests, bosses, new islands, rare variants, trick-shot rewards and optional gambling with fictional resources. These are verified public features, not information about its internal code. [Official Steam listing](https://store.steampowered.com/app/4001890/How_to_Fish/)

PC Gamer’s first-boss walkthrough describes acquiring fishing equipment and melee equipment before using a quest-related object as bait. This supports a compact tutorial that teaches the economy before its first major encounter. Exact prices are patch-sensitive and are not adopted here. [Spider Crab walkthrough](https://www.pcgamer.com/games/sim/how-to-fish-spider-crab/)

GamesRadar’s walkthrough documents changing island environments and later progression through shops, special bait and boss-related hand-ins. My design inference: each island should introduce one memorable mechanic, provide terrain useful during combat, and connect its exit condition to a local story. Its exact layouts are not reproduced here. [Full walkthrough](https://www.gamesradar.com/games/co-op/how-to-fish-guide-walkthrough/)

The two supplied summaries are valuable design inputs, but they mix public observations with proposed architecture, illustrative numbers, and speculative implementation. Their class names, fishing equations, approximate map drawings and suggested development schedules are not verified facts about How to Fish. They also assume an existing Unreal project; this workspace actually contains Three.js/Vite and Blender assets. Their advice on when to add multiplayer conflicts internally; this rebuild tests two real clients before expanding content.

## 2. New identity and loop

Cloudwake is a first-person, 1–4 player fantasy fishing adventure across floating islands. Water comes from enchanted springs, filling ponds and spilling over island edges into cloud seas. Each island’s magic changes its aquatic creatures. Boats connect settlements; fishing from shore is the main activity on the first island.

Explore → choose bait and fishing spot → charge a cast → hook and reel → land a physical creature → fight it → carry the dead catch → sell it → buy or improve equipment → solve the island’s local problem → earn a chart and sail onward.

The caught creature remains in the world. A successful catch never immediately becomes a wallet reward or an inventory counter. It can flop, charge, bite, flee toward water, be attacked by a friend, and be carried after death. Money arrives only when the buyer accepts a sellable dead catch. Noncombat catches such as shells and salvage can still be collected and sold.

## 3. Wind Bell Harbor: the first island

### Story

Every harbor bell carries a Bellheart that steadies the nearby sky current. During the last storm, Wind Bell Harbor’s Bellheart fell into the spring and was swallowed by a normally harmless carp. Its amplified magic now agitates the fish and makes the outer current unsafe.

The bellkeeper can repair the route, but needs the Bellheart back. The harbor still buys fish for food and magical materials, explaining the market and equipment trade. Recovering the heart restores the bell, calms the spring and reveals the passage to Fairy Lantern Grove. A glimpse of strange pollen on the recovered heart hints that the disturbance started elsewhere.

### Play space

Proposed scale: approximately 120 × 90 metres, with a 70–100 metre main route and roughly 30–45 seconds to cross at an ordinary walking pace. These are design targets, not measurements of the current island. Use an irregular outline, walkable terraces and real paths instead of a flat circular platform.

| Area | Position and contents | Gameplay purpose |
| --- | --- | --- |
| Arrival dock | Southern edge; small skyboat, mooring posts, shared catch basket | Spawn, crew gathering, exit goal |
| Harbor square | Short path north from dock; clear sightline to bell tower | Navigation landmark and safe social space |
| Catch buyer / tackle stall | West of square; scales, physical sell tray, rods and bait | Sell corpses, buy fishing upgrades |
| Smith’s open workshop | East of square; knife on display, visible sharpening wheel | First damage upgrade and paid sharpening |
| Spring pond | West of square; broad shallow bank and small wooden pier | First casts and forgiving fights |
| Spillway | Northeast; deeper water draining toward a cloud waterfall | Stronger catches and a visible risk/reward choice |
| Bellkeeper’s cottage and tower | Raised northern terrace; one continuous ramp | Story giver, quest hand-in and restored landmark |
| Bellmaw clearing | Near spillway, with open ground and two substantial rocks | Boss landing zone, readable attacks and cover |
| Rest bench | Near buyer; restorative food and free slow recovery | Recovery without economic deadlock |

Paths connect pond → buyer → smith → pond in a short loop. Decorative baskets and fences must not block it. Fishing banks need enough dry ground for the creature, player and friends to fight. Boat travel is unnecessary for starting the quest.

### First 20–30 minutes: intended pacing, to be playtested

1. Arrive with a borrowed starter rod and bare hands. Talk briefly to the bellkeeper. The broken bell and pond are visible from the arrival path.
2. Catch a small fish at the shallow bank. It lands alive; finish it with punches, carry it to the buyer, and sell it.
3. Repeat a few catches or sell shoreline salvage to buy the iron knife. No compulsory rare drop.
4. Deliver three ordinary catches to the tackle seller, who pays their normal value and prepares a Resonant Lure using their scales. Previously qualifying deliveries count.
5. Sharpen the knife if desired. The first sharpening is useful but not mandatory.
6. Cast the Resonant Lure in the clearly marked spillway pool. Bellmaw emerges onto the clearing.
7. Dodge a telegraphed flop, attack during recovery, and retrieve the Bellheart after the fight.
8. Return the Bellheart to the keeper. The bell rings, the sky current opens, and the next chart appears on the boat’s chart table.

Boss attempts must be repeatable without buying another quest lure. Essential gear cannot be lost permanently over the edge. The Bellheart is a unique quest item, cannot be sold, and returns to a marked recovery spot if lost.

## 4. Creature roster

All values below are initial design tuning, not values taken from another game. Start with four distinct silhouettes, not four recolored copies.

| Starter species | Identity and behavior | HP | Base sale | Suggested common-pool weight |
| --- | --- | ---: | ---: | ---: |
| Silverfin | Familiar silver fish with slightly translucent fins; short escaping hops | 16 | 8 crowns | 50% |
| Copper Bream | Broad body and warm scales; a short, signaled lunge | 30 | 12 crowns | 30% |
| Mossback Crab | Mossy shell; sidesteps and pinches at close range | 42 | 18 crowns | 15% |
| Lantern Eel | Long body and glowing tail; warns before a small electric pulse | 56 | 24 crowns | 5% |

Weight changes scale and value within limited ranges. A rare pearlescent material variant is optional; never require it for the main story. Tutorial catches use a controlled gentle pool before unlocking the full mix. A living catch has a visible health bar only when targeted or attacking.

Bellmaw: oversized carp with bronze plates and a visible pulsing Bellheart glow. Approximately 180 solo HP initially; three readable moves: wind-up flop, forward rush and a short gust. After each, pause long enough for melee retaliation. It should be feasible with the base knife. Solo and multiplayer tuning must be tested independently; adding players should not produce unavoidable damage.

| Later island | Look and fishing habitat | Distinct catches | Introduced mechanic |
| --- | --- | --- | --- |
| Fairy Lantern Grove | Giant roots, lantern tree, lily pools and mushroom shops | Pixiefin, winged pond sprites, thorn pike | First firearm: rune flintlock; brief airborne enemies |
| Tideglass Sanctuary | Floating coral ring containing a magically suspended lagoon | Pearl rays, crown crabs, hostile mermaid-like reef sirens | Harpoon weapon and tide-driven positioning |
| Moonfair Anchorage | Twilight pirate fair, boardwalks and moonlit pools | Masked carp, moon eels, coin-shell mimics | Optional fictional-coin wagering, cooking, stronger gun |

These are later content plans. Do not build all islands in the next milestone. Fairy citizens and hostile fairy-themed catches need different silhouettes and names so the world is readable.

## 5. Fishing, fighting, health and upgrades

First person: visible gloved hands, authored rod/knife animations, mouse look, adjustable FOV, optional head bob, interaction through a short center-screen aim trace. No orbit camera during normal play.

Casting: hold to charge, release toward water, show a physical bobber and line. A bite has sound and motion. Holding reel increases retrieval and tension; easing off lets tension recover. The old moving-gold-marker minigame is replaced. A failed cast returns the line; a broken line costs time, not the essential rod.

Landing: fish travels to a valid nearby bank, retaining a physical world identity. Do not spawn a dangerous fish inside the player or behind an obstructing wall. Water zones define species and valid landing areas. The player can reposition while reeling.

Combat: aim and click to swing; short range, clear wind-up and recovery, impact sound and reaction. Holding attack must not apply damage every render frame. One swing can damage a target once. Dead fish stop attacking and become carryable; the server owns their death and sale state.

Initial equipment economy:

| Item or action | Proposed cost | Effect |
| --- | ---: | --- |
| Borrowed rod | Free, recoverable | Always permits a basic catch |
| Bare hands | Free | 8 damage; slow but viable against tutorial fish |
| Iron knife | 24 crowns | 18 damage; purchased physically at smith |
| Sharpen I | 12 crowns | Permanent knife increase to 24 damage |
| Sharpen II | 28 crowns | Permanent increase to 32 damage |
| Improved reel | 18 crowns | More forgiving tension handling |
| Restorative food | 6 crowns | Recover health; a free slow rest option also exists |

Three base Silverfin sales fund the knife. Do not spend the player's starting budget on required bait. Sharpening is a permanent tier upgrade in the first slice, not a hidden recurring maintenance fee. The wheel shows current damage, new damage and exact price before purchase. Later islands sell guns and ammunition; no starter-island gun arsenal.

Health: 100 maximum initially; directional damage feedback, attack cooldowns and readable warning animations. In co-op, use a downed state and a short teammate revive. Solo defeat returns the player to camp; unsold ordinary catches can remain recoverable nearby, while key gear and quest items are protected. Friendly fire off by default.

Hunger: if included, make it a light food/satiety bonus initially. It should not kill an idle player, interrupt the fishing tutorial, or force a repetitive food bill. Health and combat feel come first.

## 6. Interface redesign

The current beige modal shop screens and large fixed quest box are retired for the rebuild.

- HUD: small health bar at lower left; four clear equipment slots along the bottom; compact wallet and crew indicators; one short tracked objective; tiny center dot.
- Aim prompts: item name and a single action near the crosshair, only inside reach. Show value/weight when looking at a catch, not over everything in the scene.
- Shops: actual equipment displayed on a counter, visible price plaques and an NPC. Looking at an item opens a compact comparison card with price, current/new stat, and Buy. Unaffordable items explain the missing amount. Owned upgrades cannot charge again.
- Selling: place a dead catch in a physical weighing tray. Show species, weight, payout and Sell. After confirmation, remove that exact world object and award money once.
- Sharpening: use a visible wheel. A brief animation, sparks and sound make the improvement tangible.
- Dialogue: short, dismissible lines near the NPC with a clear quest action; reserve full panels for journal, settings and crew management.
- Co-op: unobtrusive teammate names, health/downed indicators and room code in the crew panel. Opening a shop or pause menu does not pause the shared world; the market is a safe area.
- Accessibility: adjustable UI scale, subtitles, readable contrast, color-plus-shape rarity cues, remappable controls and reduced camera motion.

First-person art mockups accompany this brief. They are direction references; their generated UI lettering and numbers are not the authoritative gameplay specification.

## 7. Why the current art missed the target

The concepts rely on deliberately shaped buildings, bevelled edges, irregular rock strata, layered vegetation, coherent material variation, nuanced lighting and dense but readable compositions. The prototype substituted simple boxes, a flat disk, sparse grass, generic rock chunks and largely uniform materials. It proved some interactions but did not reproduce that craftsmanship. A renderer or engine name alone does not solve the gap.

Rebuild an actual small scene in Blender using a reusable art kit: one detailed shop facade, a pond bank with rock strata, a terrain terrace, the bell tower, a mature tree, foliage clusters, a rod, first-person arms, a knife and four recognizable creatures. Use UVs and a consistent texture palette; bake detail where helpful; keep collision meshes simpler than visible meshes. First-person assets must withstand close viewing.

Compare actual game captures against the original references at the same eye height and field of view. Review pond edge, market counter and tower approach before multiplying assets across the island. The acceptance target is the same material and shape language, not an untestable promise that every generated detail will render identically in real time.

## 8. Co-op and editable implementation

The browser requirement is retained unless you choose a native game later. Existing Three.js rendering and Blender authoring can continue, but the current monolithic client simulation needs restructuring. A proposed Node/Colyseus server can provide authoritative rooms and synchronized game state; this is supported by its official documentation, not yet integrated into Cloudwake. [Colyseus overview](https://docs.colyseus.io/) · [State synchronization](https://docs.colyseus.io/state)

Target: 1–4 players. The server owns creature spawning, health, fishing claims, item ownership, sales, currency, quest progress and boat movement. Clients send intent, predict local movement where appropriate, and interpolate remote entities. Stable IDs identify fish and transaction IDs prevent duplicated payouts. Synchronize useful physics states, not every decorative particle.

Shared story and island unlocks; shared crew treasury for the first version. Purchases clearly state which player receives the item. A dead catch can be picked up by one player at a time. Simultaneous sale, attack, hook and pickup requests resolve on the server. Reconnect restores the player's room identity and rejects replayed transactions.

First prove the loop with two browser clients. Then verify joining from another machine over an actual hosted server. A localhost URL or two tabs is not proof that distant friends can join. Remote co-op needs reachable hosting, secure connections and room access rules. Hosting and operating costs are a separate deployment choice; this research step does not deploy anything.

Editable data: species, loot weights, weapons, sharpening tiers, shop inventory, quest steps, water zones and island unlocks live in versioned data files. Assets remain in named Blender collections with exported glTF scenes. Gameplay markers specify spawn points, fishing volumes, sell trays, interaction anchors and collision regions so visual edits do not silently break interactions. Use a new save version; do not reinterpret old wisp saves as completed combat progression.

## 9. First rebuilt slice and acceptance gates

Build only Wind Bell Harbor initially. Sequence:

1. First-person controller, one polished pond-bank scene, real fishing line and one physical catch.
2. Health, punches, knife, dead-catch pickup and sale; run this with two connected clients before adding species.
3. Physical shop, sharpening, four species, equipment and crew HUD; add the rest of the compact harbor using the tested art kit.
4. Bellmaw quest, protected Bellheart, map unlock, boat travel, save/reconnect and four-player verification.

Must pass before describing the rebuild as playable:

- An ordinary catch lands alive, can hurt the player, can be killed, carried and sold.
- Selling a living fish fails clearly. One corpse never pays twice, even when two players act together.
- Knife purchase and sharpening change measured damage and charge the displayed amount once.
- Two clients agree on fish death, pickup owner, currency and quest state; reconnect does not duplicate items.
- The complete first-island story works solo and with friends without mandatory rare drops or irrecoverable key items.
- The real pond and shop scenes meet the revised art/UI direction in captured gameplay, with readable targets and unobstructed interaction areas.
- Report frame timing on the tested machine at a stated resolution. Do not equate polygon count or a successful build with acceptable performance.

Optional later content: cooking, style bonuses, additional firearm classes and a Moonfair wagering stall using earned fictional coins only. Wagering is never a route unlock or source of required quest resources. Do not add it to the starter tutorial.

## Deliverables from this research pass

This brief and revised first-person fishing/shop concept images. Existing gameplay has not yet been converted to combat or multiplayer. The next development task is a foundational rebuild of the small playable slice above, not a cosmetic patch over the old wisp collector.
