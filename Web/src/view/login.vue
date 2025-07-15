<template>
  <div class="wrap">
    <div class="header">
      <div class="logo">
        <img :src="Logo" alt="" />
      </div>
    </div>
    <div class="bg">
      <div class="box">
        <div v-if="!isSetting" class="login">
          <el-icon class="setting" @click="isSetting = true">
            <i-ep-setting />
          </el-icon>
          <div class="logo">
            <img :src="Logo" alt="" />
          </div>
          <template v-if="!isPwd">
            <p class="big">验证码登录</p>
            <p class="desc">未注册的手机号通过验证后自动注册</p>
            <el-form ref="phoneFormRef" :model="phoneForm" :rules="phoneRules">
              <el-form-item class="formItem" prop="phoneNumber">
                <el-input
                  v-model="phoneForm.phoneNumber"
                  placeholder="请输入手机号"
                  class="input"
                  maxlength="11"
                >
                  <template #prepend>
                    <el-select
                      v-model="select"
                      placeholder="Select"
                      style="width: 80px; height: 100%"
                    >
                      <el-option label="+86" value="1" />
                    </el-select>
                  </template>
                </el-input>
              </el-form-item>
              <el-form-item class="formItem" prop="code">
                <el-input
                  v-model="phoneForm.code"
                  placeholder="请输入验证码"
                  class="input"
                  @keydown.enter="login"
                >
                  <template #append>
                    <div
                      class="timeCount"
                      :style="{
                        cursor: timeCount > 0 ? 'no-drop' : 'pointer',
                      }"
                      @click="getCodeBtn"
                    >
                      <span v-if="timeCount === -1">获取验证码</span>
                      <span v-else>{{ timeCount }}s</span>
                    </div>
                  </template>
                </el-input>
              </el-form-item>
              <p class="toast" v-if="showCodeToast">私有云验证码用8888</p>
            </el-form>
          </template>
          <template v-else>
            <el-form
              ref="accountFormRef"
              :model="accountForm"
              :rules="accountRules"
              style="margin-top: 60px"
            >
              <el-form-item class="formItem" prop="account">
                <el-input
                  v-model="accountForm.account"
                  placeholder="请输入账号"
                  class="input"
                />
              </el-form-item>
              <el-form-item class="formItem" prop="pwd">
                <el-input
                  v-model="accountForm.pwd"
                  placeholder="请输入密码"
                  class="input"
                  type="password"
                  @keydown.enter="login"
                />
              </el-form-item>
            </el-form>
          </template>
          <el-button
            class="btn"
            type="primary"
            @click="login"
            :loading="isLoading"
            :disabled="isLoading"
          >
            登录<template v-if="!isPwd">/注册</template>
          </el-button>
          <div class="togglePswd">
            <el-link type="primary" v-if="isPwd" @click="isPwd = false">
              验证码登录
            </el-link>
            <el-link type="primary" v-else @click="isPwd = true">
              密码登录
            </el-link>
          </div>
        </div>
        <LoginSetting v-else @close="isSetting = false" />
      </div>
      <p class="version">v{{ version }}</p>
    </div>
  </div>
</template>

<script setup lang="ts">
import { useConfigStore, useUserStore } from "~/stores";
import Logo from "~/assets/logo.svg";
import { useRouter } from "vue-router";
import { computed, reactive, ref } from "vue";
import { ElMessage } from "element-plus";
import type { FormInstance, FormRules } from "element-plus";
import loginApi from "~/api/login";
import { API } from "~/api/typings";
import { md5, base64Parms } from "~/utils";
import { LoginSetting } from "~/components";

const configStore = useConfigStore();
const router = useRouter();
const userStore = useUserStore();

const select = ref("1");
const timeCount = ref(-1);
const isSetting = ref(false);
const isPwd = ref(false);
const isLoading = ref(false);
const accountFormRef = ref<FormInstance>();
const phoneFormRef = ref<FormInstance>();
const version = computed(() => __APP_VERSION__);
const showCodeToast = computed(
  () => configStore.businessServer.indexOf("demo.cloudroom.com") === -1
);

const accountForm = reactive({
  account: "",
  pwd: "",
});

const phoneForm = reactive({
  phoneNumber: "",
  code: "",
});

const validate = {
  PhoneNumber(rule: any, value: string, callback: Function) {
    if (!value) {
      return callback(new Error("请输入手机号码"));
    }
    if (!/^(?:(?:\+|00)86)?1[3-9]\d{9}$/.test(value)) {
      return callback(new Error("手机号码格式错误"));
    }
    callback();
  },
  code(rule: any, value: string, callback: Function) {
    if (!value) {
      return callback(new Error(`请输入验证码`));
    }
    if (!/^\d{4,6}$/.test(value)) {
      return callback(new Error("验证码格式不正确"));
    }
    callback();
  },
  account(rule: any, value: string, callback: Function) {
    if (!value) {
      return callback(new Error("请输入账号"));
    }
    if (/\s/.test(value)) {
      return callback(new Error("账号不能包含空格"));
    }

    callback();
  },
};

