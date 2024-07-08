import App from './App'
import components from '@/components';

// #ifdef VUE3
import {
	createSSRApp
} from 'vue'
import i18n, {
	$tt,
	$fprint
} from "@/i18n";
import * as Pinia from 'pinia';
import uViewPlus from '@/uni_modules/uview-plus';
import '@/api/request';

export function createApp() {
	const app = createSSRApp(App)
	for (const [key, component] of Object.entries(components)) {
		app.component(key, component);
	}
	app.use(i18n)
	app.use(Pinia.createPinia())
	app.use(uViewPlus)

	uni.$u.$t = $tt;
	uni.$u.$fprint = $fprint;
	uni.$u.$itoast = (k, fprint = [], opt = {
		duration: 2000
	}) => uni.showToast({
		title: fprint.length > 0 ? $fprint(k, fprint) : $tt(k),
		icon: 'none',
		duration: opt.duration
	});

	const $addUnit = uni.$u.addUnit;
	// 修改默认单位为rpx, 转单位时 * 2 // uv-p有的版本不支持修改$u.props（可见源码用没用uni.$u.props.xx.xx）. 另外一些组件嵌套组件，组件里传了固定的size
	uni.$u.addUnit = (value = 'auto', unit = '') => {
		if (/r?px|vw|vh|r?em/.test(value)) {
			unit = value.match(/r?px|vw|vh|r?em/)[0];
		}
		try {
			// uni.getSystemInfoSync().devicePixelRatio
			if (!/%/.test(value) && (unit === '' || unit === 'px')) {
				return $addUnit(Number.parseFloat(String(value)) * 2, 'rpx');
			}
		} catch (e) {}
		return $addUnit(value, unit);
	}
	uni.$u.setConfig({
		// 修改$u.config对象的属性
		config: {
			// 修改默认单位为rpx，相当于执行 uni.$u.config.unit = 'rpx'
			unit: 'rpx',
			color: {
				'u-primary': '#3981fc',
			}
		},
		// 修改$u.props对象的属性
		props: {
			// radio: { size: 30 },
		},
	});

	return {
		app,
		Pinia
	}
}
// #endif