<template>
	<view class="page-view-map">
		<map v-if="isPerview" class="map" :longitude="data.longitude" :latitude="data.latitude" scale="16"
			:markers="markers"></map>
		<vue3-map v-else @commitCheck="commitCheck" :mapKey='data.mapKey' :Radius='data.Radius'
			:showResetting='data.showResetting' :lat="data.latitude" :lng="data.longitude"></vue3-map>

	</view>
</template>

<script setup>
	import {
		onLoad
	} from "@dcloudio/uni-app";
	import {
		ref,
		reactive,
		computed,
		getCurrentInstance,
	} from 'vue';
	import {
		appNavigator,
	} from "@/utils";
	import config from "@/common/config";

	const isPerview = ref(true);

	const data = reactive({
		mapKey: config.MapKey,
		address: '',
		latitude: '',
		longitude: '',
		showResetting: false,
		Radius: '',
	});

	const markers = computed(() => {
		return [{
			id: 1,
			latitude: data.latitude,
			longitude: data.longitude,
			width: 50,
			height: 50,
			iconPath: "/uni_modules/vue3-map/static/map-inx.png"
		}];
	});

	onLoad((params) => {
		if (params.isPerview === "true") {
			data.address = params.addr;
			data.latitude = params.latitude;
			data.longitude = params.longitude;
		} else {
			isPerview.value = false;
		}
	});


	const instance = getCurrentInstance().proxy;
	const eventChannel = instance.getOpenerEventChannel();
	const commitCheck = (e) => {
		eventChannel.emit('choose', {
			data: e
		});
		appNavigator.back();
	};
</script>

<style lang="scss" scoped>
	.page-view-map {
		@include pageView(0px);
	}

	.map {
		width: 100%;
		height: 100%;
	}
</style>