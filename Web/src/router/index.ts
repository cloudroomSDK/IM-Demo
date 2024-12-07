import { createRouter, createWebHistory } from "vue-router";
// 1. 定义路由组件.
// 也可以从其他文件导入
import Layout from "~/view/layout/index.vue";
import Login from "~/view/login.vue";
import { useUserStore } from "~/stores";

import Chat from "~/view/layout/conversation/index.vue";
import Conversation from "~/view/layout/conversation/chat/index.vue";
import Friend from "~/view/layout/friend/index.vue";
import NewFriends from "~/view/layout/friend/new.vue";
import AddFriends from "~/view/layout/friend/add.vue";
import FriendInfo from "~/view/layout/friend/info.vue";
import { Empty } from "~/components";
import Group from "~/view/layout/group/index.vue";
import NewGroups from "~/view/layout/group/new.vue";
import GroupInfo from "~/view/layout/group/info.vue";

// 2. 定义一些路由
// 每个路由都需要映射到一个组件。
// 我们后面再讨论嵌套路由。
const routes = [
  { path: "/login", name: "login", component: Login },
  {
    path: "/",
    name: "home",
    component: Layout,
    redirect: "/chat",
    children: [
      {
        path: "chat",
        component: Chat,
        children: [
          {
            path: "",
            name: "chat",
            component: Empty,
          },
          {
            path: ":conversationID",
            name: "conversation",
            component: Conversation,
          },
        ],
      },
      {
        path: "friend",
        component: Friend,
        children: [
          {
            path: "",
            name: "friend",
            component: Empty,
          },
          {
            path: "add",
            name: "addFriend",
            component: AddFriends,
          },
          {
            path: "new",
            name: "newFriend",
            component: NewFriends,
          },
          {
            path: "info/:userID",
            name: "friendInfo",
            component: FriendInfo,
          },
        ],
      },
      {
        path: "group",
        component: Group,
        children: [
          {
            path: "",
            name: "group",
            component: Empty,
          },
          {
            path: "new",
            name: "newGroups",
            component: NewGroups,
          },
          {
            path: ":groupID",
            name: "groupInfo",
            component: GroupInfo,
          },
        ],
      },
      { path: "/:pathMatch(.*)*", redirect: "/chat" },
    ],
  },
];

// 3. 创建路由实例并传递 `routes` 配置
// 你可以在这里输入更多的配置，但我们在这里
// 暂时保持简单
const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes,
});

const whileList: Set<any> = new Set(["login"]);

router.beforeEach(async (to, from, next) => {
  const userStore = useUserStore();
  const isWhile = whileList.has(to.name); //当前是白名单页面

  if (!userStore.userInfo && userStore.businessData) {
    try {
      const { sdkSvr, sdkToken } = await userStore.businessLoginByToken();
      await userStore.sdkLogin({
        sdkServer: sdkSvr,
        token: sdkToken,
      });
    } catch (error) {
      userStore.businessData = undefined;
    }
  }

  if (userStore.userInfo) {
    isWhile ? next({ name: "home" }) : next();
  } else {
    isWhile ? next() : next({ name: "login" });
  }
});

export default router;
