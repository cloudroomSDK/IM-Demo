import { defineStore } from "pinia";
import { Component, markRaw } from "vue";
import { AppStore } from "./type";

export const useAppStore = defineStore("app", {
  state: (): AppStore => ({
    globalDialog: { show: false },
    sideWidth: 270,
  }),
  actions: {
    showDialog<T>({
      component,
      title,
      data,
      width,
      confirm,
    }: {
      component: Component;
      title?: string;
      width?: number;
      data?: any;
      confirm?: Function;
    }) {
      return new Promise<T>((resolve, reject) => {
        const cancel = () => {
          this.globalDialog.show = false;
          reject();
        };
        const success = (item: T) => {
          this.globalDialog.show = false;
          resolve(item);
        };
        this.globalDialog = {
          show: true,
          title,
          width,
          component: markRaw(component),
          data,
          confirm: success,
          cancel,
        };
      });
    },
  },
});
