<template>
  <Layout>
    <template #search>
      <Search v-model="searchText" />
    </template>
    <template #list>
      <li
        v-show="!searchText.trim()"
        @click="$router.push({ name: 'addFriend' })"
      >
        <Card
          :active="'_addFriend' === activeName"
          name="添加好友"
          :imgSrc="friendInviteImg"
        />
      </li>
      <li
        v-show="!searchText.trim()"
        @click="$router.push({ name: 'newFriend' })"
      >
        <Card
          :active="'_newFriend' === activeName"
          name="新的好友"
          :msgCount="friendStore.newFriendCount"
          :imgSrc="defaultUserImg"
        />
      </li>
      <li v-for="item in list" :key="item.id" @click="goFriendInfo(item)">
        <Card
          :active="item.id === activeName"
          :name="item.name"
          :imgSrc="item.imgSrc"
        />
      </li>
    </template>
  </Layout>
</template>

<script setup lang="ts">
import { Search, Card, Layout } from "~/components";

import { useRouter, useRoute } from "vue-router";
import { computed, ref } from "vue";
import { useFriendStore } from "~/stores";
import friendInviteImg from "~/assets/icons/friend_invite.svg";
import defaultUserImg from "~/assets/icons/default_user.svg";

const $router = useRouter();
const $route = useRoute();
const friendStore = useFriendStore();
const searchText = ref("");
friendStore.getList();

interface listItem {
  id: string;
  name: string;
  imgSrc?: string;
}

const activeName = computed(() => {
  if ($route.name === "newFriend") {
    return "_newFriend";
  }
  if ($route.name === "addFriend") {
    return "_addFriend";
  }
  if ($route.name === "friendInfo") {
    return $route.params.userID;
  }
  return "";
});

const goFriendInfo = function (friendUserItem: listItem) {
  $router.push({
    name: "friendInfo",
    params: { userID: friendUserItem.id },
  });
};

const list = computed(() => {
  if (!friendStore.list) return [];

  return friendStore.list
    .filter((item) => {
      if (searchText.value.trim()) {
        return (
          item.nickname
            .toLocaleLowerCase()
            .indexOf(searchText.value.toLocaleLowerCase()) > -1 ||
          item.remark
            .toLocaleLowerCase()
            .indexOf(searchText.value.toLocaleLowerCase()) > -1 ||
          item.userID
            .toLocaleLowerCase()
            .indexOf(searchText.value.toLocaleLowerCase()) > -1
        );
      }
      return true;
    })
    .map((item) => ({
      id: item.userID,
      name: item.remark || item.nickname,
      imgSrc: item.faceURL,
    }));
});
</script>
