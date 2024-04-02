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
          <div class="logo">
            <img :src="Logo" alt="" />
          </div>
          <p class="big">验证码登录</p>
          <p class="desc">未注册的手机号通过验证后自动注册</p>

          <div class="input">
            <el-input v-model="phone" placeholder="请输入手机号" class="input">
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
          </div>
          <div class="input" style="margin-top: 10px">
            <el-input v-model="vCode" placeholder="请输入验证码" class="input">
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
          </div>
          <el-button
            class="btn"
            type="primary"
            @click="login"
            :loading="isLoading"
            :disabled="isLoading"
          >
            登录/注册
          </el-button>
          <span class="loginSettingBtn" @click="isSetting = true">
            登录设置
          </span>
        </div>
        <div v-else class="setting">
          <div class="title">
            <el-icon @click="isSetting = false" style="cursor: pointer"
              ><i-ep-arrowLeft /></el-icon
            ><span>登录设置</span>
          </div>
          <div class="input-group">
            <div class="input-item">
              <span class="left">SDK服务器：</span>
              <input type="text" v-model="form.sdkServer" />
            </div>
            <div class="input-item">
              <span class="left">业务服务器：</span>
              <input type="text" v-model="form.businessServer" />
            </div>
            <div class="input-item">
              <span class="left">鉴权方式：</span>
              <el-select v-model="form.useToken" style="width: 240px">
                <el-option label="账号密码鉴权" :value="false" />
                <el-option label="动态Token鉴权" :value="true" />
              </el-select>
            </div>
            <template v-if="!form.useToken">
              <div class="input-item">
                <span class="left">AppID：</span>
                <input type="text" v-model="form.appId" />
              </div>
              <div class="input-item">
                <span class="left">AppSecret：</span>
                <input type="password" v-model="form.appSecret" />
              </div>
            </template>
            <div v-else class="input-item">
              <span class="left">Token：</span>
              <input type="text" v-model="form.token" />
            </div>
            <div>
              <el-button class="btn" type="primary" @click="sava">
                保存
              </el-button>
            </div>
            <div>
              <el-button class="btn" type="danger" plain @click="reset">
                重置为默认
              </el-button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { useConfigStore, useUserStore } from "~/stores";
import Logo from "~/assets/logo.svg";
import { useRouter } from "vue-router";
import { ref } from "vue";
import { ElMessage } from "element-plus";
import loginApi from "~/api/login";
import { API } from "~/api/typings";
import { base64Parms, md5 } from "~/utils/index";

import {
  businessServer as defaultBusinessServer,
  sdkServer as defaultSdkServer,
  appId as defaultAppId,
  appSecret as defaultAppSecret,
  useToken as defaultUseToken,
  token as defaultToken,
} from "~/config"; //获取默认值

const configStore = useConfigStore();
const router = useRouter();
const userStore = useUserStore();

const phone = ref("");
const vCode = ref(""); //验证码
const select = ref("1");
const timeCount = ref(-1);
const isSetting = ref(false);
const isLoading = ref(false);

const form = ref({
  businessServer: configStore.businessServer,
  sdkServer: configStore.sdkServer,
  appId: configStore.appId,
  appSecret: configStore.appSecret,
  token: configStore.token,
  useToken: configStore.useToken,
});

let timerId: NodeJS.Timeout;

const checkPhone = (): boolean => /^1[0-9]{10}$/.test(phone.value.trim());

const getAppID = () => {
  if (configStore.useToken) {
    const t = configStore.token.split(".")[1];
    const json = base64Parms(t);
    const data = JSON.parse(json);
    return data.appID;
  } else {
    return configStore.appId;
  }
};

const getCodeBtn = () => {
  if (timeCount.value === -1) {
    if (!checkPhone()) {
      return ElMessage.error("手机号码格式错误");
    }
    loginApi.getCode({
      phoneNumber: phone.value,
      areaCode: "+86",
      usedFor: API.Login.UsedFor.Login,
      appID: getAppID(),
    });
    timeCount.value = 59;
    timerId = setInterval(() => {
      timeCount.value--;
      if (timeCount.value === -1) {
        clearInterval(timerId);
      }
    }, 1000);
  }
};

const login = async function () {
  if (!checkPhone()) {
    return ElMessage.error("手机号码格式错误");
  }
  if (!vCode.value.trim()) {
    return ElMessage.error("请输入验证码");
  }
  console.log({
    businessServer: configStore.businessServer,
    sdkServer: configStore.sdkServer,
    appId: configStore.appId,
    appSecret: configStore.appSecret,
    token: configStore.token,
    useToken: configStore.useToken,
  });
  isLoading.value = true;

  try {
    // 登录业务服务器
    await userStore.businessLogin({
      verifyCode: vCode.value,
      phoneNumber: phone.value,
      areaCode: "+86",
      appID: getAppID(),
      password: "",
    });

    try {
      //登录SDK
      await userStore.sdkLogin(
        Object.assign(
          {
            sdkServer: "http://" + configStore.sdkServer,
          },
          configStore.useToken
            ? {
                token: configStore.token,
              }
            : {
                appId: getAppID(),
                appSecret: md5(configStore.appSecret),
              }
        )
      );

      router.replace({ name: "home" });
    } catch (err: any) {
      console.error(err);
      err.errMsg && ElMessage.error(err.errMsg);
    }
  } catch (err: any) {}

  isLoading.value = false;
};
const sava = () => {
  configStore.businessServer = form.value.businessServer;
  configStore.sdkServer = form.value.sdkServer;
  configStore.useToken = form.value.useToken;
  if (form.value.useToken) {
    configStore.token = form.value.token;
  } else {
    configStore.appId = form.value.appId;
    configStore.appSecret = form.value.appSecret;
  }
  isSetting.value = false;
};
const reset = () => {
  form.value.businessServer = defaultBusinessServer;
  form.value.sdkServer = defaultSdkServer;
  form.value.useToken = defaultUseToken;
  form.value.token = defaultToken;
  form.value.appId = defaultAppId;
  form.value.appSecret = defaultAppSecret;
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
        .input {
          height: 48px;
          margin-bottom: 17px;
          .timeCount {
            width: 70px;
            text-align: center;
            color: var(--el-color-primary);
          }
          :deep(.el-select__wrapper) {
            height: 100%;
          }
        }
        .loginSettingBtn {
          cursor: pointer;
          text-align: center;
          font-size: 16px;
          margin-top: 26px;
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
      }
      .btn {
        margin-top: 12px;
        height: 56px;
        font-size: 18px;
        width: 100%;
      }
    }
  }
}
</style>
