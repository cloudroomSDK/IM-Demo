import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
import path from "node:path";
import AutoImport from "unplugin-auto-import/vite";
import Components from "unplugin-vue-components/vite";
import { ElementPlusResolver } from "unplugin-vue-components/resolvers";
import Icons from "unplugin-icons/vite";
import IconsResolver from "unplugin-icons/resolver";
import ElementPlus from "unplugin-element-plus/vite";
import vueDevTools from "vite-plugin-vue-devtools";

export default defineConfig({
  base: process.env.NODE_ENV === "development" ? "/" : "/web/im/",
  resolve: {
    alias: {
      "~": path.resolve("src"),
    },
    extensions: [".js", ".ts", ".json", ".vue"],
  },
  css: {
    preprocessorOptions: {
      scss: {
        additionalData: `@use "~/element.scss" as *;`,
      },
    },
  },
  optimizeDeps: {
    exclude: ["@cloudroom/im-wasm-sdk"],
  },
  plugins: [
    vue(),
    vueDevTools(),
    AutoImport({
      resolvers: [
        ElementPlusResolver(/* {
          // 自动引入修改主题色添加这一行，使用预处理样式，不添加将会导致使用ElMessage，ElNotification等组件时默认的主题色会覆盖自定义的主题色
          importStyle: "sass",
        } */),
        // 自动导入图标组件
        IconsResolver({
          prefix: "Icon",
        }),
      ],
    }),
    Components({
      resolvers: [
        // 自动注册图标组件
        IconsResolver({
          enabledCollections: ["ep"],
        }),
        ElementPlusResolver({
          // 按需引入修改主题色添加这一行，使用预处理样式
          importStyle: "sass",
        }),
      ],
    }),
    Icons({
      autoInstall: true,
    }),
    ElementPlus({
      useSource: true,
    }),
  ],
  build: {
    rollupOptions: {
      input: {
        main: path.resolve(__dirname, "index.html"),
      },
      output: {
        // entryFileNames: "[name].js",
        manualChunks(id) {
          if (id.includes("indexeddb-main-thread-worker-b24e7a21.js")) {
            return "DBWorker";
          }
        },
      },
    },
    sourcemap: true,
  },
  worker: {
    rollupOptions: {
      output: {
        entryFileNames: "IMWorker-[hash].js",
      },
    },
  },
  define: {
    __APP_VERSION__: JSON.stringify(process.env.npm_package_version),
  },
});
