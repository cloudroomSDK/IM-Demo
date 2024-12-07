<template>
  <div class="setting">
    <div class="title">
      <el-icon @click="emit('close')" style="cursor: pointer"
        ><i-ep-arrowLeft /></el-icon
      ><span>登录设置</span>
    </div>
    <el-form
      hide-required-asterisk
      ref="ruleFormRef"
      style="max-width: 362px"
      :model="form"
      :rules="rules"
      label-width="auto"
      class="form"
    >
      <el-form-item label="服务器：" prop="businessServer">
        <el-input v-model="form.businessServer" />
      </el-form-item>
      <el-form-item style="margin-bottom: 0">
        <el-button class="btn" type="primary" @click="submitForm">
          保存
        </el-button>
      </el-form-item>
      <el-form-item>
        <el-button
          class="btn"
          type="danger"
          plain
          @click="ruleFormRef?.resetFields()"
        >
          重置为默认
        </el-button>
      </el-form-item>
    </el-form>
  </div>
</template>
<script lang="ts" setup>
import { onMounted, reactive, ref } from "vue";
import { useConfigStore } from "~/stores";

import { businessServer as defaultBusinessServer } from "~/config"; //获取默认值

import { updateBaseURL } from "~/utils/request";
import type { FormInstance, FormRules } from "element-plus";

const configStore = useConfigStore();

const emit = defineEmits(["close"]);

const ruleFormRef = ref<FormInstance>();

const form = reactive({
  businessServer: defaultBusinessServer,
});

onMounted(() => {
  form.businessServer = configStore.businessServer;
});

const rules = reactive<FormRules<typeof form>>({
  businessServer: [
    { required: true, message: "请输入服务器地址", trigger: "blur" },
  ],
});

const submitForm = () => {
  ruleFormRef.value?.validate((valid) => {
    if (valid) {
      configStore.businessServer = form.businessServer;
      updateBaseURL();
      emit("close");
    }
  });
};
</script>
<style lang="scss" scoped>
.setting {
  padding: 58px 0;
  .title {
    display: flex;
    align-items: center;
    padding: 0 44px;
    font-size: 24px;
    margin-bottom: 40px;
    span {
      margin-left: 12px;
    }
  }
  .input-group {
    padding: 0 50px;
    .input-item {
      border: 1px solid #e1e1e1;
      height: 50px;
      display: flex;
      align-items: center;
      padding-left: 18px;
      color: #666;
      font-size: 16px;
      margin-bottom: 24px;
      border-radius: 4px;
      .left {
        width: 100px;
        text-align: right;
      }
      input {
        border: none;
        font-size: 16px;
        line-height: 16px;
        height: 100%;
        flex: 1;
      }
      :deep(.el-select__wrapper) {
        box-shadow: none;
      }
    }
  }
  .btn {
    margin-top: 12px;
    height: 56px;
    font-size: 18px;
    width: 100%;
  }
}

.setting {
  padding: 58px 0;
  .title {
    display: flex;
    align-items: center;
    padding: 0 44px;
    font-size: 24px;
    margin-bottom: 40px;
    font-weight: 700;
    span {
      margin-left: 12px;
    }
  }
  .form {
    margin: 0 auto;
    .el-form-item {
      position: relative;
      margin-bottom: 24px;

      :deep(.el-form-item__label-wrap) {
        position: absolute;
        z-index: 20;
        left: 18px;
        top: 50%;
        margin-top: -16px;
        label {
          color: #666;
          font-size: 16px;
          padding-right: 0;
        }
      }
      :deep(.el-form-item__content) {
        .el-input__wrapper,
        .el-select__wrapper {
          padding-left: 86px;
          height: 48px;
        }
        .el-select__wrapper {
          text-align: right;
          .el-select__selected-item {
            color: #000;
            font-size: 16px;
            padding-right: 20px;
          }
        }
        .el-input {
          height: 48px;
          .el-input__inner {
            font-size: 16px;
            color: #000;
          }
        }
        .el-form-item__error {
          font-size: 14px;
          margin-top: 4px;
        }
      }
    }
  }
  .btn {
    margin-top: 12px;
    height: 56px;
    font-size: 18px;
    width: 100%;
  }
}
</style>
