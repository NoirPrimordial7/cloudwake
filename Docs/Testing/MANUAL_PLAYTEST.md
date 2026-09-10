# Bellheart: your first playtest

Run **Playtest-Bellheart.cmd** in the project folder. It starts at the dock with a fresh character. Saves go to the separate `Bellheart_Playtest_v1` slot; your normal checkpoint is preserved. Closing and reopening this launcher starts fresh again; F9 can load a checkpoint from this playtest slot.

Spend about 15–30 minutes on this first pass. The blocks and text labels are placeholders. We are judging distances, space and gameplay clarity before committing to detailed models.

## Controls

Inventory update: mouse wheel or 1–4 selects a hotbar slot. Tab opens the kit: click gear, then a slot to move it. Buy a bag/bucket outside Mira's shop and use F to store carried items in the selected container. See INVENTORY_PLAYTEST.md for capacity and retrieval rules. Rod and knife are no longer permanently assigned to keys 1 and 2.

Click inside the game to capture the mouse. WASD moves; mouse looks; Shift sprints; Space jumps. E interacts/picks up. G drops. 1-4 selects the assigned hotbar item; the mouse wheel cycles slots. Hold/release left click to cast, click during BITE to hook, then hold to reel and release to lower tension. R retrieves/cancels the line. Left click strikes a landed fish or attacks with the knife. F5 saves; F9 loads.

## Route to try

1. Follow the gold path from the dock toward the village and find **Orin** at the uphill house.
2. Visit **Mira** at The Cloud Catch for a rod.
3. Go to the lower pond bank, cast into the water, hook and reel a fish ashore.
4. Click the landed fish until it is dead. E picks it up. Take it to Mira's **sell counter**, then press E.
5. Buy the **Iron Knife** at Bram's counter. Try the sharpening station if you can afford it.
6. Inspect the socket at the Bell Tower, then ask Mira for bait.
7. Use the bait lure in the east arena. Select the knife in your hotbar and fight Bellcrab. Move away during the red slam warning.
8. Pick up the dropped Bellheart, carry it back to the tower, and install it.
9. Try the waterfall/hidden/tree side path and workshop approach. Drop an item and pick it up; save and reload once.

## Tell me what feels wrong

- [ ] Dock → Orin: too far, too short, or about right?
- [ ] Can you recognize the tower and giant tree from the main route?
- [ ] Did you find Mira and Bram without getting lost?
- [ ] Is the fishing bank roomy enough to land and handle a fish?
- [ ] Was casting/reeling understandable? Was the tension timing annoying?
- [ ] Can you comfortably aim at each shop counter?
- [ ] Is the boss arena roomy enough to dodge?
- [ ] Is carrying Bellheart uphill tedious?
- [ ] Did a slope, step, path edge or object block you unexpectedly?
- [ ] Does the island feel too empty or too cramped at this scale?

For a bug, tell me **where you were, what you pressed, what happened, and what you expected**. For layout feedback, a short reply such as “Orin is too far; fishing bank is good; boss arena needs more room” is enough.

## Layout decision

**Not locked yet.** Automated checks cover mechanics and traversal; your playtest supplies the pacing/readability feedback. Art production follows on `art/bellheart-pass-01`. Main remains the working greybox baseline; do not merge this branch until its changes have been reviewed.
