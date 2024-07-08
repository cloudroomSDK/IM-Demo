import PinYin from "./pinyin";
export {
	appNavigator,
	AppRoutes
}
from "./appNavigator";
export {
	images
}
from "./images";
export {
	formatTime,
	html2Text,
	formatConversionTime,
	parseMessageByType
}
from "./imUtils";
import {
	appNavigator
}
from "./appNavigator";
import {
	scanCode,
}
from "./uniUtils";
import Effect from "./effect";
export const effect = new Effect();

export const getElRect = (el) => {
	return new Promise((resolve) => {
		uni.createSelectorQuery()
			.select(el)
			.boundingClientRect(resolve)
			.exec();
	});
};

export const getElCtx = (el) => {
	return new Promise((resolve) => {
		uni.createSelectorQuery()
			.select(el)
			.context(resolve)
			.exec();
	});
};

export const getDbDir = () => {
	return new Promise((resolve, reject) => {
		plus.io.requestFileSystem(plus.io.PRIVATE_DOC, (fs) => {
			fs.root.getDirectory(
				"user", {
					create: true,
				},
				(entry) => {
					resolve(entry.fullPath);
				},
				(error) => {
					reject(error);
				},
			);
		});
	});
};

export const getPurePath = (path) => {
	const prefix = "file://";
	const relativeRrefix = "_doc/";
	if (path.includes(prefix)) {
		path = path.replace(prefix, "");
	}
	if (path.includes(relativeRrefix)) {
		path = plus.io.convertLocalFileSystemURL(path);
	}
	return path;
};

export const formatChooseData = (data, key = "nickname") => {
	const ucfirst = (l1) => {
		if (l1.length > 0) {
			var first = l1.substr(0, 1).toUpperCase();
			var spare = l1.substr(1, l1.length);
			return first + spare;
		}
	};

	const arraySearch = (l1, l2) => {
		for (var name in PinYin) {
			if (PinYin[name].indexOf(l1) != -1) {
				return ucfirst(name);
				break;
			}
		}
		return false;
	};

	const codefans = (l1) => {
		l1 = l1 ?? "unkown";
		var l2 = l1.length;
		var I1 = "";
		var reg = new RegExp("[a-zA-Z0-9- ]");
		for (var i = 0; i < l2; i++) {
			var val = l1.substr(i, 1);
			var name = arraySearch(val, PinYin);
			if (reg.test(val)) {
				I1 += val;
			} else if (name !== false) {
				I1 += name;
			}
		}
		I1 = I1.replace(/ /g, "-");
		while (I1.indexOf("--") > 0) {
			I1 = I1.replace("--", "-");
		}
		return I1;
	};

	var arr = [],
		firstName;

	for (var i = 0; i < data.length; i++) {
		firstName = data[i].initial = codefans(data[i][key]).substr(0, 1);
		arr.push(firstName.toUpperCase());
	}

	var arrlist = [];
	for (i = 0; i < arr.length; i++) {
		if (arrlist.indexOf(arr[i]) == -1) {
			arrlist.push(arr[i]);
		}
	}

	var dataSort = [];
	for (var i = 0; i < arrlist.length; i++) {
		dataSort[i] = {
			initial: arrlist[i],
		};
		dataSort[i].data = [];
		for (var j = 0; j < data.length; j++) {
			if (data[j].initial.toUpperCase() == dataSort[i].initial) {
				dataSort[i].data.push(data[j]);
			}
		}
	}
	for (var i = 0; i < dataSort.length - 1; i++) {
		for (var j = 1; j < dataSort.length - i; j++) {
			if (dataSort[j - 1].initial > dataSort[j].initial) {
				var a = dataSort[j];
				dataSort[j] = dataSort[j - 1];
				dataSort[j - 1] = a;
			}
		}
	}
	const NomalInitial = "QWERTYUIOPLKJHGFDSAZXCVBNM".split("");
	const special = {
		initial: "#",
		data: [],
	};
	const newFilterData = dataSort.filter((d) => {
		if (!NomalInitial.includes(d.initial)) {
			special.data = [...special.data, ...d.data];
		} else {
			return d;
		}
	});
	if (special.data.length > 0) {
		newFilterData.push(special);
	}
	const anchors = newFilterData.map((item) => item.initial);
	const list = newFilterData.map((item) => item.data);
	return {
		list,
		anchors,
	};
};

// 构造二维码链接
const qrCodePerfix = "io.crim.app";
export const getQrCodeLink = (info) => {
	return "groupID" in info ? `${qrCodePerfix}/joinGroup/${info.groupID}` :
		`${qrCodePerfix}/addFriend/${info.userID}`;
};

// 扫码获取二维码链接
export const scanQrCodeResult = async () => {
	const res = await scanCode(["qrCode"]);
	if (res?.includes(qrCodePerfix)) {
		const arr = res.split("/");
		const type = arr[1];
		const targetId = arr[2];
		if (type === "addFriend") {
			// addFriend
			appNavigator.toFriCard({
				userID: targetId,
				isScan: true,
			});
		} else {
			// joinGroup
			appNavigator.toGrpCard({
				groupID: targetId,
				joinSource: 4,
				isScan: true,
			});
		}
	} else {
		uni.$u.$itoast("qrCodeIsError");
	}
};

// uview时间显示格式
export const uViewFormatterCn = (type, value) => `${value}${uni.$u.$t(`date.${type}`)}`;

const KBLimit = Math.pow(2, 10);
const MBLimit = Math.pow(2, 20);
const GBLimit = Math.pow(2, 30);
export const formatFileSize = size => {
	let position = 0;
	let unit = "B";
	if (size > GBLimit) {
		// 大于1GB的转GB
		position = 30;
		unit = "GB";
	} else if (size > MBLimit) {
		// 大于1MB的转MB
		position = 20;
		unit = "MB";
	} else if (size > KBLimit) {
		// 大于1KB的转KB
		position = 10;
		unit = "KB";
	}
	const sizes = size / Math.pow(2, position);
	return (sizes === parseInt(sizes) ? sizes : sizes.toFixed(2)) + unit;
};

export const GlobalEvents = {
	AtUser: "AtUser",
	TypingUpdate: "TypingUpdate",
	ScrollToBottom: "ScrollToBottom",
	SingleSelection: "SingleSelection",
	MultiSelection: "MultiSelection",
	AddMem: "AddMem",
	RemoveMem: "RemoveMem",
};