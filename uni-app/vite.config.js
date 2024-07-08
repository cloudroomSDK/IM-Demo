import path from "path"
import {
	defineConfig
} from "vite";
import uni from "@dcloudio/vite-plugin-uni";
import postcssPluginPx2rem from "postcss-plugin-px2rem";
import vueJsx from '@vitejs/plugin-vue-jsx';
// import AutoImport from 'unplugin-auto-import/vite';
// import Components from 'unplugin-vue-components/vite';
// import {
// 	VantResolver
// } from '@vant/auto-import-resolver';
const resolveDir = (dir) => path.resolve(__dirname, dir);

export default defineConfig({
	resolve: {
		alias: {
			// 'vue-i18n': 'vue-i18n/dist/vue-i18n.cjs.js'
		}
	},
	plugins: [
		uni(),
		vueJsx({
			// options are passed on to @vue/babel-plugin-jsx
			jsx: true,
		}),
		// AutoImport({
		// 	resolvers: [VantResolver()],
		// }),
		// Components({
		// 	resolvers: [VantResolver()],
		// }),
	],
	css: {
		postcss: {
			plugins: [postcssPluginPx2rem({
				rootValue: 16, // 16px = 1rem
				unitPrecision: 5, //允许REM单位增长到的十进制数字，其实就是精度控制
				// propWhiteList: [], // 默认值是一个空数组，这意味着禁用白名单并启用所有属性。
				// propBlackList: [], // 黑名单
				// exclude:false,  //默认false，可以（reg）利用正则表达式排除某些文件夹的方法，例如/(node_module)/ 。如果想把前端UI框架内的px也转换成rem，请把此属性设为默认值
				// selectorBlackList: [], //要忽略并保留为px的选择器
				// ignoreIdentifier: false, //（boolean/string）忽略单个属性的方法，启用ignoreidentifier后，replace将自动设置为true。
				// replace: true, // （布尔值）替换包含REM的规则，而不是添加回退。
				mediaQuery: false, //（布尔值）允许在媒体查询中转换px
				minPixelValue: 1 //设置要替换的最小像素值(3px会被转rem)。 默认 0
			})]
		}
	}
});