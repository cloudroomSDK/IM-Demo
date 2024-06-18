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
      <el-form-item label="SDK服务器：" prop="sdkServer">
        <el-input v-model="form.sdkServer" />
      </el-form-item>
      <el-form-item label="业务服务器：" prop="businessServer">
        <el-input v-model="form.businessServer" />
      </el-form-item>
      <el-form-item label="鉴权方式：" prop="useToken">
        <el-select v-model="form.useToken">
          <el-option label="账号密码鉴权" :value="false" />
          <el-option label="动态Token鉴权" :value="true" />
        </el-select>
      </el-form-item>
      <template v-if="!form.useToken">
        <el-form-item label="AppID：" prop="appId">
          <el-input v-model="form.appId" />
        </el-form-item>
        <el-form-item label="AppSecret：" prop="appSecret">
          <el-input v-model="form.appSecret" type="password" />
        </el-form-item>
      </template>
      <el-form-item v-else label="Token：" prop="token">
        <el-input v-model="form.token" />
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

import {
  businessServer as defaultBusinessServer,
  sdkServer as defaultSdkServer,
  appId as defaultAppId,
  appSecret as defaultAppSecret,
  useToken as defaultUseToken,
  token as defaultToken,
} from "~/config"; //获取默认值

import { updateBaseURL } from "~/utils/request";
import { commonValidator } from "~/utils";
import type { FormInstance, FormRules } from "element-plus";

const configStore = useConfigStore();

const emit = defineEmits(["close"]);

const ruleFormRef = ref<FormInstance>();

const form = reactive({
  businessServer: defaultBusinessServer,
  sdkServer: defaultSdkServer,
  appId: defaultAppId,
  appSecret: defaultAppSecret,
  token: defaultToken,
  useToken: defaultUseToken,
});

onMounted(() => {
  form.businessServer = configStore.businessServer;
  form.sdkServer = configStore.sdkServer;
  form.appId = configStore.appId;
  form.appSecret = configStore.appSecret;
  form.token = configStore.token;
  form.useToken = configStore.useToken;
});

const validateToken = (rule: any, value: any, callback: any) => {
  if (/\s/.test(value)) {
    return callback(new Error(`token不能包含空格`));
  }
  if (!/^([A-Za-z0-9-_=]+\.){2}[A-Za-z0-9-_+\/=]*$/.test(value)) {
    return callback(new Error("token格式不正确"));
  }

  callback();
};

const rules = reactive<FormRules<typeof form>>({
  businessServer: [
    { required: true, message: "请输入服务器地址", trigger: "blur" },
  ],
  sdkServer: [{ required: true, message: "请输入服务器地址", trigger: "blur" }],
  appId: [{ validator: commonValidator, trigger: "blur" }],
  appSecret: [{ validator: commonValidator, trigger: "blur" }],
  token: [{ validator: validateToken, trigger: "blur" }],
});

const submitForm = () => {
  ruleFormRef.value?.validate((valid) => {
    if (valid) {
      configStore.businessServer = form.businessServer;
      configStore.sdkServer = form.sdkServer;
      configStore.useToken = form.useToken;
      if (form.useToken) {
        configStore.token = form.token;
      } else {
        configStore.appId = form.appId;
        configStore.appSecret = form.appSecret;
      }
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
          padding-left: 118px;
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
