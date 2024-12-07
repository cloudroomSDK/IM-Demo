import { DomEditor, IDomEditor } from "@wangeditor/editor";

function withFace<T extends IDomEditor>(editor: T) {
  const { isInline, isVoid } = editor;

  editor.isInline = (elem) => {
    const type = DomEditor.getNodeType(elem);
    if (type === "face") return true; // 针对 type: face ，设置为 inline
    return isInline(elem);
  };

  editor.isVoid = (elem) => {
    const type = DomEditor.getNodeType(elem);
    if (type === "face") return true; // 针对 type: face ，设置为 void
    return isVoid(elem);
  };

  return editor; // 返回 newEditor ，重要！！！
}

export default withFace;
