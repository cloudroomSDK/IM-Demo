<template>
  <div>
    <div class="container">
      <template v-if="data.type === 'createGroup'">
        <div class="labelItem aic">
          <div class="labelName">群名称</div>
          <div class="context">
            <el-input
              v-model="groupName"
              placeholder="请输入群名称"
              :maxlength="64"
              show-word-limit
            />
          </div>
        </div>
        <div class="labelItem aic">
          <div class="labelName">群头像</div>
          <div class="context">
            <CanEditAvatar
              type="group"
              :src="imageUrl"
              :disabled="false"
              :size="40"
              @updateUrl="updateUrl"
            />
          </div>
        </div>
      </template>
      <div class="labelItem select">
        <div class="labelName" v-if="data.type !== 'groupMember'">
          <p v-if="data.type === 'createGroup'">邀请</p>
          <template v-else>
            <p
              v-if="canSelectConversation"
              :class="[{ active: selectValue === 1 }, 'cupo']"
              @click="selectValue = 1"
            >
              最近聊天
            </p>
            <p
              v-if="canSelectFriend"
              :class="[{ active: selectValue === 2 }, 'cupo']"
              @click="selectValue = 2"
            >
              我的好友
            </p>
            <p
              v-if="canSelectGroup"
              :class="[{ active: selectValue === 3 }, 'cupo']"
              @click="selectValue = 3"
            >
              我的群组
            </p>
          </template>
        </div>
        <div class="bd">
          <div class="l">
            <el-checkbox-group v-model="checkList">
              <div
                v-if="data.type === 'groupMember'"
                class="infinite-list"
                style="overflow: auto"
              >
                <div v-for="item in allList" :key="item.userID">
                  <el-checkbox
                    :value="'f_' + item.userID"
                    :disabled="item?.disabled || multipleDisabled"
                  >
                    <div class="item">
                      <Avatar :size="30" :src="item.faceURL" />
                      <span class="name">{{ item.showName }}</span>
                    </div>
                  </el-checkbox>
                </div>
              </div>
              <el-scrollbar height="390px" v-else>
                <div
                  v-show="selectValue === 1"
                  v-for="item in conversationStore.conversationList"
                  :key="item.userID"
                >
                  <el-checkbox
                    :value="
                      item.groupID ? 'g_' + item.groupID : 'u_' + item.userID
                    "
                    :disabled="multipleDisabled"
                  >
                    <div class="item">
                      <Avatar
                        :size="30"
                        :src="item.faceURL"
                        :type="item.groupID ? 'group' : 'user'"
                      />
                      <span class="name">{{ item.showName }}</span>
                    </div>
                  </el-checkbox>
                </div>
                <div
                  v-show="selectValue === 2"
                  v-for="item in (data.selectList || friendStore.list) as SelectItem[]"
                  :key="item.userID"
                >
                  <el-checkbox
                    :value="'u_' + item.userID"
                    :disabled="item?.disabled || multipleDisabled"
                  >
                    <div class="item">
                      <Avatar :size="30" :src="item.faceURL" />
                      <span class="name">
                        {{ item.remark || item.nickname }}
                      </span>
                    </div>
                  </el-checkbox>
                </div>
                <div
                  v-show="selectValue === 3"
                  v-for="item in groupStore.list"
                  :key="item.groupID"
                >
                  <el-checkbox
                    :value="'g_' + item.groupID"
                    :disabled="multipleDisabled"
                  >
                    <div class="item">
                      <Avatar :size="30" :src="item.faceURL" type="group" />
                      <span class="name">
                        {{ item.groupName }}
                      </span>
                    </div>
                  </el-checkbox>
                </div>
              </el-scrollbar>
            </el-checkbox-group>
          </div>
          <div class="r">
            <Empty v-show="!selectList.length" />
            <div v-show="selectList.length">
              <p class="p">已选{{ selectList.length }}项</p>
              <el-scrollbar height="362px">
                <div
                  v-for="(item, index) in selectList"
                  :key="item!.key"
                  class="selectItem"
                >
                  <Avatar :size="30" :src="item?.imgSrc" />
                  <span class="name">{{ item?.showName }}</span>
                  <el-icon class="icon" @click="checkList.splice(index, 1)">
                    <i-ep-close />
                  </el-icon>
                </div>
              </el-scrollbar>
            </div>
          </div>
        </div>
      </div>
    </div>
    <div class="btn-group">
      <el-button class="button" type="primary" plain @click="$emit('close')">
        取消
      </el-button>
      <el-button
        class="button"
        type="primary"
        @click="success"
        :disabled="successDisabled"
      >
        确定
      </el-button>
    </div>
  </div>
</template>
<script lang="ts" setup>
import { computed, ComputedRef, onBeforeMount, Ref, ref } from "vue";
import { Avatar, Empty, CanEditAvatar } from "~/components";
import {
  useConversationStore,
  useFriendStore,
  useUserStore,
  useGroupStore,
} from "~/stores";
import { IMTYPE } from "~/utils/imsdk";

type SelectItem = IMTYPE.FriendUserItem & {
  disabled?: boolean;
};

interface Type {
  type:
    | "all"
    | "conversation"
    | "friend"
    | "group"
    | "groupMember"
    | "createGroup";
  groupID?: string;
  selectList?: SelectItem[];
  multiple?: boolean;
}

type Props = {
  data: Type;
};
const props = withDefaults(defineProps<Props>(), {
  data: () => ({
    type: "all",
    multiple: true,
  }),
});

