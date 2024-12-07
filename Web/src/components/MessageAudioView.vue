<template>
  <div class="view" :class="reverse ? 'r' : 'l'">
    <el-check-tag
      class="tag"
      type="primary"
      effect="dark"
      :checked="checked"
      @change="onChange"
    >
      <div class="audio">
        <i class="audio-icon">
          <i class="icon1" />
          <i class="icon2" v-show="iconType > 1" />
          <i class="icon3" v-show="iconType > 2" />
        </i>
        <span class="text">{{ props.data.duration }}''</span>
      </div>
    </el-check-tag>
    <el-button
      v-if="playerStatus === PlayerStatus.PAUSED"
      @click="continuePlay"
      type="info"
      size="small"
      class="button"
    >
      继续播放
    </el-button>
  </div>
</template>
<script setup lang="ts">
import { ref } from "vue";
import { IMTYPE } from "~/utils/imsdk";
import { AudioPlayer } from "~/utils/audioPlayer";
import { ElMessage } from "element-plus";
import { onUnmounted } from "vue";

interface Props {
  data: IMTYPE.MessageItem["soundElem"];
  reverse: boolean;
}

const props = withDefaults(defineProps<Props>(), {
  reverse: false,
});

enum PlayerStatus {
  PLAYING = 1,
  PAUSED = 2,
  STOPPED = 3,
}

const playerStatus = ref(PlayerStatus.STOPPED);

const checked = ref(false);
const iconType = ref(3);

const onChange = (val: boolean) => {
  checked.value = val;
  checked.value ? playAudio() : pauseAudio();
};
let iconTimer: NodeJS.Timeout;
const playIcon = () => {
  iconTimer = setInterval(() => {
    if (iconType.value === 3) {
      iconType.value = 0;
    }
    iconType.value++;
  }, 300);
};
const player = new AudioPlayer(props.data.sourceUrl);
player.onEnded = () => {
  playerStatus.value = PlayerStatus.STOPPED;
  checked.value = false;
  clearInterval(iconTimer);
  iconType.value = 3;
};
player.onPause = () => {
  playerStatus.value = PlayerStatus.PAUSED;
  checked.value = false;
  clearInterval(iconTimer);
  iconType.value = 3;
};
player.onError = () => {
  ElMessage.error("播放失败");
};

const playAudio = () => {
  player.play();
  playIcon();
  playerStatus.value = PlayerStatus.PLAYING;
};
const pauseAudio = () => {
  player.pause();
  playerStatus.value = PlayerStatus.PAUSED;
  clearInterval(iconTimer);
  iconType.value = 3;
};
const continuePlay = () => {
  player.continue();
  checked.value = true;
  playerStatus.value = PlayerStatus.PLAYING;
  playIcon();
};
onUnmounted(() => {
  player.destroy();
});
</script>

<style lang="scss" scoped>
.view {
  display: flex;
  align-items: center;
  .button {
    margin: 0 10px;
    border-radius: 99px;
    --el-color-info: #efefef;
    color: #777;
  }
  &.l {
    .audio-icon {
      float: left;
      margin-right: 6px;
      margin-left: -20px;
      .icon2,
      .icon3 {
        border-right-width: 0.15em;
      }
    }
  }
  &.r {
    flex-direction: row-reverse;
    .audio-icon {
      float: right;
      margin-left: 6px;
      margin-right: -14px;
      .icon2,
      .icon3 {
        border-left-width: 0.15em;
      }
    }
  }
  .audio {
    font-size: 16px;
    line-height: 20px;
    height: 20px;
    .text {
      color: #666;
    }
    .audio-icon {
      position: relative;
      width: 2em;
      height: 1.2em;
      font-size: 16px;

      .icon1 {
        position: absolute;
        left: 50%;
        top: 50%;
        transform: translate(-50%, -50%);
        border: 0.1em solid var(--el-color-primary);
        border-radius: 50%;
      }
      .icon2 {
        position: absolute;
        left: 50%;
        top: 50%;
        transform: translate(-50%, -50%);
        padding: 0.3em 0.3em;

        border-style: solid;
        border-color: var(--el-color-primary);
        border-radius: 0.5em;
      }
      .icon3 {
        position: absolute;
        left: 50%;
        top: 50%;
        transform: translate(-50%, -50%);
        padding: 0.5em 0.7em;
        border-style: solid;
        border-color: var(--el-color-primary);
        border-radius: 0.7em;
      }
    }
  }
}
</style>
