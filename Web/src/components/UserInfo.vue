<template>
  <div class="card">
    <UserHeader :img-src="imgSrc" :nickname="info.nickname" :userID="userID" />
    <el-divider />

    <div class="body">
      <el-form :model="form" label-width="70px">
        <el-form-item label="昵称" v-if="isMe">
          <el-input v-model="form.nickname" :disabled="!isEdit" />
        </el-form-item>
        <el-form-item label="生日">
          <el-date-picker
            v-model="form.birth"
            type="date"
            :placeholder="isEdit ? '选择生日' : ''"
            :disabled="!isEdit"
            format="YYYY/MM/DD"
            value-format="x"
            :clearable="false"
          />
        </el-form-item>
        <el-form-item label="手机号码">
          <el-input v-model="form.phoneNumber" :disabled="!isEdit" />
        </el-form-item>
      </el-form>
      <template v-if="inGroupInfo">
        <el-descriptions :column="1">
          <el-descriptions-item label="身份">
            {{
              inGroupInfo.roleLevel === 20
                ? "普通用户"
                : inGroupInfo.roleLevel === 60
                ? "管理员"
                : "群主"
            }}
          </el-descriptions-item>
          <el-descriptions-item label="入群时间">
            {{ formatString(inGroupInfo.joinTime) }}
          </el-descriptions-item>
        </el-descriptions>
      </template>
    </div>
    <el-divider />
    <div class="footer">
      <template v-if="isMe">
        <el-button
          v-if="!isEdit"
          class="button"
          type="primary"
          @click="isEdit = true"
        >
          编辑资料
        </el-button>
        <el-button
          v-if="isEdit"
          class="button"
          type="primary"
          @click="editInfo"
        >
          保存
        </el-button>
        <el-button v-if="isEdit" class="button" @click="cancel">
          取消
        </el-button>
      </template>
      <el-button
        v-else-if="isFriend"
        class="button"
        type="primary"
        @click="goChat"
      >
        聊天
      </el-button>
      <el-button v-else class="button" type="primary" @click="add()">
        添加好友
      </el-button>
    </div>
  </div>
</template>
<script lang="ts" setup>
import { UserHeader, AddFriend } from "~/components";
import { ElMessageBox } from "element-plus";
import { computed, h, onMounted, Ref, ref } from "vue";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { useUserStore, useConversationStore } from "~/stores";
import { getBusinessUserInfo, updateBusinessUserInfo } from "~/api/login";
import { formatString } from "~/utils/dayjs";

const userStore = useUserStore();
const conversationStore = useConversationStore();

const props = defineProps<{
  userID: string;
  groupID?: string;
  isApiOpen: boolean;
}>();

const isEdit = ref(false);
const isFriend = ref(false);
const isMe = ref(false);
let imgSrc = ref("");

const form = ref({
  nickname: "",
  phoneNumber: "",
  birth: 0,
});
const info = ref({
  nickname: "",
  phoneNumber: "",
  birth: 0,
});

let inGroupInfo: Ref<IMTYPE.GroupMemberItem>;
onMounted(async () => {
  if (props.groupID) {
    const { data } = await IMSDK.getSpecifiedGrpMembersInfo({
      groupID: props.groupID,
      userIDList: [props.userID],
    });
    inGroupInfo = ref(data[0]);
  }

  // 如果打开自己的窗口
  if (userStore.businessData?.userID === props.userID) {
    isMe.value = true;
    const userInfo = (await getBusinessUserInfo([props.userID])).users[0];

    info.value.phoneNumber = userInfo.phoneNumber;
    info.value.birth = userInfo.birth;
    info.value.nickname = userStore.userInfo!.nickname;
    imgSrc = computed(() => userStore.userInfo!.faceURL);
    resetInfo();
    return;
  }

  //检查好友关系
  const friendshipInfoArr = (await IMSDK.checkFriend([props.userID])).data;
  if (friendshipInfoArr[0].userID === props.userID) {
    isFriend.value = !!friendshipInfoArr[0].result;
    //如果是好友
    if (isFriend.value) {
      const userInfo = (await getBusinessUserInfo([props.userID])).users[0];

      info.value.birth = userInfo.birth ?? 0;
      info.value.nickname = userInfo.nickname;
      info.value.phoneNumber = userInfo.phoneNumber;
      imgSrc.value = userInfo.faceURL;
    } else {
      //如果不是好友
      const userInfo = (await IMSDK.getUsersInfo([props.userID])).data[0];
      imgSrc.value = userInfo.publicInfo!.faceURL;
      info.value.nickname = userInfo.publicInfo!.nickname;
    }

    resetInfo();
  }
});
const add = () => {
  let str = "";
  ElMessageBox({
    title: "好友验证",
    message: h(AddFriend, {
      nickname: info.value.nickname,
      userID: props.userID,
      imgSrc: imgSrc.value,
      modelVale: str,
      onUpdateValue(newStr: string) {
        str = newStr;
      },
    }),
    showClose: true,
    confirmButtonText: "发送消息",
    beforeClose: async (action, instance, done) => {
      if (action === "confirm") {
        instance.confirmButtonLoading = true;
        console.log(str);
        instance.confirmButtonText = "Loading...";
        try {
          await IMSDK.addFriend({
            toUserID: props.userID,
            reqMsg: str,
          });
        } catch (error) {}
        setTimeout(() => {
          instance.confirmButtonLoading = false;
        }, 300);
      }
      done();
    },
  }).then((action) => {
    console.log(action);
  });
};

const goChat = () => {
  conversationStore.gotoConversationChat({ userID: props.userID });
  props.isApiOpen && ElMessageBox.close();
};

const resetInfo = () => {
  form.value.nickname = info.value.nickname;
  form.value.phoneNumber = info.value.phoneNumber;
  form.value.birth = info.value.birth;
};

const cancel = () => {
  resetInfo();
  isEdit.value = false;
};

const editInfo = async () => {
  console.log(form.value);
  const obj = Object.keys(form.value).reduce((previousValue, currentValue) => {
    // @ts-ignore
    if (form.value[currentValue] !== info.value[currentValue]) {
      // @ts-ignore
      previousValue[currentValue] = form.value[currentValue];
    }
    return previousValue;
  }, {});

  await updateBusinessUserInfo({
    birth: form.value.birth,
    nickname: form.value.nickname,
  });
  
  isEdit.value = false;
};
</script>
<style lang="scss" scoped>
.card {
  .body {
    padding: 10px;
    :deep(.el-input__prefix) {
      display: none;
    }
    :deep(.el-input.is-disabled) {
      --el-disabled-bg-color: none;
      .el-input__wrapper {
        box-shadow: none;
        .el-input__inner {
          color: #333;
          -webkit-text-fill-color: #333;
          cursor: default;
        }
      }
    }
    :deep(.el-descriptions__table tr) {
      &:last-of-type .el-descriptions__cell {
        padding-bottom: 0;
      }
      .el-descriptions__cell {
        line-height: 32px;
        padding-bottom: 18px;
      }
      .el-descriptions__label {
        width: 58px;
        margin-right: 24px;
        color: #606266;
        display: inline-block;
        text-align: right;
      }
      .el-descriptions__content {
        color: #333;
      }
    }
  }
  .footer {
    text-align: center;
    display: flex;
    .button {
      flex: 1;
      padding: 0;
    }
  }
}
</style>
