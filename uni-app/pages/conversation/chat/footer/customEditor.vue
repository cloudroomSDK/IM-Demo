<template>
	<view class="editor-wrap" :insertImageFlag="insertImageFlag" :insertAtFlag="insertAtFlag"
		:change:insertAtFlag="input.insertAtFlagUpdate" :change:insertImageFlag="input.insertImageFlagUpdate">
		<editor :placeholder="placeholder" :read-only="placeholder !== ''" ref="chateditor" id="chateditor"
			@ready="editorReady" @focus="editorFocus" @blur="editorBlur" @input="editorInput" />
		<view class="canvas-container">
			<canvas v-if="canvasData.show" canvas-id="atCanvas" :style="{ width: canvasData.width }"
				id="atCanvas"></canvas>
		</view>
	</view>
</template>

<script>
	import {
		ref,
		computed,
		reactive,
		nextTick,
		defineComponent,
		getCurrentInstance,
	} from "vue";
	import {
		getElCtx,
		html2Text
	}
	from "@/utils";
	import {
		canvasToTempFilePath
	} from "@/utils/uniUtils";

	export default defineComponent({
		setup(props, {
			emit
		}) {
			const insertImageFlag = ref(false);
			const insertAtFlag = ref(null);
			const canvasData = ref({
				width: 0,
				show: false,
			});

			let imageData = {};
			let editorContext = null;
			const editorReady = async () => {
				const res = await getElCtx("#chateditor");
				editorContext = res.context;
				emit("ready", editorContext);
			};
			const editorFocus = () => emit("focus");
			const instance = getCurrentInstance();
			const editorBlur = () => {
				emit("blur");
			}
			let lastStr = "";
			const editorInput = ({
				detail
			}) => {
				let str = detail.html ?? "";
				const oldArr = lastStr.split("");
				let contentStr = str;
				oldArr.forEach((str) => {
					contentStr = contentStr.replace(str, "");
				});
				contentStr = html2Text(contentStr);
				if (contentStr === "@") {
					emit("tryAt");
				}
				emit("input", detail);
				lastStr = detail.html ?? "";
			};

			const internalInsertImage = () => {
				editorContext.insertImage(Object.assign({}, imageData, {
					complete: () => {
						insertImageFlag.value = false;
						insertAtFlag.value = null;
						// plus.key.showSoftKeybord();
					},
				}));
			};

			const internalInsertAtEl = async ({
				text,
				width,
				userID,
				nickname
			}) => {
				canvasData.value = {
					width: `${width}px`,
					show: true
				}
				await new Promise((resolve) => {
					setTimeout(async () => {
						const ctx = uni.createCanvasContext("atCanvas");
						const fontSize = 14;
						ctx.setFontSize(fontSize);
						ctx.setFillStyle("#3e44ff");
						ctx.fillText(text, 0, 16);
						ctx.draw();
						const res = await canvasToTempFilePath("atCanvas");
						imageData = {
							src: res.tempFilePath,
							width,
							height: "20px",
							data: {
								sendID: userID,
								nickname,
							},
							extClass: "at_el",
						};
						insertImageFlag.value = true;
						resolve();
					}, 20);
				})
			};

			const internalInsertAtEls = async (atEls) => {
				for (let i = 0; i < atEls.length; i++) {
					const atEl = atEls[i];
					await internalInsertAtEl(atEl);
					// 延迟一下，等internalInsertImage完成 【靠谱一点的做法是watch一个internalInsertImage之后改变的值，来执行下一步】
					await new Promise((resolve) => {
						setTimeout(resolve, 0);
					});
				}
				// plus.key.showSoftKeybord();
				instance.proxy.chateditorFocus();
			};

			const createCanvasData = (atMems) => {
				nextTick(() => {
					insertAtFlag.value = atMems;
				});
			};

			const clearEditor = () => editorContext && editorContext.clear();

			const undoEditor = () => editorContext && editorContext.undo();

			return {
				editorReady,
				editorFocus,
				editorBlur,
				editorInput,
				insertImageFlag,
				insertAtFlag,
				imageData,
				canvasData,
				internalInsertAtEl,
				internalInsertAtEls,
				internalInsertImage,
				createCanvasData,
				clearEditor,
				undoEditor,
			}
		},
		props: {
			placeholder: {
				type: String,
				default: ""
			}
		},
		emits: ["ready", "focus", "blur", "input", "tryAt"],
		expose: ["clearEditor", "undoEditor", "createCanvasData"]
	})
