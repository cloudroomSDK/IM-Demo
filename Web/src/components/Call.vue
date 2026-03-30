<template>
  <div class="call-modal">
    <div class="body">
      <div
        class="remote-video"
        v-if="remoteCamOpened"
        v-SDKVideo.user="{ UID: invitationView.conversation!.userID }"
      />
      <div class="place" v-else>
        <Avatar :src="invitationView.conversation!.faceURL" :size="60" />
        <div class="nickname">{{ invitationView.conversation!.showName }}</div>
        <div class="status">
          <template
            v-if="
              invitationView.invitationStatus === 'waiting' ||
              invitationView.invitationStatus === 'ended'
            "
          >
            <span v-if="invitationView.opType === InvitationOpType.BUTT">
              {{
                invitationView.isMyCall
                  ? "正在呼叫..."
                  : `邀请我进行${invitationView.mediaType === "audio" ? "语音" : "视频"}通话。`
              }}
            </span>
            <span v-else-if="invitationView.opType === InvitationOpType.Reject"
              >对方挂断了呼叫！</span
            >
            <span v-else-if="invitationView.opType === InvitationOpType.Timeout"
              >对方没有应答！</span
            >
            <span v-else-if="invitationView.opType === InvitationOpType.Busy"
              >对方忙，请稍后再拨!</span
            >
            <span
              v-else-if="
                invitationView.opType === InvitationOpType.AcceptedByOther
              "
              >已在其他设备接听</span
            >
            <span
              v-else-if="
                invitationView.opType === InvitationOpType.RejectedByOther
              "
              >已在其他设备拒绝</span
            >
          </template>
          <template v-else-if="invitationView.invitationStatus === 'calling'">
            <span>正在连接通话服务器...</span>
          </template>
        </div>
      </div>

      <div
        class="my-video"
        v-if="myCamOpened"
        v-SDKVideo.user="{ UID: userStore.getMyUserID }"
      />
    </div>
    <div
      class="footer"
      :class="{
        fadeIn:
          invitationView.invitationStatus === 'meeting' && remoteCamOpened,
      }"
    >
      <div
        class="container"
        v-if="invitationView.invitationStatus === 'meeting'"
      >
        <div class="status">
          <span>{{ formatDuration(count) }}</span>
        </div>
        <div
          class="ctrl"
          :style="{ marginTop: remoteCamOpened ? '0' : '20px' }"
        >
          <div class="item">
            <img
              :src="myMicOpened ? IOpenMic : ICloseMic"
              @click="myMicOpened = !myMicOpened"
            />
          </div>
          <div class="item">
            <img :src="Ihangup" @click="onHangup" />
          </div>
          <div class="item">
            <img
              :src="myCamOpened ? IOpenCam : ICloseCam"
              @click="myCamOpened = !myCamOpened"
            />
          </div>
        </div>
      </div>
      <div class="ctrl" v-else>
        <div
          class="item"
          v-if="
            !invitationView.isMyCall &&
            invitationView.invitationStatus === 'waiting'
          "
        >
          <img :src="Ianswer" @click="onAnswer" />
          <span>接 听</span>
        </div>
        <div class="item">
          <img :src="Ihangup" @click="onHangup" />
          <span>挂 断</span>
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { InvitationOpType } from "~/utils/callModule";
import { Avatar } from ".";
import Ianswer from "~/assets/icons/call_answer.svg";
import Ihangup from "~/assets/icons/call_hangup.svg";
import IOpenCam from "~/assets/icons/call_open_cam.svg";
import ICloseCam from "~/assets/icons/call_close_cam.svg";
import IOpenMic from "~/assets/icons/call_open_mic.svg";
import ICloseMic from "~/assets/icons/call_close_mic.svg";
import { InvitationView } from "~/utils/InvitationView";
import { ref, watch } from "vue";
import { formatDuration } from "~/utils/dayjs";
import { RTCCtrl, vSDKVideo } from "~/utils/rtcsdk";
import { useUserStore } from "~/stores";

const count = ref(0);
let timerID: NodeJS.Timeout | null = null;
const { invitationView } = defineProps<{
  invitationView: InvitationView;
}>();

