import { SlateElement } from "@wangeditor/editor";
import { FaceElement } from "./custom-types";
import { unicodeToChar } from "~/utils/face";

/**
 * 生成“表情”元素的 HTML
 * @param elem 表情元素，即上文的 myResume
 * @param childrenHtml 子节点的 HTML 代码，void 元素可忽略
 * @returns “表情”元素的 HTML 字符串
 */
function faceToHtml(elem: SlateElement, childrenHtml: string): string {
  const { name = "" } = elem as FaceElement;
  return unicodeToChar(name);
}

// 配置
const conf = {
  type: "face", // 新元素的 type ，重要！！！
  elemToHtml: faceToHtml,
};

export default conf;