</script>

<script module="input" lang="renderjs">
	export default {
		methods: {
			chateditorFocus() {
				console.log("chateditorFocus");
				const editor = this.$refs.chateditor.$el.querySelector('.ql-editor');
				editor.focus();
				console.log("editor.focus();");
			},
			insertImageFlagUpdate(newValue, oldValue, ownerVm, vm) {
				if (newValue === null) {
					return;
				}
				const editor = ownerVm.$el.querySelector('.ql-editor');
				if (newValue) {
					editor && editor.setAttribute('inputmode', 'none')
					ownerVm.callMethod('internalInsertImage')
				} else {
					editor && editor.removeAttribute('inputmode')
				}
			},
			insertAtFlagUpdate(newValue, oldValue, ownerVm, vm) {
				if (newValue === null) {
					return;
				}
				if (newValue) {
					const atElTemp = [];
					for (let i = 0; i < newValue.length; i++) {
						const item = newValue[i];
						const data = this.truncateText(`@${item.nickname}`, 120);
						atElTemp.push({
							...data,
							...item
						});
					}
					ownerVm.callMethod('internalInsertAtEls', atElTemp);
					// const data = this.truncateText(`@${newValue.nickname}`, 120)
					// ownerVm.callMethod('internalInsertAtEl', {
					// 	...data,
					// 	...newValue
					// })
				}
			},
			truncateText(text, maxWidth) {
				const container = document.createElement("div");
				container.style.width = "auto";
				container.style.overflow = "hidden";
				container.style.textOverflow = "ellipsis";
				container.style.whiteSpace = "nowrap";
				container.style.position = "absolute";
				container.style.visibility = "hidden";

				const textNode = document.createTextNode(text);
				container.appendChild(textNode);

				document.body.appendChild(container);

				const isOverflowing = container.scrollWidth > maxWidth;

				if (!isOverflowing) {
					const width = container.clientWidth + 4
					document.body.removeChild(container);
					return {
						text,
						width
					};
				}

				container.style.width = maxWidth + "px";
				container.style.visibility = "visible";

				let truncatedText = text;
				while (container.scrollWidth > maxWidth) {
					truncatedText = truncatedText.slice(0, -1);
					container.textContent = truncatedText + "...";
				}

				document.body.removeChild(container);
				return {
					text: `${truncatedText}...`,
					width: maxWidth + 4
				};
			}
		},
	}
</script>

<style lang="scss" scoped>
	.editor-wrap {
		position: relative;
	}

	#chateditor {
		padding: 0 12px;
		border-radius: 15px;
		min-height: 30px;
		max-height: 120px;
		height: auto;
		word-break: break-all;
		background: #F2F2F2;
	}

	:deep(.ql-editor) {
		img {
			vertical-align: sub !important;
		}

		p {
			padding: 4px;
		}
	}

	:deep(.ql-blank) {
		&::before {
			font-style: normal !important;
			font-size: 12px;
			color: #999;
			top: 50%;
			transform: translate(0, -50%);
		}
	}

	.canvas-container {
		position: fixed;
		bottom: -99px;
		z-index: -100;

		&-name {
			max-width: 240px;
			display: inline-block;
			overflow: hidden;
			text-overflow: ellipsis;
			white-space: nowrap;
		}

		#atCanvas {
			height: 20px;
		}

		.convas-container-name {
			font-size: 16px !important;
		}
	}
</style>