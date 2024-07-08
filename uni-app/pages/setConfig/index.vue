<template>
	<view class="page-view-container">
		<u-navbar :title="$t('pages.setConfig')" autoBack border>
		</u-navbar>
		<view class="set-config">
			<u-form class="base-form" :model="configModel" :rules="rules" errorType="toast" ref="from"
				labelWidth="100px" labelAlign="rights">
				<view class="set-config-card">
					<u-form-item :label="$t('setConfig.sdkServer')" :borderBottom="true" prop="sdkServer">
						<u-input v-model="configModel.sdkServer" border="none" clearable>
						</u-input>
					</u-form-item>
					<u-form-item :label="$t('setConfig.businessServer')" prop="businessServer">
						<u-input v-model="configModel.businessServer" border="none" clearable>
						</u-input>
					</u-form-item>
				</view>

				<view class="set-config-card">
					<u-form-item :label="`${$t('setConfig.authType')}：`" :borderBottom="true">
						<view class="base-form-item text-right pr-15" @tap="showPickAuthType">
							<text class="flex-1">{{ authTypeCn }}</text>
						</view>
						<template #right>
							<u-icon class="pr-15" name="arrow-down"></u-icon>
						</template>
					</u-form-item>

					<template v-if="isAppIDAuth">
						<u-form-item label="AppID：" :borderBottom="true" prop="appID">
							<u-input v-model="configModel.appID" border="none" clearable>
							</u-input>
						</u-form-item>
						<u-form-item label="AppSecret：" :borderBottom="true" prop="appSecret">
							<u-input v-model="configModel.appSecret" type="password" border="none" clearable>
							</u-input>
						</u-form-item>
					</template>
					<u-form-item label="Token：" :borderBottom="true" prop="token" v-else>
						<u-input v-model="configModel.token" border="none" clearable>
						</u-input>
					</u-form-item>
				</view>

				<view class="set-config-button save">
					<text @click="save">{{ $t('save') }}</text>
				</view>


				<view class="set-config-button reset">
					<text @click="reset">{{ $t('setConfig.reset')}}</text>
				</view>
			</u-form>
			<!-- 鉴权方式picker -->
			<u-picker class="picker-auth-type" :title="$t('setConfig.authType')" :visibleItemCount="4"
				:defaultIndex="defaultIndex" :show="show" :columns="columns" keyName="label" @cancel="cancelAuthType"
				@confirm="confirmAuthType" />
		</view>
	</view>
</template>

<script setup>
	import {
		ref,
		reactive,
		computed,
	} from "vue";
	import config from "@/common/config";
	import appStorage from "@/common/storage";
	import crimsdk from "@cloudroom/crimsdk";
	const configModel = reactive(appStorage.getConfigModel());
	if (configModel.appID === config.pubAppID) {
		configModel.appID = "默认AppID";
	}
	const rules = {
		sdkServer: [{
			type: "string",
			required: true,
			message: "请输入SDK服务器地址",
			trigger: ["blur", "change"],
		}, ],
		businessServer: [{
			type: "string",
			required: true,
			message: "请输入业务服务器地址",
			trigger: ["blur", "change"],
		}, ],
		appID: [{
			validator: (rule, value, callback) => {
				if (configModel.authType === 0 && uni.$u.isEmpty(uni.$u.trim(value))) {
					return false;
				}
				return true;
			},
			message: "请输入appID",
			trigger: ["blur", "change"],
		}, ],
		appSecret: [{
			validator: (rule, value, callback) => {
				if (configModel.authType === 0 && uni.$u.isEmpty(uni.$u.trim(value))) {
					return false;
				}
				return true;
			},
			message: "请输入appSecret",
			trigger: ["blur", "change"],

		}, ],
		token: [{
			validator: (rule, value, callback) => {
				if (configModel.authType === 1 && uni.$u.isEmpty(uni.$u.trim(value))) {
					return false;
				}
				return true;
			},
			message: "请输入token",
			trigger: ["blur", "change"],
		}, ],
	};

	const isAppIDAuth = computed(() => configModel.authType === 0);

	// 鉴权类型
	const show = ref(false);
	const aclabel = "账号密码鉴权",
		tokenLabel = "动态Token鉴权";
	const columns = reactive([
		[{
			label: aclabel,
			value: 0
		}, {
			label: tokenLabel,
			value: 1
		}],
	]);
	const defaultIndex = computed(() => [configModel.authType]);
	const authTypeCn = computed(() => configModel.authType === 0 ? aclabel : tokenLabel);
	const showPickAuthType = () => {
		show.value = true;
	};
	const confirmAuthType = ({
		value
	}) => {
		configModel.authType = value[0].value;
		show.value = false;
	};
	const cancelAuthType = () => {
		show.value = false;
	};

	const save = () => {
		appStorage.setConfigModel(configModel);
		crimsdk.unInitSDK()
		plus.runtime.restart();
	};

	const reset = () => {
		const cfgModel = appStorage.resetConfigModel();
		for (let k in cfgModel) {
			configModel[k] = cfgModel[k];
		}
		crimsdk.unInitSDK();
		plus.runtime.restart();
	};
</script>

<style lang="scss" scoped>
	.set-config {

		&-card {
			padding-left: 20px;
			margin-bottom: 14px;
			background-color: #fff;
		}

		.base-form-item {
			display: flex;
			position: relative;
			width: 100%;
			height: $base-form-item-height;
			align-items: center;
		}

		&-button {
			height: $base-form-item-height;
			line-height: $base-form-item-height;
			text-align: center;
			background-color: #fff;

			&.save {
				margin-top: 25px;
				color: $uni-color-primary;
			}

			&.reset {
				margin-top: 12px;
				color: $uni-color-danger;
			}
		}
	}

	:deep() {
		.u-input {
			padding-right: 8px !important;
		}

		.u-picker__view__column__item {
			font-size: 24px;
			font-weight: normal !important;
			;
		}
	}
</style>