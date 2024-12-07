<template>
  <div style="height: 100%">
    <Editor
      style="height: 100%"
      mode="simple"
      :defaultConfig="editorConfig"
      @onCreated="handleCreated"
      @customPaste="customPaste"
      @keydown.prevent.enter="emit('enter')"
    />
    <mention-modal
      v-if="isShowModal && conversationStore.isCurrentGroupChat"
      @insertMention="insertMention"
      @hideMentionModal="hideMentionModal"
    />
  </div>
</template>

<script setup lang="ts">
import "~/utils/wangeditor";

import { onBeforeUnmount, ref } from "vue";
import { Editor } from "@wangeditor/editor-for-vue";
import { IDomEditor, IEditorConfig } from "@wangeditor/editor";
import { MentionModal } from ".";
import { useConversationStore } from "~/stores";
const conversationStore = useConversationStore();

const editorRef = defineModel<IDomEditor>("editorRef");
const emit = defineEmits(["enter", "pasteFile"]);
const isShowModal = ref(false);

const showMentionModal = () => {
  isShowModal.value = true;
};
const hideMentionModal = () => {
  isShowModal.value = false;
  editorRef.value?.restoreSelection(); // 恢复选区
};
const insertMention = (id: string, name: string) => {
  const mentionNode = {
    type: "mention",
    value: name,
    info: id,
    children: [{ text: `@{${id}|${name}}` }],
  };
  const editor = editorRef.value;
  if (editor) {
    editor.restoreSelection(); // 恢复选区
    editor.deleteBackward("character"); // 删除 '@'
    editor.insertNode(mentionNode); // 插入 mention
    editor.move(1); // 移动光标
  }
};

// 编辑器配置
const editorConfig: Partial<IEditorConfig> = {
  EXTEND_CONF: {
    mentionConfig: {
      showModal: showMentionModal, // 必须
      hideModal: hideMentionModal, // 必须
    },
  },
};

// 自定义粘贴
const customPaste = (editor: IDomEditor, event: any, callback: Function) => {
  const html = event.clipboardData.getData("text/html");
  const text = event.clipboardData.getData("text/plain");
  const fragment = event.clipboardData.getData("application/x-slate-fragment");

  Array.prototype.forEach.call(event.clipboardData.files, (item: any) => {
    // 粘贴的是文件
    emit("pasteFile", item);
  });

  // 阻止默认的粘贴行为 txt没有html样式
  // 处理富文本粘贴及修复editor粘贴纯文本会粘贴两次的问题
  // 参见 https://github.com/wangeditor-team/wangEditor/issues/5560
  if (!html && text) {
    const currentText = editor.getText();
    const MAXLENGTH = /*  props.maxLength ?? */ 5000;
    if (text.length + currentText.length > MAXLENGTH) {
      callback(false);
      event.preventDefault();
      return false;
    }
    return true;
  } else if (fragment) {
    return true;
  } else {
    text && editor.insertText(text);
    event.preventDefault();
    // 继续执行默认的粘贴行为
    return false;
  }
};

// 组件销毁时，也及时销毁编辑器
onBeforeUnmount(() => {
  if (editorRef.value == null) return;
  editorRef.value.destroy();
});

const handleCreated = (editor: any) => {
  editorRef.value = editor; // 记录 editor 实例，重要！
};
</script>
