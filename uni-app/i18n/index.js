import {
	createI18n
} from 'vue-i18n';
import en from './en.json';
import zhHans from './zh-Hans.json';

const i18n = createI18n({
	locale: uni.getLocale(),
	messages: {
		'zh-Hans': zhHans,
		en
	},
});

export default i18n;
export const $tt = i18n.global.t;
export const $fprint = (str, matchs) => {
	let count = 0;
	return $tt(str)?.replace(/(%s)+/g, () => matchs[count++]);
}