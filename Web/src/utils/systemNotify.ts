import { useConversationStore } from "~/stores";
import { toLastMessage } from ".";
import { IMSDK, MessageItem, SessionType, ConversationItem } from "./imsdk";

export class Notify {
  private static permission: NotificationPermission = "denied";
  private static curNotificationIsCall?: boolean;
  static init() {
    if (!window.Notification) {
      console.log("浏览器不支持notify");
      return;
    }

    if (Notification.permission === "granted") {
      this.permission = "granted";
    } else {
      Notification.requestPermission()
        .then((permission) => {
          console.log("Notification.permission:", permission);
          if (permission === "granted") {
            Notify.permission = permission;
          }
        })
        .catch((res) => {
          console.log("Notification.permission error:", res);
        });
    }
  }

  static async pushMsg(msg: MessageItem) {
    if (document.visibilityState === "visible") return;

    const { data: conversationItem } = await IMSDK.getOneConversation({
      sourceID: msg.groupID ? msg.groupID : msg.sendID,
      sessionType: msg.groupID ? SessionType.Group : SessionType.Single,
    });

    if (Notify.permission === "granted") {
      if (Notify.curNotificationIsCall) return;

      const firstBody = msg.groupID ? msg.senderNickname + ": " : "";

      const notification = new Notification(conversationItem.showName, {
        tag: "Notify",
        silent: true,
        body: firstBody + toLastMessage(msg),
        icon:
          conversationItem.faceURL ||
          import.meta.env.BASE_URL + "src/assets/logo.svg",
        // @ts-ignore
        renotify: true,
      });

      notification.onclick = (event) => {
        window.focus();
        const conversationStore = useConversationStore();
        conversationStore.gotoConversationChat({
          conversationItem: conversationItem,
        });
      };
    }
  }
  static async pushCall(body: string, conversationItem: ConversationItem) {
    if (document.visibilityState === "visible") return;

    if (Notify.permission === "granted") {
      Notify.curNotificationIsCall = true;
      const notification = new Notification(conversationItem.showName, {
        tag: "Notify",
        body: body,
        silent: true,
        icon:
          conversationItem.faceURL ||
          import.meta.env.BASE_URL + "src/assets/logo.svg",
        // @ts-ignore
        renotify: true,
      });

      let termID = setTimeout(() => {
        Notify.curNotificationIsCall = false;
      }, 8e3);

      notification.onclick = (event) => {
        clearTimeout(termID);
        Notify.curNotificationIsCall = false;
        window.focus();
      };
    }
  }
}
