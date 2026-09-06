# First-person build QA — 6 September 2026

Production build passes. Five new simulation tests pass: catch/kill/carry lifecycle; exclusive ownership and duplicate sale protection; shop proximity/prices/upgrade limits; four-player capacity and disconnect drop; single Bellheart reward. Four legacy tests also pass.

Two local Chrome clients joined one room and replicated movement; no page errors observed. First-person movement, mouse-look, casting and holding to reel were exercised through browser inputs: a live 16-HP Silverfin landed and fists automatically equipped. Screenshots in art/v2 are actual WebGL captures, not concept art.

Visual review caught sparse roofing and overly contrasting meadow triangles. Corrected shingle alignment/overlap and unified meadow material. Captured corrected first-person view. Final material and character fidelity remains far below the concept references; this is a mechanics prototype. Co-op latency, reconnect persistence, performance across hardware, mobile and controller support are not validated.

Editable source is packed in art/v2/Cloudwake-FirstPerson.blend. Background export leaves saved source unchanged. Production serves dist only; development Vite is not the public tunnel target.
`nPublic HTTPS preview also verified with two Chrome clients: room join and movement replication passed, with zero page errors observed. This tests the tunnel from this machine, not a geographically remote human play session.
