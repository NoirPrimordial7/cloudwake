# Air-control tuning

AirControl 0.62; low-speed boost 1.6 below 250 cm/s; falling lateral friction and falling braking both zero. Walk/sprint remain 500/800 cm/s; jump speed remains 500 cm/s and gravity is unchanged. Releasing sprint in flight defers the walking cap until grounded. This preserves takeoff momentum without adding vertical lift or instantly reversing velocity.

The nine-case CharacterMovement test uses real acceleration, collision and grounded landing on an isolated floor. Measured walk/sprint takeoff speeds were 500/800 cm/s, peak rise 127.6 cm, and flight roughly 1.02 seconds. Sprint plus full lateral input travels about 5.96 m forward and 4.89 m sideways. A-to-D input reverses lateral acceleration gradually. Diagonal speed remains capped. See Bellheart-AirControl-2026-09-10.txt.

Run with `-BHAirTest -game -nullrhi -unattended`; the test never accesses player saves. Keyboard/mouse smoke checks supplement the sustained-input matrix; short injected key chords cannot replace the user's subjective feel review.
