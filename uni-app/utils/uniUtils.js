export const downloadFile = (url, opt = {}) => {
	return new Promise((resolve, reject) => {
		const downloadTask = uni.downloadFile({
			url,
			success: resolve
		});

		// console.log('下载进度' + result.progress);
		// console.log('已经下载的数据长度' + result.totalBytesWritten);
		// console.log('预期需要下载的数据总长度' + result.totalBytesExpectedToWrite);
		opt.onProgressUpdate && downloadTask.onProgressUpdate(opt.onProgressUpdate);
	});
}

// 预览图片
// [current: 下标]
export const previewImage = (urls, current, itemList = ['保存图片']) => {
	return new Promise((resolve, reject) => {
		uni.previewImage({
			current,
			urls,
			longPressActions: {
				itemList,
				success(data) {
					downloadFile(urls[data.index]).then(res => {
						saveImageToPhotosAlbum(res.tempFilePath);
					});
				}
			},
			fail(err) {
				console.log(err);
			},
		});
	});
};


export const saveImageToPhotosAlbum = (filePath) => {
	return new Promise((resolve, reject) => {
		uni.saveImageToPhotosAlbum({
			filePath,
			success() {
				resolve();
				uni.showToast({
					title: '已保存到系统相册',
					icon: "none"
				})
			},
			fail(err) {
				reject();
				uni.showToast({
					title: '保存失败',
					icon: "none"
				})
			}
		})
	});
}

//数组 "album" or "camera" 默认 ["album", "camera"]
export const chooseImage = (sourceType, count = 9) => {
	return new Promise((resolve, reject) => {
		uni.chooseImage({
			count,
			sizeType: ["compressed"],
			sourceType,
			success: function({
				tempFilePaths
			}) {
				resolve(tempFilePaths);
			},
			fail: function(err) {
				console.log(err);
				reject(err);
			},
		});
	});
};

export const chooseVideo = (sourceType, extension = ["mp4"]) => {
	return new Promise((resolve, reject) => {
		uni.chooseVideo({
			compressed: true,
			sourceType,
			extension,
			success: function({
				tempFilePath,
				duration
			}) {
				const idx = tempFilePath.lastIndexOf(".");
				const videoType = tempFilePath.slice(idx + 1);
				if (tempFilePath.includes("_doc/")) {
					tempFilePath = `file://${plus.io.convertLocalFileSystemURL(tempFilePath)}`;
				}
				resolve({
					path: tempFilePath,
					videoType,
					duration: Number(duration.toFixed()),
					snapshotPath: ""
				});
			},
			fail: function(err) {
				console.log(err);
				reject(err);
			},
		});
	});
};

export const base64ToImage = (base64Data, extension) => {
	return new Promise((resolve, reject) => {
		const fs = uni.getFileSystemManager();
		const filePath = `${uni.env.USER_DATA_PATH}/temp.${extension}`;
		const base64 = base64Data.split(',')[1];
		fs.writeFile({
			filePath,
			data: base64,
			encoding: 'base64',
			success: (res) => {
				console.log('图片保存成功', filePath);
				resolve(res);
			},
			fail: (err) => {
				console.error('图片保存失败', err);
				reject(err);
			}
		});
	});
}


export const scanCode = (scanType = ["qrCode"]) => {
	return new Promise((resolve, reject) => {
		uni.scanCode({
			scanType,
			success: ({
				result
			}) => {
				resolve(result);
			},
			fail: function(err) {
				reject(err);
			},
		});
	});
}

export const canvasToTempFilePath = (canvasId) => {
	return new Promise((resolve, reject) => {
		uni.canvasToTempFilePath({
			canvasId,
			success: function(res) {
				resolve(res);
				uni.createCanvasContext(canvasId).clearRect();
			},
			fail: function(err) {
				reject(err);
			}
		})
	});
}

export const chooseFile = () => {
	return new Promise((resolve, reject) => {
		plus.io.requestFileSystem(plus.io.PRIVATE_DOC, function(fs) {
			// 可通过fs操作PRIVATE_DOC文件系统 
			console.log(Object.keys(fs));
		}, function(e) {
			alert("Request file system failed: " + e.message);
		});
	});
}

export const getLocation = () => {
	return new Promise((resolve, reject) => {
		uni.getLocation({
			// type: "wgs84", // "wgs84" | "gcj02"
			// geocode: false,
			// isHighAccuracy: false, // 开启高精度定位
			success: function(res) {
				resolve(res);
			},
			fail: function(err) {
				reject(err);
			}
		})
	});
}