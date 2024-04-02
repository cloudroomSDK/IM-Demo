import { createApp } from "vue";
import "./style.scss";
import App from "./App.vue";
import router from "./router";
import { createPinia } from "pinia";
import piniaPluginPersistedstate from "pinia-plugin-persistedstate";
import ElementPlus from "element-plus";
//@ts-ignore
import zhCn from "element-plus/dist/locale/zh-cn.mjs";

const pinia = createPinia();
pinia.use(piniaPluginPersistedstate); //数据持久化插件，https://prazdevs.github.io/pinia-plugin-persistedstate/zh/guide/why.html
createApp(App)
  .use(pinia)
  .use(router)
  .use(ElementPlus, {
    locale: zhCn,
  })
  .mount("#app");

// 使el-button点击后处于失焦状态，清除焦点的样式
document.body.addEventListener("click", ({ target }: any) => {
  if (target.className.includes && target.className.includes("el-button"))
    target.blur();
  if (target.parentElement?.className.includes("el-button"))
    target.parentElement.blur();
});
