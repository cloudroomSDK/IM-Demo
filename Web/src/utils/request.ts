import axios from "axios";
import { ElMessage } from "element-plus";
import { errorHandle } from "~/api/errorHandle";
import { useConfigStore, useUserStore } from "~/stores";

const request = axios.create({
  timeout: 5000,
});

export const updateBaseURL = () => {
  const configStore = useConfigStore();
  let baseUrl;
  if (configStore.businessServer === "默认服务器") {
    if (import.meta.env.VITE_DEFAULT_SERVER) {
      baseUrl = import.meta.env.VITE_DEFAULT_SERVER;
    } else {
      console.warn(
        "默认服务器未配置，将使用当前域名+默认端口作为业务服务器地址",
      );
      const isHttps = location.protocol === "https:";

      baseUrl = `${location.protocol}//${location.hostname}:${
        isHttps ? 8218 : 8018
      }`;
    }
  } else {
    baseUrl = configStore.businessServer;
  }

  if (!/^https?:\/\//.test(baseUrl)) {
    baseUrl = "http://" + baseUrl;
  }
  request.defaults.baseURL = baseUrl;
  return baseUrl;
};

// 首次请求时使用拦截器更改业务服务器，更新后销毁该拦截器
const myInterceptor = request.interceptors.request.use(function (config) {
  config.baseURL = updateBaseURL();
  request.interceptors.request.eject(myInterceptor);
  return config;
});

request.interceptors.request.use(
  async (config) => {
    config.headers.token = config.headers.token ?? useUserStore().getChatToken;
    return config;
  },
  (err) => Promise.reject(err),
);

request.interceptors.response.use(
  (res) => {
    if (res.data.errCode !== 0) {
      errorHandle(res.data);
      return Promise.reject(res.data);
    }
    return res.data.data;
  },
  (err) => {
    console.error("error", err);
    if (err.message.includes("timeout")) {
      ElMessage.error("网络超时");
    }
    if (err.message.includes("Invalid URL")) {
      ElMessage.error("业务服务器地址格式错误");
    }
    if (err.message.includes("Network Error")) {
      ElMessage.error("网络错误");
    }

    return Promise.reject(err);
  },
);
export default request;
