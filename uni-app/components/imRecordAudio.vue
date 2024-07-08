<!-- <template>
	<view class="btnListBox">
		<view class="audioBox" v-if="audioLength">
			<u-row>
				<u-col span="2">
					<u--text align='center' :text="currentTime"></u--text>
				</u-col>
				<u-col span="8">
					<u-slider @change='currentTime_change' v-model="currentTime" :max='audioLength'></u-slider>
				</u-col>
				<u-col span="2">
					<u--text align='center' :text="audioLength+' 秒'"></u--text>
				</u-col>
			</u-row>
		</view>
		<view v-if="record_status === '准备录音'" @click="startRecord">
			<u-icon :color='iconColor' name="mic" size='100px'></u-icon>
			<u--text align='center' text="点我开始录音"></u--text>
		</view>
		<view v-if="record_status === '录音中'" @click="endRecord">
			<Breathing>
				<u-icon :color='iconColor' name="mic" size='100px'></u-icon>
			</Breathing>
			<u--text align='center' text="录音中"></u--text>
		</view>
		<view v-if="record_status === '已录音'" class="row">
			<view @click="reset">
				<u-icon :color='iconColor' name="reload" size='100px'></u-icon>
				<u--text align='center' text="重新录制"></u--text>
			</view>
			<view v-if="play_status== '待播放' ||  play_status== '暂停中'" @click="play">
				<u-icon :color='iconColor' name="play-circle-fill" size='100px'></u-icon>
				<u--text align='center' text="播放试听"></u--text>
			</view>
			<view v-if="play_status== '播放中'" @click="pause">
				<u-icon :color='iconColor' name="pause-circle-fill" size='100px'></u-icon>
				<u--text align='center' text="暂停播放"></u--text>
			</view>
		</view>
	</view>
</template>
<script>
	// 创建对象--录音机
	const recorderManager = uni.getRecorderManager();
	// 创建对象--录音
	const audio = uni.createInnerAudioContext()
	import Breathing from "../components/breathing.vue";
	export default {
		components: {
			Breathing,
		},
		data() {
			return {
				iconColor: '#3c9cff',
				// 录音当前播放位置
				currentTime: 0,
				audioLength: 0,
				record_status: '准备录音',
				play_status: '待播放',
				voicePath: '',
			}
		},
		onLoad() {
			let self = this;
			recorderManager.onStart(res => {});
			recorderManager.onStop(res => {
				self.voicePath = res.tempFilePath;
				audio.src = res.tempFilePath
			});
			audio.onCanplay(res => {
				if (audio.duration) {
					self.audioLength = audio.duration.toFixed(0)
				}
			})
			audio.onTimeUpdate(res => {
				self.audioLength = audio.duration.toFixed(0)
				self.currentTime = audio.currentTime.toFixed(0);
			});
			audio.onEnded(res => {
				self.currentTime = 0
				self.play_status = '待播放'
			});
		},
		methods: {
			currentTime_change(new_currentTime) {
				audio.seek(new_currentTime)
			},
			reset() {
				this.record_status = '准备录音'
				this.voicePath = ''
				this.audioLength = 0
				this.currentTime = 0
			},
			startRecord() {
				recorderManager.start();
				this.record_status = '录音中'
			},
			endRecord() {
				recorderManager.stop();
				this.record_status = '已录音'
			},
			play() {
				if (this.voicePath) {
					audio.play();
					this.play_status = '播放中'
				}
			},
			pause() {
				audio.pause();
				this.play_status = '暂停中'
			}
		}
	}
</script>
<style scoped>
	.audioBox {
		width: 100%;
	}

	.btnListBox {
		flex-direction: column;
		display: flex;
		justify-content: center;
		align-items: center;
		height: 100vh;
	}

	.row {
		width: 100%;
		display: flex;
		justify-content: space-evenly;
	}
</style> -->