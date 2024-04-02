# Web IM Demo

Web IM Demo是一个开源项目，项目展示了 IM Wasm SDK 接口的使用场景，方便客户了解 API 接口功能并快速接入到现有工程中。

[在线演示](http://sdk.cloudroom.com/web/im/)   [技术文档](https://docs.cloudroom.com/im/document/intro/introduce?platform=Web)

> 项目采用vue3 + ts + vite + pinia + vue-router + element-plus技术栈。

## 🚀 快速开始

### 克隆项目

```bash
git clone git@github.com:cloudroomSDK/IM-Demo.git
```

### 创建账号

1. 您需要在云屋[SDK官网](https://sdk.cloudroom.com/)注册账号并创建项目，具体流程参见[准备工作](https://docs.cloudroom.com/im/document/fastIntegration/build_example?platform=Web)
1. 项目创建完成，获取到`AppId`和`AppSecret`，选择以下之一方式：
    - 在`Web/src/config/index.ts`文件中修改对应的值
    - 启动项目后，在设置界面中填入对应的值

### 运行
```bash
cd Web
# 安装依赖
npm install

# 启动服务
npm run dev
```

浏览器访问 [http://127.0.0.1:5173/](http://127.0.0.1:5173/)

### 构建
```bash
npm run build
```

## 🔖 附录

- [开发者文档](https://docs.cloudroom.com/im/document/intro/introduce?platform=Web)
- [其他平台SDK下载](https://sdk.cloudroom.com/pages/download#sdk)
- [在线Demo](http://sdk.cloudroom.com/web/im/)
- [官网](https://sdk.cloudroom.com) 一 您可以在此联系官方技术支持

## 📄 License

MIT许可