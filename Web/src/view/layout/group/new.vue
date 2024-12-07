<template>
  <div>
    <div class="wrap">
      <div class="header">
        <div class="title">入群申请列表</div>
        <el-select class="select" v-model="selectValue" placeholder="请选择">
          <el-option label="入群申请" :value="1" />
          <el-option label="我的请求" :value="2" />
        </el-select>
      </div>

      <div class="context" v-loading="loading">
        <el-scrollbar
          class="scrollbar"
          v-if="list.length"
          :key="String(loading)"
        >
          <div class="list">
            <el-collapse accordion>
              <el-collapse-item
                class="listItem"
                v-for="(item, idx) in list"
                :key="item.userID"
                :name="idx"
              >
                <template #title>
                  <div class="left">
                    <Avatar :src="item.groupFaceURL" type="group" />
                    <span class="name">{{ item.groupName }}</span>
                  </div>
                  <div class="right">
                    <span v-if="item.handleResult === 0">未处理， </span>
                    群聊ID: {{ item.groupID }}
                  </div>
                </template>
                <el-descriptions border>
                  <el-descriptions-item label="申请者" :span="3">
                    <div class="ac">
                      <Avatar :src="item.userFaceURL" :size="36" />
                      <span>{{ item.nickname }}</span>
                    </div>
                  </el-descriptions-item>
                  <el-descriptions-item label="用户ID">
                    {{ item.userID }}
                  </el-descriptions-item>
                  <el-descriptions-item label="申请时间">
                    {{ formatString(item.reqTime) }}
                  </el-descriptions-item>
                  <el-descriptions-item label="来源">
                    <span v-if="item.joinSource === 2">邀请</span>
                    <span v-if="item.joinSource === 3">搜索</span>
                    <span v-if="item.joinSource === 4">二维码</span>
                  </el-descriptions-item>
                  <el-descriptions-item
                    v-if="item.inviterUserID"
                    label="邀请者ID"
                  >
                    <el-link
                      type="primary"
                      @click="openUserInfo(item.inviterUserID)"
                    >
                      {{ item.inviterUserID }}
                    </el-link>
                  </el-descriptions-item>
                  <el-descriptions-item label="申请信息" :span="3">
                    <span style="word-break: break-all;">{{ item.reqMsg }}</span>
                  </el-descriptions-item>
                  <el-descriptions-item
                    label="操作"
                    :span="3"
                    align="center"
                    label-align="left"
                  >
                    <div v-if="item.handleResult === 1">已接受</div>
                    <div v-else-if="item.handleResult === -1">已拒绝</div>
                    <div v-else-if="selectValue === 1">
                      <el-button type="danger" @click.stop="refuse(item)">
                        拒绝
                      </el-button>
                      <el-button type="primary" @click.stop="accept(item)">
                        接受
                      </el-button>
                    </div>
                    <div v-else>未处理</div>
                  </el-descriptions-item>
                </el-descriptions>
              </el-collapse-item>
            </el-collapse>
          </div>
        </el-scrollbar>
        <Empty v-else />
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { onMounted, ref, watch } from "vue";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { formatString } from "~/utils/dayjs";
import { Avatar, Empty } from "~/components";
import { openUserInfo } from "~/utils";
import { useGroupStore } from "~/stores";
const groupStore = useGroupStore();

const selectValue = ref(1);
const loading = ref(true);

const list = ref<IMTYPE.GroupApplicationItem[]>([]);

// 拒绝别人的好友申请
const refuse = (item: IMTYPE.GroupApplicationItem) => {
  IMSDK.refuseGrpReq({
    groupID: item.groupID,
    fromUserID: item.userID,
    handleMsg: "",
  });
  item.handleResult = -1;
};

// 同意别人的好友申请
const accept = (item: IMTYPE.GroupApplicationItem) => {
  IMSDK.acceptGrpReq({
    groupID: item.groupID,
    fromUserID: item.userID,
    handleMsg: "",
  });
  item.handleResult = 1;
};

const refersh = async () => {
  loading.value = true;
  if (selectValue.value === 1) {
    const { data } = await IMSDK.getGrpReqListAsRecipient();
    data.sort((a, b) => b.reqTime - a.reqTime);
    list.value = data;
  } else {
    const { data } = await IMSDK.getGrpReqListAsApplicant();
    data.sort((a, b) => b.reqTime - a.reqTime);
    list.value = data;
  }
  groupStore.messageCount = 0;
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
    .list {
      padding: 12px 154px 12px 112px;
      .listItem {
        border-color: var(--el-border-color);
        .left {
          flex: 1;
          display: flex;
          align-items: center;
          text-align: left;

          span {
            margin-left: 18px;
            font-size: 16px;
            &.name {
              width: 0;
              flex: 1;
              overflow: hidden;
              text-overflow: ellipsis;
              white-space: nowrap;
            }
          }
        }
        .right {
          margin: 0 10px;
        }
        .msg {
          padding-left: 20px;
        }
        :deep(.el-collapse-item__header) {
          height: 70px;
        }
      }
      .ac {
        display: flex;
        align-items: center;
        span {
          margin-left: 10px;
        }
      }
    }
  }
}
</style>
