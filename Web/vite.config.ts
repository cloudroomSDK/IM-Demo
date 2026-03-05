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
  build: {
    outDir: "im_wasm",
  },
  resolve: {
    alias: {
      "~": path.resolve("src"),
    },
    extensions: [".js", ".ts", ".json", ".vue"],
    preserveSymlinks: true,
  },
  css: {
    preprocessorOptions: {
      scss: {
        silenceDeprecations: ["legacy-js-api"],
        additionalData: `@use "~/element.scss" as *;`,
      },
    },
  },
  optimizeDeps: {
    exclude: ["crim-wasm-sdk"],
  },
  plugins: [
    vue(),
    // vueDevTools(),
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
  define: {
    __APP_VERSION__: JSON.stringify(process.env.npm_package_version),
  },
});
