<template>
  <div class="context">
    <div class="search">
      <el-input
        @keydown.enter="search"
        v-model="input"
        placeholder="请输入群聊ID"
        ref="$input"
      >
        <template #append>
          <el-button @click="search"><i-ep-search /></el-button>
        </template>
      </el-input>
    </div>
    <div v-loading="loading">
      <el-scrollbar height="300px" class="scrollbar">
        <div class="info" v-if="list.length">
          <div class="item" v-for="item in list" :key="item.groupID">
            <el-descriptions border :column="2">
              <el-descriptions-item label="群名" :span="2">
                <div class="ac">
                  <Avatar :src="item.faceURL" :size="36" />
                  <span class="text">{{ item.groupName }}</span>
                </div>
              </el-descriptions-item>
              <el-descriptions-item label="群聊ID">
                {{ item.groupID }}
              </el-descriptions-item>
              <el-descriptions-item label="创建时间">
                {{ formatString(item.createTime) }}
              </el-descriptions-item>
              <el-descriptions-item label="群主ID">
                {{ item.ownerUserID }}
              </el-descriptions-item>
              <el-descriptions-item label="验证方式" :span="2">
                <span v-if="item.needVerification === 2">允许所有人加群</span>
                <span v-else>需管理员验证</span>
              </el-descriptions-item>
              <el-descriptions-item
                v-if="!item.isJoin && item.needVerification !== 2"
                label="填写验证信息"
                :span="3"
                align="center"
              >
                <el-input
                  v-model="input2"
                  style="width: 100%; height: 100%"
                  placeholder="验证信息"
                />
              </el-descriptions-item>
              <el-descriptions-item label="操作" :span="2" align="center">
                <span v-if="item.isJoin">您已在群聊中</span>
                <template v-else>
                  <el-button
                    v-if="item.needVerification === 2"
                    type="primary"
                    @click="join(item, 'join')"
                  >
                    加入群聊
                  </el-button>
                  <el-button
                    v-else
                    type="primary"
                    @click="join(item, 'send')"
                    :disabled="input2.trim() === ''"
                  >
                    发送验证信息
                  </el-button>
                </template>
              </el-descriptions-item>
            </el-descriptions>
          </div>
        </div>
        <Empty v-else />
      </el-scrollbar>
    </div>
  </div>
</template>
<script lang="ts" setup>
import { ref } from "vue";
import { Empty, Avatar } from "~/components";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { formatString } from "~/utils/dayjs";
import { useGroupStore } from "~/stores";
import { ElMessage } from "element-plus";

const input = ref("");
const input2 = ref("");
const $input = ref<HTMLElement | null>(null);
type GroupItem = IMTYPE.GroupItem & {
  isJoin?: boolean;
};
const list = ref<GroupItem[]>([]);
const loading = ref(false);
const groupStore = useGroupStore();

const search = async () => {
  $input.value!.blur();
  if (!input.value) {
    list.value = [];
    return;
  }
  loading.value = true;
  try {
    const { data } = await IMSDK.getSpecifiedGrpsInfo([input.value]);
    const grpList = await groupStore.getList();
    list.value = data.map((item) => {
      const idx = grpList.findIndex((item2) => item2.groupID === item.groupID);
      return { ...item, isJoin: idx > -1 };
    });
    input2.value = "";
  } catch (error) {}
  loading.value = false;
};
const join = async (group: IMTYPE.GroupItem, val: "join" | "send") => {
  await IMSDK.joinGrp({
    groupID: group.groupID,
    reqMsg: input2.value,
    joinSource: 3,
  });
  ElMessage.success(val === "join" ? "加入成功" : "发送成功");
};
</script>

<style lang="scss" scoped>
.context {
  padding: 20px;
  .scrollbar {
    padding: 10px;
    .info {
      margin-top: 20px;
      .item {
        margin-bottom: 20px;
        .ac {
          display: flex;
          align-items: center;
          width: 100%;
          .text {
            flex: 1;
            width: 200px;
            text-wrap: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
            margin-left: 10px;
          }
        }
      }
    }
  }
}
</style>
