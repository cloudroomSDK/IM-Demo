<template>
  <div class="sendBusinessCard">
    <div class="pd">
      <Search v-model="searchText" />
    </div>
    <p class="pd">我的好友</p>
    <el-scrollbar height="400px">
      <div class="friend" v-for="item in filterList">
        <div class="friendItem">
          <div class="l">
            <Avatar :size="30" class="avatar" :src="item.faceURL" />
            <span class="nickname">{{ item.nickname }}</span>
          </div>
          <el-button
            class="r"
            type="primary"
            plain
            @click="$emit('send', item)"
          >
            发送
          </el-button>
        </div>
      </div>
    </el-scrollbar>
  </div>
</template>
<script lang="ts" setup>
import { computed, onBeforeMount, ref } from "vue";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { Avatar, Search } from "~/components";

const searchText = ref("");
const list = ref<IMTYPE.FriendUserItem[]>([]);

const filterList = computed(() => {
  if (searchText.value === "") return list.value;
  const str = searchText.value.toLocaleLowerCase();
  return list.value.filter(
    (item) =>
      item.userID.toLocaleLowerCase().indexOf(str) > -1 ||
      item.nickname.toLocaleLowerCase().indexOf(str) > -1
  );
});
onBeforeMount(async () => {
  const friendList = (await IMSDK.getFriendList()).data;
  friendList;
  list.value = friendList
    .map((item) => item.friendInfo)
    .filter(Boolean) as IMTYPE.FriendUserItem[];
});
</script>
<style lang="scss" scoped>
.pd {
  padding: 0 22px;
}
.sendBusinessCard {
  p {
    font-size: 14px;
    margin: 20px 0;
  }
  .friend {
    padding-left: 12px;
    padding-right: 14px;

    .friendItem {
      display: flex;
      align-items: center;
      margin-bottom: 10px;
      padding: 4px 8px;
      border-radius: 4px;

      &:hover {
        background-color: #e7e9f2;
      }
      .l {
        flex: 1;
        display: flex;
        align-items: center;
        .avatar {
          width: 30px;
          margin-right: 10px;
        }
        .nickname {
          text-wrap: nowrap;
          flex: 1;
          width: 0;
          overflow: hidden;
          text-overflow: ellipsis;
          margin-right: 10px;
          line-height: 20px;
        }
      }
      .r {
        width: 50px;
        height: 30px;
        padding: 0;
      }
    }
  }
}
</style>
