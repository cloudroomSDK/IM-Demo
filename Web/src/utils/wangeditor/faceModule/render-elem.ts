import { h, VNode } from "snabbdom";
import { DomEditor, IDomEditor, SlateElement } from "@wangeditor/editor";
import { FaceElement } from "./custom-types";
import { getFaceUrl } from "~/utils/face";

/**
 * 渲染“表情”元素到编辑器
 * @param elem 表情元素，即上文的 myResume
 * @param children 元素子节点，void 元素可忽略
 * @param editor 编辑器实例
 * @returns vnode 节点（通过 snabbdom.js 的 h 函数生成）
 */
function renderFace(
  elem: SlateElement,
  children: VNode[] | null,
  editor: IDomEditor
): VNode {
  // 当前节点是否选中
  const selected = DomEditor.isNodeSelected(editor, elem);

  // 构建 vnode
  const { name = "" } = elem as FaceElement;
  const iconVnode = h("img", {
    props: {
      src: getFaceUrl(name),
    },
    style: {
      width: "2em",
      height: "2em",
      padding: "0.1em 0.1em",
      margin: "0 0.1em",
      backgroundColor: selected ? "#3981fc" : "transparent",
      verticalAlign: "-6px",
    },
    on: {
      click(e) {
        setTimeout(() => {
          editor.move(1);
        });
      },
    },
  });

  return iconVnode;
}

const conf = {
  type: "face", // 节点 type ，重要！！！
  renderElem: renderFace,
};

export default conf;
