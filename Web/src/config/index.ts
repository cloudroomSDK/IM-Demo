// 该文件保存默认的服务器配置，若未修改设置或者重置设置将从此处读取

let businessServer: string;

const isPublic =
  ["sdk.cloudroom.com", "127.0.0.1", "localhost"].indexOf(location.hostname) >
  -1;

if (isPublic) {
  businessServer = "https://demo.cloudroom.com:8218"; //业务服务器
} else {
  businessServer = `${location.hostname}:8018`; //业务服务器
}

export { businessServer }; //业务服务器

//高德地图js api 的key,如果没有key将使用iframe方式加载高德地图，体验效果不如js api
export const amapKey = "";