const userStore = useUserStore();

const myCamOpened = ref(false);
const myMicOpened = ref(false);
const remoteCamOpened = ref(false);

function close() {
  clearInterval(timerID!);
  invitationView.destory();
}
function onAnswer() {
  invitationView.onClickAccept();
}
function onHangup() {
  close();
}
// 暴露给父组件的方法
const childMethod = () => {
  console.log("子组件方法被调用，参数:");
};

watch(
  () => invitationView.invitationStatus,
  (newStatus) => {
    if (newStatus === "meeting") {
      RTCCtrl.setCallback({
        notifyUserCamOpened(userID, isOpened) {
          if (userID === invitationView.conversation?.userID) {
            remoteCamOpened.value = isOpened;
          }
        },
      });

      myMicOpened.value = true;
      if (invitationView.mediaType === "video") {
        myCamOpened.value = true;
      }
      // 在通话中时，调用父组件的方法
      timerID = setInterval(async () => {
        count.value = await invitationView.getDuration();
      }, 1000);

      remoteCamOpened.value = RTCCtrl.isCamOpened(
        invitationView.conversation!.userID,
      );
    }
  },
);

watch(myCamOpened, (newStatus) => {
  RTCCtrl.SwitchCam(userStore.getMyUserID!, newStatus);
});

watch(myMicOpened, (newStatus) => {
  RTCCtrl.SwitchMic(userStore.getMyUserID!, newStatus);
});

// 必须显式暴露方法
defineExpose({
  childMethod,
});
</script>

<style lang="scss" scoped>
.call-modal {
  width: 100%;
  height: 100%;
  background: #262b3b;
  color: #fff;
  user-select: none;
  display: flex;
  flex-direction: column;
  justify-content: center;
  gap: 16px;
  position: relative;
  .body {
    text-align: center;
    flex: 1;
    overflow: hidden;
    .remote-video {
      width: 100%;
      height: 100%;
    }
    .my-video {
      position: absolute;
      right: 4px;
      top: 4px;
      max-width: 50%;
      aspect-ratio: 16/9;
      box-shadow: 0 0 5px rgba(0, 0, 0, 0.5);
      overflow: hidden;
      border-radius: 4px;
    }
    .place {
      margin-top: 10%;
      .nickname {
        font-size: 24px;
        margin-top: 6px;
        font-weight: 600;
        line-height: 40px;
      }
      .status {
        margin-top: 10%;
        font-size: 15px;
      }
    }
  }

  .status {
    color: #b9c0c8;
    font-size: 13px;
    text-align: center;
    margin-bottom: 10px;
  }
  .footer {
    position: absolute;
    bottom: 5%;
    width: 100%;
    opacity: 1;
    &.fadeIn {
      transition: all 0.3s;
      opacity: 0;
      transform: translateY(100%);
    }

    .ctrl {
      display: flex;
      justify-content: space-around;
      align-items: center;

      .item {
        display: flex;
        flex-direction: column;
        text-align: center;
        gap: 6px;
        font-size: 14px;
        img {
          cursor: pointer;
        }
      }
    }
  }

  &:hover {
    .fadeIn {
      transform: translateY(0);
      opacity: 1;
    }
  }
}
</style>

<style lang="scss">
.call-zhezhao {
  --el-overlay-color-lighter: transparent;
  pointer-events: none;
  z-index: 99999 !important;
  .el-message-box {
    width: 270px;
    min-width: 270px;
    height: 340px;
    min-height: 340px;
    color: #fff;
    background-color: #1a1d28;
    pointer-events: all;
    resize: both;
    overflow: hidden;
    --el-messagebox-padding-primary: 0;
    --el-messagebox-width: unset;

    .el-message-box__header {
      --el-messagebox-font-size: 14px;
      --el-messagebox-title-color: #fff;
      .el-message-box__title {
        line-height: 34px;
        padding-left: 10px;
      }
    }
    .el-message-box__content {
      flex: 1;
      overflow: hidden;
      height: calc(100% - 34px);
    }
    .el-message-box__container,
    .el-message-box__message {
      width: 100%;
      height: 100%;
    }
  }
}
</style>
