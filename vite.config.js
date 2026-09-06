import { defineConfig } from "vite";
export default defineConfig({
  server: {
    host: "127.0.0.1",
    port: 4173,
    strictPort: true,
    watch: {
      ignored: [
        "**/tools/**",
        "**/art/**",
        "**/.npm-cache/**",
        "**/test-results/**",
      ],
    },
  },
  build: { chunkSizeWarningLimit: 850 },
});
