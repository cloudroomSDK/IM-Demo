<template>
	<im-popover ref="popover" placement="bottom-end">
		<image class="more-menu" :src="images.moreMenu"></image>
		<template #content>
			<view class="more-menu-popover-wrapper">
				<view class="more-menu-text" @click="actionAction(item)" v-for="item,index in actions" :key="index">
					<image class="more-menu-icon" :src="item.image"></image>
					{{ item.text}}
				</view>
			</view>
		</template>
	</im-popover>
</template>

<script setup name="imMenus">
	import {
		onHide
	} from "@dcloudio/uni-app";
	import {
		ref,
		reactive
	} from "vue";
	import {
		appNavigator,
		images,
		scanQrCodeResult,
	} from "@/utils";

	const popover = ref(null);
	onHide(() => {
		popover.value.close();
	});

	const actions = [{
			image: images.scan,
			text: uni.$u.$t("contacts.scan"),
			index: 0,
			fn: () => {}
		},
		{
			image: images.addFri,
			text: uni.$u.$t("contacts.addFri"),
			index: 1,
			fn: appNavigator.toAddFri
		},
		{
			image: images.joinGrp,
			text: uni.$u.$t("contacts.joinGrp"),
			index: 2,
			fn: appNavigator.toJoinGrp
		},
		{
			image: images.createGrp,
			text: uni.$u.$t("contacts.createGrp"),
			index: 3,
			fn() {
				appNavigator.toChooseUser({
					redirect: "createGrp"
				});
			}
		},
	];

	const actionAction = ({
		index,
		fn,
	}) => {
		if (index === 0) {
			scanQrCodeResult();
		} else {
			fn && fn();
		}

		popover.value.close();
	};
</script>

<style lang="scss" scoped>
	.more-menu {
		width: 40px;
		height: 40px;

		&-popover-wrapper {
			padding: 10px 0;
		}

		&-icon {
			width: 20px;
			height: 20px;
			margin-right: 15px;
		}

		&-text {
			display: flex;
			flex: 1;
			align-items: center;
			height: 48px;
			padding: 0 30px 0 20px;
			word-wrap: break-word;
			white-space: nowrap;
			color: #3D3D3D;
		}
	}
</style>