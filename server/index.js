import http from "node:http";
import { randomUUID, randomBytes } from "node:crypto";
import { WebSocketServer } from "ws";
import { GameRoom } from "./simulation.js";
import { createServer as createViteServer } from "vite";
import { readFile } from "node:fs/promises";
import path from "node:path";
const production = process.argv.includes("--production"),
  port = Number(process.env.PORT) || 4175;
const vite = production
  ? null
  : await createViteServer({
      configFile: false,
      server: {
        middlewareMode: true,
        allowedHosts: true,
        watch: {
          ignored: [
            "**/tools/**",
            "**/art/**",
            "**/server/data/**",
            "**/.npm-cache/**",
          ],
        },
      },
      appType: "spa",
    });
const mime = {
  ".html": "text/html",
  ".js": "text/javascript",
  ".css": "text/css",
  ".glb": "model/gltf-binary",
  ".png": "image/png",
  ".jpg": "image/jpeg",
  ".json": "application/json",
  ".woff2": "font/woff2",
};
const server = http.createServer(async (req, res) => {
  if (req.url === "/health") {
    res.setHeader("Content-Type", "application/json");
    res.end(JSON.stringify({ ok: true, rooms: rooms.size }));
    return;
  }
  if (vite) {
    vite.middlewares(req, res);
    return;
  }
  try {
    const pathname = decodeURIComponent(
      new URL(req.url, "http://localhost").pathname,
    );
    const file = path.resolve("dist", "." + pathname);
    if (
      !file.startsWith(path.resolve("dist") + path.sep) &&
      file !== path.resolve("dist")
    )
      throw Error();
    const target = pathname === "/" ? path.resolve("dist/index.html") : file;
    res.setHeader(
      "Content-Type",
      mime[path.extname(target)] || "application/octet-stream",
    );
    res.end(await readFile(target));
  } catch {
    res.statusCode = 404;
    res.end("Not found");
  }
});
const rooms = new Map(),
  sessions = new Map();
const wss = new WebSocketServer({
  noServer: true,
  maxPayload: 4096,
  perMessageDeflate: false,
});
server.on("upgrade", (req, socket, head) => {
  if (req.url?.split("?")[0] !== "/game") return;
  wss.handleUpgrade(req, socket, head, (ws) => wss.emit("connection", ws, req));
});
wss.on("connection", (ws) => {
  let joined = null,
    playerId = null;
  let count = 0,
    last = Date.now();
  ws.on("message", (buffer) => {
    try {
      if (Date.now() - last > 1000) {
        count = 0;
        last = Date.now();
      }
      if (++count > 80) {
        ws.close(1008, "Rate exceeded");
        return;
      }
      const m = JSON.parse(buffer.toString());
      if (!m || typeof m !== "object") return;
      if (m.type === "join" && !joined) {
        let code =
          typeof m.code === "string" ? m.code.trim().toUpperCase() : "";
        if (code && !/^[A-Z0-9]{6}$/.test(code))
          throw Error("Use a six-character crew code.");
        if (code && !rooms.has(code))
          throw Error("That crew was not found. Create a new voyage.");
        if (!code) {
          if (rooms.size >= 50)
            throw Error("The harbor is busy. Please try later.");
          do {
            code = randomBytes(3).toString("hex").toUpperCase();
          } while (rooms.has(code));
          const room = new GameRoom(code, (id, msg) => {
            for (const [key, s] of sessions)
              if (
                s.room === room &&
                (!id || key === id) &&
                s.ws.readyState === 1
              )
                s.ws.send(JSON.stringify(msg));
          });
          rooms.set(code, room);
        }
        const room = rooms.get(code);
        playerId = randomUUID();
        room.join(
          playerId,
          typeof m.name === "string"
            ? m.name.replace(/[<>\x00-\x1f]/g, "").trim() || "Sailor"
            : "Sailor",
        );
        joined = room;
        sessions.set(playerId, { ws, room });
        ws.send(JSON.stringify({ type: "welcome", id: playerId, code }));
        ws.send(JSON.stringify(room.snapshot()));
        return;
      }
      if (joined) joined.action(playerId, m);
    } catch (e) {
      if (ws.readyState === 1)
        ws.send(
          JSON.stringify({
            type: "error",
            text: e.message || "Invalid message",
          }),
        );
    }
  });
  ws.on("close", () => {
    if (joined) {
      joined.leave(playerId);
      sessions.delete(playerId);
      if (!joined.players.size) rooms.delete(joined.code);
    }
  });
});
setInterval(() => {
  for (const room of rooms.values()) {
    room.tick(0.05);
    const data = JSON.stringify(room.snapshot());
    for (const p of room.players.values()) {
      const s = sessions.get(p.id);
      if (s?.ws.readyState === 1 && s.ws.bufferedAmount < 128000)
        s.ws.send(data);
    }
  }
}, 50);
server.listen(port, "0.0.0.0", () =>
  console.log(`Cloudwake multiplayer ready at http://localhost:${port}`),
);
