import { IModuleConf } from "@wangeditor/editor";

import withFace from "./plugin";
import elemToHtmlConf from "./elem-to-html";
import renderElemConf from "./render-elem";
import parseHtmlConf from "./parse-elem-html";

const module: Partial<IModuleConf> = {
  editorPlugin: withFace,
  renderElems: [renderElemConf],
  elemsToHtml: [elemToHtmlConf],
  parseElemsHtml: [parseHtmlConf],
};

export default module;
