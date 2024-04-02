import axios from "axios";
import { ElMessage } from "element-plus";
import { errorHandle } from "~/api/errorHandle";
import { useConfigStore, useUserStore } from "~/stores";

const getBaseUrl = function () {
  const configStore = useConfigStore();
  return `http://${configStore.businessServer}:8018`;
};
// import { getIMToken } from "./storage";

const request = axios.create({
  timeout: 5000,
});

request.interceptors.request.use(
  async (config) => {
    config.baseURL = getBaseUrl();
    config.headers.token = config.headers.token ?? useUserStore().getChatToken;
    return config;
  },
  (err) => Promise.reject(err)
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

    // debugger;
    return Promise.reject(err);
  }
);
export default request;