const $emit = defineEmits(["success", "close"]);
const conversationStore = useConversationStore();
const friendStore = useFriendStore();
const userStore = useUserStore();
const groupStore = useGroupStore();

let canSelectConversation: ComputedRef,
  canSelectFriend: ComputedRef,
  canSelectGroup: ComputedRef,
  selectList: ComputedRef,
  selectValue: Ref<number>,
  allList = ref<any[]>([]),
  checkList = ref<string[]>([]),
  groupName = ref(""),
  imageUrl = ref("");

selectList = computed(() => {
  return checkList.value.map((str) => {
    if (str.indexOf("f_") === 0) {
      const member = allList.value!.find(
        (item) => item.userID === str.slice(2)
      );
      if (member) {
        return {
          key: str,
          userID: member.userID,
          imgSrc: member.faceURL,
          showName: member.showName,
        };
      }
    } else if (str.indexOf("u_") === 0) {
      const frient = friendStore.list!.find(
        (item) => item.userID === str.slice(2)
      );
      if (frient) {
        return {
          key: str,
          userID: frient.userID,
          imgSrc: frient.faceURL,
          showName: frient.remark || frient.nickname,
        };
      }
    } else {
      const conversation = conversationStore.conversationList!.find(
        (item) => item.groupID === str.slice(2)
      );
      if (conversation) {
        return {
          key: str,
          groupID: conversation.groupID,
          imgSrc: conversation.faceURL,
          showName: conversation.showName,
        };
      }
    }
  });
});

const multipleDisabled = computed(
  () => props.data.multiple === false && checkList.value.length > 0
);

onBeforeMount(async () => {
  if (props.data.type === "groupMember") {
    allList.value = allList.value.concat(
      groupStore.currentGroupMemberList?.map((item) => ({
        userID: item.userID,
        showName: item.nickname,
        faceURL: item.faceURL,
        disabled: userStore.userInfo?.userID === item.userID,
      }))
    );
  } else {
    canSelectConversation = computed(
      () => props.data.type === "all" || props.data.type === "conversation"
    );
    canSelectFriend = computed(
      () =>
        props.data.type === "all" ||
        props.data.type === "friend" ||
        props.data.type === "createGroup"
    );
    canSelectGroup = computed(
      () => props.data.type === "all" || props.data.type === "group"
    );

    canSelectConversation && conversationStore.getList();
    canSelectFriend && friendStore.getList();
    canSelectGroup && groupStore.getList();

    let t: number;
    if (canSelectConversation.value) t = 1;
    else if (canSelectFriend.value) t = 2;
    else t = 3;

    selectValue = ref(t);
  }
});

const successDisabled = computed(() => {
  if (props.data.type === "createGroup" && !groupName.value.trim().length) {
    return true;
  }
  return selectList.value.length === 0;
});

const success = () => {
  if (props.data.type === "createGroup") {
    return $emit("success", {
      groupName: groupName.value,
      faceUrl: imageUrl.value,
      selectList: selectList.value,
    });
  }
  $emit("success", selectList.value);
};

const updateUrl = (url: string) => {
  imageUrl.value = url;
};
</script>
<style lang="scss" scoped>
.aic {
  align-items: center;
}
.container {
  display: flex;
  padding: 20px;
  flex-direction: column;
  .labelItem {
    display: flex;
    flex-direction: row;
    margin-bottom: 20px;
    .labelName {
      width: 100px;
    }
    .context {
      flex: 1;
      .avatar-uploader {
        .avatar {
          width: 40px;
          height: 40px;
          display: block;
        }
        &:deep() {
          .el-upload {
            border: 1px dashed var(--el-border-color);
            border-radius: 6px;
            cursor: pointer;
            position: relative;
            overflow: hidden;
            transition: var(--el-transition-duration-fast);
          }
          .el-upload:hover {
            border-color: var(--el-color-primary);
          }

          .el-icon.avatar-uploader-icon {
            font-size: 16px;
            color: #8c939d;
            width: 40px;
            height: 40px;
            text-align: center;
          }
        }
      }
    }
    &.select {
      p {
        margin-bottom: 14px;
        font-size: 14px;
        line-height: 14px;
        &.active {
          color: var(--el-color-primary);
        }
        &.cupo {
          cursor: pointer;
        }
      }
      .bd {
        flex: 1;
        border: 1px solid #ccc;
        border-radius: 10px;
        overflow: hidden;
        display: flex;
        .l {
          flex: 1;
          padding-top: 10px;
          .infinite-list {
            height: 390px;
            overflow: hidden;
          }
        }
        .r {
          flex: 1;
          border-left: 1px solid #ccc;
          .p {
            margin-top: 4px;
            margin-left: 20px;
            font-size: 16px;
            line-height: 20px;
          }
        }
        :deep(.el-checkbox),
        .selectItem {
          padding: 4px 20px;
          display: flex;
          height: 50px;
          .el-checkbox__label {
            flex: 1;
          }
        }
        .item,
        .selectItem {
          display: flex;
          align-items: center;
          .name {
            width: 0;
            flex: 1;
            margin-left: 10px;
            text-wrap: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
            line-height: 20px;
          }
          .icon {
            cursor: pointer;
            width: 16px;
            height: 16px;
            border: 1px solid #ccc;
            border-radius: 50%;
            color: #ccc;
            font-size: 12px;
          }
        }
      }
    }
  }
}
.btn-group {
  text-align: right;
  padding-bottom: 20px;
  margin-right: 20px;
  .button {
    width: 100px;
    padding: 0;
  }
}
</style>
