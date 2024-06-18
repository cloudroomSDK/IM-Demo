// 该文件保存默认的服务器配置，若未修改设置或者重置设置将从此处读取

let businessServer: string, sdkServer: string, appId: string;

const isPublic =
  location.hostname === "sdk.cloudroom.com" ||
  location.hostname === "127.0.0.1" ||
  location.hostname === "localhost";

if (isPublic) {
  businessServer = "https://demo.cloudroom.com:8218"; //业务服务器
  sdkServer = "https://sdk.cloudroom.com"; //SDK服务器
  appId = "4026859001"; //appid
} else {
  businessServer = `${location.hostname}:8018`; //业务服务器
  sdkServer = `${location.hostname}:2727`; //SDK服务器
  appId = "demo"; //appid
}

export { businessServer, sdkServer, appId }; //业务服务器
export const appSecret = "123456"; //appSecret
export const useToken = false; //使用Token鉴权
export const token = ""; //token
