# Inventory and containers

- Mouse wheel selects one of four hotbar slots. Keys 1–4 are shortcuts to those same slots, not hard-coded rod/knife actions.
- Tab opens/closes your kit. Click an owned gear tile, then a slot to swap it there. Click a slot without selecting gear to equip it.
- Buy the satchel (40 Crowns, four items) or fish bucket (28 Crowns, six finished fish) from the displays outside Mira's shop. Each can be bought once.
- Carry a physical item with E. Select the appropriate container and press F (or left click) to store it. Alternatively use the store buttons inside Tab.
- Click a stored item to retrieve it into your hands. Hands must be empty. Close Tab, then G drops it physically or E sells a dead fish at Mira's counter.
- Living fish must be finished before storage. The Bellheart fits the satchel, never the fish bucket; retrieve it before installation.
- Container purchases, contents, item values, hotbar order and selected slot persist in checkpoints. Existing saves without inventory fields still load.
- Inventory does not pause the world. Opening it cancels an active cast. Containers have functional panels but no final held models yet. Gear uses prototype text tiles pending the art/UI pass.

## Reference boundary

The official [How to Fish Steam page](https://store.steampowered.com/app/4001890/?l=english) confirms physical fishing, selling, gear purchases and island/boss progression. It does not document exact inventory controls. Scroll selection, assignable slots and the bag/bucket capacities above implement the user's requested Cloudwake behavior, not a claim about that game's precise rules.

## Verification

Compiled the editor target successfully. Automated persistence tests passed purchases/duplicate rejection, slot reassignment, wheel-wrap logic, six-fish capacity, carried-plus-stored save/load, physical retrieval and Bellheart container restrictions. The full arrival-to-restoration loop passed in 15.83 seconds.

Native Windows input verified selecting the rod tile and moving it from slot four to slot one, retrieving a Cloudfin (bucket 3 to 2; physical fish in hands), storing it again (2 to 3), closing Tab and scrolling from the rod to the next slot. The first unattended OS-cursor-warp check failed to select its tile; it was replaced by this real-input verification rather than reported as passing. BHInventoryPreview remains a rendered screenshot fixture; BHManualPreview keeps that isolated fixture open for input review.
