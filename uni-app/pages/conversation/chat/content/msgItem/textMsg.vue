<template>
	<view class="msg-bg-con">
		<mp-html @linktap="linktap" :previewImg="false" :showImgMenu="false" :lazyLoad="false" :content="content" />
	</view>
</template>

<script setup>
	import {
		ref,
		computed
	} from "vue";
	import {
		isMySelf,
		parseBr,
		parseAt,
		parseLink
	} from "@/utils/imUtils";
	import {
		MessageType
	} from "@cloudroom/crimsdk";

	const props = defineProps({
		msgItem: {
			type: Object,
			default: {}
		}
	});

	const content = computed(() => {
		const {
			sendID,
			contentType,
			textElem,
			atTextElem,
			quoteElem,
		} = props.msgItem;
		if (contentType === MessageType.QuoteMessage) {
			return quoteElem.text;
		}
		if (contentType === MessageType.AtTextMessage) {
			return parseAt(atTextElem, false, {
				linkStyle: isMySelf(sendID) ? "color:#FFFFFF;" : "color:#0089FF;"
			});
		}
		return parseLink(parseBr(textElem?.content));
	});

	const emits = defineEmits(["clickAtUser"]);

	const linktap = ({
		innerText,
		href
	}) => {
		if (innerText.includes("@")) {
			emits("clickAtUser", href);
		}
	}
</script>

<style lang="scss" scoped>
</style>