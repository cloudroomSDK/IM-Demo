<template>
	<view class="msg-loc-con" @click="toLocation">
		<view class="name truncate">{{ location.name }}</view>
		<view class="addr truncate">{{ location.addr }}</view>
		<!-- <map class="map" :longitude="location.longitude" :latitude="location.latitude" scale="16" :markers="location.markers"></map> -->
		<image class="map" :src="img"></image>
	</view>
</template>

<script setup>
	import {
		ref,
		computed,
	} from "vue";
	import {
		appNavigator
	} from "@/utils";
	import config from "@/common/config";

	const props = defineProps({
		msgItem: {
			type: Object,
			default: {}
		}
	});

	const location = computed(() => {
		const {
			locationElem
		} = props.msgItem;
		const res = {
			name: "",
			addr: "",
			latitude: locationElem.latitude,
			longitude: locationElem.longitude,
			markers: [{
				id: 0,
				latitude: locationElem.latitude,
				longitude: locationElem.longitude,
				width: 50,
				height: 50,
				iconPath: "/uni_modules/vue3-map/static/map-inx.png"
			}]
		};
		try {
			const desc = JSON.parse(locationElem.description);
			Object.assign(res, desc);
		} catch (e) {}
		return res;
	});
	const img = computed(() => {
		const {
			latitude,
			longitude,
		} = props.msgItem.locationElem;

		const lat = Number.parseFloat(latitude).toFixed(6);
		const lng = Number.parseFloat(longitude).toFixed(6);

		return `https://restapi.amap.com/v3/staticmap?location=${lng},${lat}&zoom=10&size=200*160&markers=mid,0xFF0000,0:${lng},${lat}&key=${config.MapKey}`;
	});

	const toLocation = () => {
		appNavigator.previewMap({
			isPerview: true,
			...location.value
		});
	};

	// console.log("msgItem", props.msgItem);
</script>

<style lang="scss" scoped>
	.msg-loc-con {
		position: relative;
		width: 45vw;
		padding: 8px;
		border-radius: 6px;
		background-color: #f0f0f0;

		.addr {
			margin-top: 5px;
			font-size: 12px;
			color: #666;
		}

		.map {
			position: relative;
			margin-top: 5px;
			height: 120px;
			width: 100%;
		}

		.map-cover-view {
			position: absolute;
			width: 100%;
			height: 100%;
			top: 0;
			left: 0;
			background-color: rgba(255, 255, 0, 0.3);
		}
	}
</style>