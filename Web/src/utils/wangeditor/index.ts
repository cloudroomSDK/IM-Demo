import "@wangeditor/editor/dist/css/style.css"; // 引入 css
import { Boot } from "@wangeditor/editor";

import faceModule from "./faceModule";
export * from "./faceModule/custom-types";

// node_module里面修改过提及功能的样式及文本字符输出，参见patches/@wangeditor+plugin-mention+1.0.0.patch的修改
// 此处修改会在npm install时自动添加修改补丁，是patch-package插件的能力。
import mentionModule from "@wangeditor/plugin-mention";

Boot.registerModule(faceModule);
Boot.registerModule(mentionModule);
