declare module "*.svg" {
  const content: any;
  export default content;
}

declare module "*.vue" {
  import { ComponentOptions } from "vue";
  const componentOptions: ComponentOptions;
  export default componentOptions;
}

declare const __APP_VERSION__: string