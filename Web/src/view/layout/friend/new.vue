<template>
  <div>
    <div class="wrap">
      <div class="header">
        <div class="title">新的好友</div>
        <el-select class="select" v-model="selectValue" placeholder="请选择">
          <el-option label="新的好友" :value="1" />
          <el-option label="我的请求" :value="2" />
        </el-select>
      </div>

      <div class="context" v-loading="loading">
        <el-scrollbar class="scrollbar">
          <ul>
            <li v-for="item in list" :key="item.userID" class="listItem">
              <div class="left">
                <Avatar :src="item.imgSrc"></Avatar>
                <span>{{ item.nickname }}</span>
              </div>
              <div class="right">
                <div v-if="item.handleResult === 1">已接受</div>
                <div v-else-if="item.handleResult === -1">已拒绝</div>
                <div v-else-if="selectValue === 1">
                  <el-button type="danger" @click="refuse(item)">
                    拒绝
                  </el-button>
                  <el-button type="primary" @click="accept(item)">
                    接受
                  </el-button>
                </div>
                <div v-else>未处理</div>
              </div>
            </li>
          </ul>
        </el-scrollbar>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { onMounted, ref, watch } from "vue";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { Avatar } from "~/components";
import { useFriendStore } from "~/stores";

const friendStore = useFriendStore();
const selectValue = ref(1);
const loading = ref(true);

interface Member {
  userID: string;
  nickname: string;
  imgSrc?: string;
  handleResult: IMTYPE.ApplicationHandleResult;
}
const list = ref<Member[]>([]);

// 拒绝别人的好友申请
const refuse = async (member: Member) => {
  await IMSDK.refuseFriendReq({
    toUserID: member.userID,
    handleMsg: "",
  });
  member.handleResult = -1;
};

// 同意别人的好友申请
const accept = async (member: Member) => {
  await IMSDK.acceptFriendReq({
    toUserID: member.userID,
    handleMsg: "",
  });
  member.handleResult = 1;
};

const refersh = async () => {
  loading.value = true;
  list.value = [];
  if (selectValue.value === 1) {
    list.value = (await IMSDK.getFriendReqListAsRecipient()).data.map(
      (item) => ({
        userID: item.fromUserID,
        nickname: item.fromNickname,
        imgSrc: item.fromFaceURL,
        handleResult: item.handleResult,
      })
    );
    friendStore.newFriendCount = 0;
  } else {
    console.log((await IMSDK.getFriendReqListAsApplicant()).data);
    list.value = (await IMSDK.getFriendReqListAsApplicant()).data.map(
      (item) => ({
        userID: item.toUserID,
        nickname: item.toNickname,
        imgSrc: item.toFaceURL,
        handleResult: item.handleResult,
      })
    );
  }

  loading.value = false;
};
watch(selectValue, refersh);
onMounted(async () => {
  refersh();
});
</script>

<style lang="scss" scoped>
.wrap {
  display: flex;
  flex-direction: column;
  height: 100%;
  overflow: hidden !important;
  .header {
    margin-left: 28px;
    margin-right: 10px;
    height: 100px;
    display: flex;
    align-items: center;
    .title {
      flex: 1;
      font-size: 18px;
    }
    .select {
      width: 120px;
      height: 30px;
    }
  }
  .context {
    flex: 1;
    overflow: hidden;
    ul {
      padding: 0 90px;
      .listItem {
        display: flex;
        padding: 12px 64px 12px 22px;
        align-items: center;
        border-bottom: 1px solid var(--el-border-color);
        height: 60px;
        .left {
          flex: 1;
          display: flex;
          align-items: center;
          span {
            margin-left: 18px;
          }
        }
      }
    }
  }
}
</style>
