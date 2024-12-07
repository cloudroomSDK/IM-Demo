import { IDomEditor, SlateDescendant, SlateElement } from "@wangeditor/editor";
import { FaceElement } from "./custom-types";
import { unicodeToChar } from "~/utils/face";

/**
 * 解析 HTML 字符串，生成“表情”元素
 * @param domElem HTML 对应的 DOM Element
 * @param children 子节点
 * @param editor editor 实例
 * @returns “表情”元素，如上文的 myResume
 */
function parseHtml(
  domElem: Element,
  children: SlateDescendant[],
  editor: IDomEditor
): SlateElement {
  // 从 DOM element 中获取“表情”的信息
  const name = domElem.getAttribute("data-name") || "";

  // 生成“表情”元素
  const myResume = {
    type: "face",
    name: name,
    children: [{ text: unicodeToChar(name) }], // void node 必须有 children ，其中有一个空字符串，重要！！！
  } as FaceElement;

  return myResume;
}
const parseHtmlConf = {
  selector: 'img[data-w-e-type="face"]', // CSS 选择器，匹配特定的 HTML 标签
  parseElemHtml: parseHtml,
};

export default parseHtmlConf;
