# Greybox layout revisions — 2026-09-08

## 2026-09-10 — side-route playtest preparation

The side route now passes west of the giant-tree trunk, approaches Mira and Bram from their open south fronts, and reaches the workshop from its south entrance. The previous polyline collided with the tree and crossed building walls. Landmark positions and building footprints are unchanged. `Side` in BHWorld.cpp is the implemented route; BHSideWalk verified grounded traversal in 50.61 seconds. The arrival-to-restoration regression and persistence tests also pass. These technical checks do not lock the layout; the first human playtest is pending.

Reference landmark coordinates and footprints are unchanged. Collision testing requires the following path refinements to the 2D reference polyline:

- Dock ramp reaches the 7 m south bank elevation at Y=-54 m, before the terrace edge.
- Village approach reaches 14 m at (16,18), before entering the village terrace.
- Orin approach reaches 20 m at (-10,44), then passes (-14,53) and (-14,63) along the east side of the house. A west spur connects to the front porch. This avoids walking through the house and its NPC.
- Tower approach reaches 26 m at (-3,65), then crosses a flat landing to (0,65), before entering the hill platform. This prevents the perpendicular ramp joint from exceeding the capsule's step height.

These are provisional gameplay layout revisions, not permission for final environment art. The authoritative implemented polyline is the `Main` array in `Source/Cloudwake/BHWorld.cpp`; the traversal test follows it using CharacterMovement with ground-contact and elevation assertions.