const phoneRules = reactive<FormRules<typeof phoneForm>>({
  phoneNumber: [{ validator: validate.PhoneNumber, trigger: "blur" }],
  code: [{ validator: validate.code, trigger: "blur" }],
});

const accountRules = reactive<FormRules<typeof accountForm>>({
  account: [{ validator: validate.account, trigger: "blur" }],
  pwd: [{ required: true, message: "请输入密码", trigger: "blur" }],
});

let timerId: NodeJS.Timeout;

const getCodeBtn = () => {
  if (timeCount.value === -1 && phoneFormRef.value) {
    phoneFormRef.value.validateField("phoneNumber", (valid) => {
      if (valid) {
        loginApi.getCode({
          phoneNumber: phoneForm.phoneNumber,
          areaCode: "+86",
          usedFor: API.Login.UsedFor.Login,
        });
        timeCount.value = 59;
        timerId = setInterval(() => {
          timeCount.value--;
          if (timeCount.value === -1) {
            clearInterval(timerId);
          }
        }, 1000);
      }
    });
  }
};

const login = async () => {
  console.log(isPwd ? accountFormRef : phoneFormRef);
  (isPwd.value ? accountFormRef : phoneFormRef).value?.validate(
    async (valid) => {
      if (valid) {
        console.log({
          businessServer: configStore.businessServer,
        });
        isLoading.value = true;

        try {
          const obj = isPwd.value
            ? {
                account: accountForm.account,
                password: md5(accountForm.pwd),
              }
            : {
                verifyCode: phoneForm.code,
                phoneNumber: phoneForm.phoneNumber,
                areaCode: "+86",
              };
          console.log(obj);
          // 登录业务服务器
          const { sdkSvr, sdkAuthType, sdkToken, sdkAppId, sdkSecret } =
            await userStore.businessLogin(obj);

          try {
            const loginInfo =
              sdkAuthType === "0"
                ? {
                    sdkServer: sdkSvr,
                    appId: sdkAppId,
                    appSecret: md5(base64Parms(sdkSecret)),
                  }
                : {
                    sdkServer: sdkSvr,
                    token: sdkToken,
                  };

            //登录SDK
            await userStore.sdkLogin(loginInfo);

            router.replace({ name: "home" });
          } catch (err: any) {
            console.error(err);
            err.errMsg && ElMessage.error(err.errMsg);
          }
        } catch (err: any) {}

        isLoading.value = false;
      }
    }
  );
};
</script>

<style lang="scss" scoped>
.wrap {
  height: 100%;
  background-color: #f1f8fe;
  display: flex;
  flex-direction: column;
  .header {
    background-color: #fff;
    height: 66px;
    display: flex;
    align-items: center;
    .logo {
      width: 42px;
      margin-left: 20%;
      img {
        width: 100%;
      }
    }
  }
  .bg {
    flex: 1;
    display: flex;
    justify-content: center;
    align-items: center;
    position: relative;
    .box {
      width: 462px;
      // height: 590px;
      background-color: #fff;
      border-radius: 16px;

      .login {
        height: 100%;
        display: flex;
        flex-direction: column;
        padding: 56px 56px 30px;
        position: relative;
        .setting {
          position: absolute;
          right: 30px;
          top: 30px;
          cursor: pointer;
          font-size: 30px;
        }
        .logo {
          text-align: center;
          img {
            width: 120px;
            padding: 34px 10px 34px 20px;
            box-shadow: 0 0 8px 0px rgba(0, 0, 0, 0.1);
            border-radius: 10px;
          }
        }
        .big {
          font-size: 30px;
          font-weight: bold;
          margin-top: 40px;
        }
        .desc {
          font-size: 14px;
          margin-top: 10px;
          margin-bottom: 24px;
        }
        .formItem {
          margin-bottom: 20px;
          .input {
            height: 48px;
            flex: 1;
          }
          .timeCount {
            width: 70px;
            text-align: center;
            color: var(--el-color-primary);
          }
          :deep(.el-select__wrapper) {
            height: 100%;
          }
        }
        .togglePswd {
          text-align: center;
          font-size: 16px;
          line-height: 20px;
          margin-top: 26px;
          user-select: none;
        }
        .toast {
          font-size: 14px;
          margin-bottom: 10px;
          color: var(--el-text-color-placeholder);
        }
      }
      .btn {
        margin-top: 12px;
        height: 56px;
        font-size: 18px;
        width: 100%;
      }
    }
    .version {
      position: absolute;
      right: 10px;
      bottom: 10px;
      font-size: 14px;
      color: var(--el-text-color-placeholder);
    }
  }
}
</style>
