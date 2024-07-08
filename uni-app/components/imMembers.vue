<template>
	<view class="member-row" v-for="(items,index) in arrItems" :key="index">
		<view class="member-box" v-for="item in items" :key="item.userID" @click="clickItem(item)">
			<im-avatar :size="52" :src="item.faceURL"></im-avatar>
			<view class="member-name truncate" v-if="showname">{{ item.nickname }}</view>
		</view>
	</view>
</template>

<script setup name="imMembers">
	import {
		ref,
		computed,
		watch
	} from "vue";
	import {
		appNavigator,
		images
	} from "@/utils";
	import useUserStore from "@/store/user";

	const userStore = useUserStore();

	const props = defineProps({
		type: {
			type: String,
			default: ""
		},
		layout: {
			type: String,
			default: ""
		},
		showname: {
			type: Boolean,
			default: true
		},
		members: {
			type: Object,
			default: []
		},
		maxShowNum: {
			type: Number,
			default: 15
		},
		choose: {
			type: Function,
			default: (data) => {}
		},
		groupID: {
			type: String,
			default: ""
		},
	});

	const arrItems = computed(() => {
		const layout = props.layout.replace(/\s+/g, "").split(",");
		const datas = layout.reduce((data, lay) => {
			if (lay === "members") {
				const len = props.maxShowNum + 1 - layout.length;
				const mems = props.members.slice(0, len);
				// 根据最大显示数显示群成员
				data.push(...mems);
			} else if (lay === "create") {
				data.push({
					type: "create",
					faceURL: images.add,
					nickname: uni.$u.$t("members.add")
				});
			} else if (lay === "add") {
				data.push({
					type: "add",
					faceURL: images.add,
					nickname: uni.$u.$t("members.add")
				});
			} else if (lay === "remove") {
				data.push({
					type: "remove",
					faceURL: images.remove,
					nickname: uni.$u.$t("members.remove")
				});
			} else if (lay === "self") {
				data.push({
					type: "self",
					faceURL: userStore.faceURL,
					nickname: userStore.nickname
				});
			}
			return data;
		}, []);
		const result = [];
		for (let i = 0; i < datas.length; i += 5) {
			result.push(datas.slice(i, i + 5));
		}
		return result;
	});

	const emits = defineEmits(["click"]);
	const memIds = () => {
		const memIds = props.members.reduce((result, mem) => {
			result.push(mem.userID);
			return result;
		}, []);
		return memIds.join(",");
	}

	const clickItem = (item) => {
		const {
			type
		} = item;
		if (type === "create") {
			appNavigator.toChooseUser({
				redirect: "createGrp"
			});
		} else if (props.groupID && type === "remove") {
			// 删除：有群id从群成员选
			appNavigator.toChooseMember({
				type,
				groupID: props.groupID,
				members: memIds(),
			}, {
				events: {
					choose(data) {
						props.choose({
							type,
							...data
						});
					}
				}
			});
		} else if (type === "add" || type === "remove") {
			// 添加：从好友选
			// 删除未创建成功的群成员：从memIds选
			appNavigator.toChooseUser({
				type,
				members: memIds(),
			}, {
				events: {
					choose(data) {
						props.choose({
							type,
							...data
						});
					}
				}
			});
		} else if (type === "self") {
			// 点自己的头像 暂不处理了
			emits("click", item);
		} else {
			emits("click", item);
		}
	};
</script>
<style lang="scss" scoped>
	.member-row {
		// display: flex;
		// justify-content: space-between;

		&+.member-row {
			margin-top: 8px;
		}
	}

	.member-box {
		display: inline-block;
		width: 20%;
		text-align: center;

		:deep(.u-avatar) {
			display: inline-block;
		}
	}

	.member-name {
		width: 100%;
		font-size: 12px;
		color: #333;
		margin-top: 8px;
		padding: 0 4px;
		box-sizing: border-box;
	}
</style>