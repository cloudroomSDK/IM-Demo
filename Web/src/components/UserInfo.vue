<template>
  <div class="card">
    <UserHeader
      v-if="isMe"
      :img-src="userStore.userInfo?.faceURL"
      :nickname="userStore.userInfo?.nickname!"
      :userID="userStore.userInfo?.userID!"
    />
    <UserHeader
      v-else
      :img-src="imgSrc"
      :nickname="info.nickname"
      :userID="userID"
    />
    <el-divider />

    <div class="body">
      <el-form :model="form" label-width="70px">
        <el-form-item label="昵称" v-if="isMe">
          <el-input
            v-model="form.nickname"
            :disabled="!isEdit"
            maxlength="64"
          />
        </el-form-item>
        <el-form-item label="生日">
          <el-date-picker
            v-model="form.birth"
            type="date"
            :placeholder="isEdit ? '选择生日' : '-'"
            :disabled="!isEdit"
            format="YYYY/MM/DD"
            value-format="x"
            :clearable="false"
          />
        </el-form-item>
        <el-form-item label="手机号码">
          <el-input v-model="form.phoneNumber" disabled />
        </el-form-item>
        <el-form-item label="性别">
          <el-select
            class="select"
            :class="{ disabled: !isEdit }"
            v-model="form.gender"
            placeholder="-"
            :disabled="!isEdit"
          >
            <el-option label="-" :value="0" style="display: none" />
            <el-option label="男" :value="1" />
            <el-option label="女" :value="2" />
          </el-select>
        </el-form-item>
        <el-form-item label="邮箱">
          <el-input v-model="form.email" :disabled="!isEdit" />
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
import { ElMessage, ElMessageBox } from "element-plus";
import { computed, h, onMounted, Ref, ref } from "vue";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { useUserStore, useConversationStore } from "~/stores";
import { getBusinessUserInfo, updateBusinessUserInfo } from "~/api/login";
import { formatString } from "~/utils/dayjs";

const userStore = useUserStore();
const conversationStore = useConversationStore();

interface Props {
  userID: string;
  groupID?: string;
  allowLookInfo?: boolean;
  allowAddFriend?: boolean;
}
const props = withDefaults(defineProps<Props>(), {
  allowLookInfo: true,
  allowAddFriend: true,
});

const isEdit = ref(false);
const isFriend = ref(false);
const isMe = ref(false);
let imgSrc = ref("");

const $emit = defineEmits(["close"]);

const form = ref({
  nickname: "-",
  phoneNumber: "-",
  birth: 0,
  gender: 0,
  email: "-",
});

const info = ref({
  nickname: "-",
  phoneNumber: "-",
  birth: 0,
  gender: 0,
  email: "-",
});

let inGroupInfo: Ref<IMTYPE.GroupMemberItem>;
onMounted(async () => {
  // 如果打开自己的窗口
  if (userStore.getMyUserID === props.userID) {
    isMe.value = true;
    const userInfo = (await getBusinessUserInfo([props.userID])).users[0];

    info.value.phoneNumber = userInfo.phoneNumber || "-";
    info.value.birth = userInfo.birth || 0;
    info.value.nickname = userStore.userInfo!.nickname || "-";
    info.value.gender = userInfo!.gender || 0;
    info.value.email = userInfo!.email || "-";
    imgSrc = computed(() => userStore.userInfo!.faceURL);
    resetInfo();
  } else {
    if (!props.allowLookInfo) {
      ElMessage.error("管理员禁止查看成员资料");
      return;
    }
    //检查好友关系
    const { data: friendshipInfoArr } = await IMSDK.checkFriend([props.userID]);
    if (friendshipInfoArr[0].userID === props.userID) {
      isFriend.value = !!friendshipInfoArr[0].result;
      //如果是好友
      if (isFriend.value) {
        const userInfo = (await getBusinessUserInfo([props.userID])).users[0];

        info.value.birth = userInfo.birth || 0;
        info.value.nickname = userInfo.nickname || "-";
        info.value.phoneNumber = userInfo.phoneNumber || "-";
        info.value.gender = userInfo.gender || 0;
        info.value.email = userInfo.email || "-";
        imgSrc.value = userInfo.faceURL;
      } else {
        //如果不是好友
        const userInfo = (await IMSDK.getUsersInfo([props.userID])).data[0];
        imgSrc.value = userInfo.publicInfo!.faceURL;
        info.value.nickname = userInfo.publicInfo!.nickname;
      }

      resetInfo();
    }
  }

  if (props.groupID) {
    const { data } = await IMSDK.getSpecifiedGrpMembersInfo({
      groupID: props.groupID,
      userIDList: [props.userID],
    });
    inGroupInfo = ref(data[0]);
  }
});
const add = () => {
  if (!props.allowAddFriend) {
    ElMessage.error("管理员禁止添加好友");
    return;
  }
  let str = "";
  ElMessageBox({
    title: "好友验证",
    customClass: "el-message-add_friend",
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
  $emit("close");
  conversationStore.gotoConversationChat({ userID: props.userID });
};

const resetInfo = () => {
  form.value.nickname = info.value.nickname;
  form.value.birth = info.value.birth;
  form.value.phoneNumber = info.value.phoneNumber;
  form.value.gender = info.value.gender;
  form.value.email = info.value.email;
};

const cancel = () => {
  resetInfo();
  isEdit.value = false;
};

const editInfo = async () => {
  const obj = Object.keys(form.value).reduce((previousValue, currentValue) => {
    // @ts-ignore
    if (form.value[currentValue] !== info.value[currentValue]) {
      // @ts-ignore
      previousValue[currentValue] = form.value[currentValue];
    }
    return previousValue;
  }, {});
  console.log(obj);
  await updateBusinessUserInfo(obj);
  info.value.nickname = form.value.nickname;
  info.value.birth = form.value.birth;
  info.value.email = form.value.email;
  info.value.gender = form.value.gender;

  isEdit.value = false;
};
</script>
<style lang="scss" scoped>
.card {
  .body {
    padding: 10px;
    .select {
      --el-select-width: 100px;
      --el-select-disabled-border: transparent;
      --el-select-disabled-color: #333;
      --el-fill-color-light: transparent;
      &.disabled {
        :deep(.el-select__wrapper.is-disabled) {
          cursor: auto;
        }
        :deep(.el-select__suffix) {
          display: none;
        }
      }
    }
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
