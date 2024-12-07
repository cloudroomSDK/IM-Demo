<template>
  <Layout>
    <template #search>
      <Search v-model="searchText" />
    </template>
    <template #list>
      <li v-show="!searchText.trim()" @click="openCreateGroup">
        <Card name="创建群组" :imgSrc="createGroupImg" />
      </li>
      <li
        v-show="!searchText.trim()"
        @click="$router.push({ name: 'newGroups' })"
      >
        <Card
          name="入群申请"
          :imgSrc="msgGroupImg"
          :active="'_newGroups' === activeName"
          :msgCount="groupStore.messageCount"
        />
      </li>
      <li v-show="!searchText.trim()" @click="openJoinGroup">
        <Card name="加入群聊" :imgSrc="joinGroupImg" />
      </li>
      <li v-for="item in list" :key="item.id" @click="goGroupInfo(item)">
        <Card
          :active="item.id === activeName"
          :name="item.name"
          :imgSrc="item.imgSrc"
          avatarType="group"
        />
      </li>
    </template>
  </Layout>
</template>

<script setup lang="ts">
import { Search, Card, MemberSelect, Layout, AddGroup } from "~/components";

import { useRouter, useRoute } from "vue-router";
import { computed, ref } from "vue";
import { useGroupStore, useAppStore } from "~/stores";
import { IMSDK } from "~/utils/imsdk";
import createGroupImg from "~/assets/icons/create_group.svg";
import msgGroupImg from "~/assets/icons/msg_group.svg";
import joinGroupImg from "~/assets/icons/join_group.svg";

const $router = useRouter();
const $route = useRoute();
const appStore = useAppStore();
const groupStore = useGroupStore();
const searchText = ref("");
groupStore.getList();

interface listItem {
  id: string;
  name: string;
  imgSrc?: string;
}

const activeName = computed(() => {
  if ($route.name === "groupInfo") {
    return $route.params.groupID;
  }
  if ($route.name === "newGroups") {
    return "_newGroups";
  }
  return "";
});

const goGroupInfo = function (groupInfo: listItem) {
  $router.push({
    name: "groupInfo",
    params: { groupID: groupInfo.id },
  });
};

const list = computed(() => {
  if (!groupStore.list) return [];

  return groupStore.list
    .filter((item) => {
      if (searchText.value.trim()) {
        return (
          item.groupName
            .toLocaleLowerCase()
            .indexOf(searchText.value.toLocaleLowerCase()) > -1 ||
          item.groupID
            .toLocaleLowerCase()
            .indexOf(searchText.value.toLocaleLowerCase()) > -1
        );
      }
      return true;
    })
    .map((item) => ({
      id: item.groupID,
      name: item.groupName,
      imgSrc: item.faceURL,
    }));
});

type CreateGroup = {
  groupName: string;
  faceUrl?: string;
  selectList: {
    userID: string;
  }[];
};

const openCreateGroup = async () => {
  const data = await appStore.showDialog<CreateGroup>({
    component: MemberSelect,
    title: "创建群聊",
    data: {
      type: "createGroup",
    },
    width: 700,
  });
  console.log(data);
  const { data: groupItem } = await IMSDK.createGrp({
    groupInfo: {
      groupType: 3,
      groupName: data.groupName,
      faceURL: data.faceUrl,
    },
    memberUserIDs: data.selectList.map((item) => item.userID),
    adminUserIDs: [],
  });

  $router.push({
    name: "groupInfo",
    params: { groupID: groupItem.groupID },
  });
};
const openJoinGroup = () => {
  appStore.showDialog({
    component: AddGroup,
    title: "加入群聊",
    width: 700,
  });
};
</script>
