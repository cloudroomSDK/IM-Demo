<script setup>
import { ref, onMounted, onUnmounted } from "vue";
import { getMap } from "~/utils/amap";
import { v4 as uuidv4 } from "uuid";
import { amapKey } from "~/config";
const id = uuidv4();
let map = null;
const desc = ref("");
const src = ref("");
const props = defineProps({
  simple: {
    type: Boolean,
    default: false,
  },
  LngLat: {
    type: Array,
    default: [113.943062, 22.549078],
  },
  title: {
    type: String,
    default: "科兴科学园",
  },
});

const useKeyFn = async () => {
  try {
    const AMap = await getMap();
    map = new AMap.Map(id, {
      // 设置地图容器id
      // viewMode: "3D", // 是否为3D地图模式
      zoom: 16, // 初始化地图级别
      center: props.LngLat, // 初始化地图中心点位置
      scrollWheel: !props.simple, //地图是否可通过鼠标滚轮缩放浏览
      doubleClickZoom: !props.simple, //地图是否可通过双击鼠标放大地图
      dragEnable: !props.simple, //地图是否可通过鼠标拖拽平移
      keyboardEnable: !props.simple, //地图是否可通过键盘控制
      touchZoom: !props.simple, //地图在移动终端上是否可通过多点触控缩放浏览地图
    });

    const marker = new AMap.Marker({
      position: new AMap.LngLat(props.LngLat[0], props.LngLat[1]), //经纬度对象，也可以是经纬度构成的一维数组[116.39, 39.9]
      title: props.title,
    });
    const markerList = [marker];
    map.add(markerList);
    if (!props.simple) {
      var scale = new AMap.Scale();
      scale.addTo(map);
    }
  } catch (error) {
    console.error(error);
    desc.value = error;
  }
};

const useIframeFn = () => {
  src.value = `https://uri.amap.com/marker?position=${props.LngLat.join(
    ","
  )}&name=${props.title}&coordinate=gaode&callnative=0`;
};

onMounted(() => {
  if (amapKey) {
    useKeyFn();
  } else if (!props.simple) {
    useIframeFn();
  }
});

onUnmounted(() => {
  map?.destroy();
});
</script>

<template>
  <div v-if="amapKey" :id="id" class="container" ref="container">
    {{ desc }}
  </div>
  <div v-else-if="simple" class="container noshow" ref="container">
    点击查看地图
  </div>
  <div v-else class="container" ref="container">
    <iframe class="iframe" v-if="src" :src="src" frameborder="0"></iframe>
  </div>
</template>

<style>
.amap-logo {
  display: none !important;
  visibility: hidden !important;
}

.amap-copyright {
  display: none !important;
  visibility: hidden !important;
}
</style>
<style scoped lang="scss">
.container {
  width: 100%;
  height: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
  background-color: #f5f5f5;
  border-radius: 8px;
  .iframe {
    width: 100%;
    height: 100%;
  }
}
</style>
