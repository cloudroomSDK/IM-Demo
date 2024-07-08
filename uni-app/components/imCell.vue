<template>
	<view class="im-cell" :class="{ border, truncate, 'touch-feed': touchFeed }" :hover-stay-time="250"
		@click="emits('click')">
		<im-avatar :shape="shape" class="im-cell-avatar" :src="avatar" :size="size" :group="groupAvatar"
			v-if="!hideAvatar"></im-avatar>
		<u-cell class="im-cell-content" :title="title" :label="label" :isLink="isLink" :url="url"
			:border="cellBorder && !border">
			<template v-if="$slots.title" #title>
				<slot name="title"></slot>
			</template>
			<template v-if="$slots.value" #value>
				<slot name="value"></slot>
			</template>
		</u-cell>
	</view>
</template>

<script setup name="imCell">
	import {
		ref
	} from "vue";
	const props = defineProps({
		title: {
			type: String,
			default: ""
		},
		label: {
			type: String,
			default: ""
		},
		hideAvatar: {
			type: Boolean,
			default: false
		},
		avatar: {
			type: String,
			default: ""
		},
		groupAvatar: {
			type: Boolean,
			default: false
		},
		size: {
			type: Number,
			default: 40
		},
		shape: {
			type: String,
			default: "circle"
		},
		isLink: {
			type: Boolean,
			default: false
		},
		border: {
			type: Boolean,
			default: false
		},
		cellBorder: {
			type: Boolean,
			default: true
		},
		url: {
			type: String,
			default: ""
		},
		touchFeed: {
			type: Boolean,
			default: false
		},
		truncate: {
			type: Boolean,
			default: true
		},
	});
	const emits = defineEmits(["click"]);
</script>

<style lang="scss" scoped>
	.im-cell {
		display: flex;
		align-items: center;
		height: 62px;
		padding: 0 16px;
		box-sizing: border-box;

		&.border {
			border-bottom: 1px solid $u-bg-color;
		}

		&.touch-feed:active {
			background-color: $u-bg-color;
		}

		&-avatar {
			margin-right: 12px;
		}


		&.truncate {
			overflow: hidden;

			:deep(.u-cell) {

				.u-cell__title,
				.u-cell__title-text,
				.u-cell__body__content {
					overflow: hidden;
					white-space: nowrap;
					text-overflow: ellipsis;
				}
			}

		}

		&-content {
			flex: 1;
			height: 62px;
			overflow: hidden;
			background-color: transparent !important;

			:deep(.u-cell__body) {
				flex: 1;
				padding: initial;

				// .u-cell__body__content {}

				// .u-cell__title {}

				.u-cell__title-text {
					font-size: 16px;
					color: #000;
				}
			}
		}
	}
</style>