import msg from "@/static/audio/msg.wav";

export default class Effect {

	msgAudioInstance;
	isAndroid = uni.getSystemInfoSync().platform === "android";

	constructor() {}

	get msgAudio() {
		if (!this.msgAudioInstance) {
			if (this.isAndroid) {
				const main = plus.android.runtimeMainActivity();
				const RingtoneManager = plus.android.importClass(
					"android.media.RingtoneManager",
				);
				const uri = RingtoneManager.getActualDefaultRingtoneUri(
					main,
					RingtoneManager.TYPE_NOTIFICATION,
				);
				const MediaPlayer = plus.android.importClass(
					"android.media.MediaPlayer",
				);
				this.msgAudioInstance = MediaPlayer.create(main, uri);
			} else {
				this.msgAudioInstance = uni.createInnerAudioContext();
				this.msgAudioInstance.autoplay = false;
				this.msgAudioInstance.src = msg;
			}
		}
		return this.msgAudioInstance;
	}

	play() {
		if (this.isAndroid) {
			this.msgAudio.setLooping(false);
			this.msgAudio.prepare();
			this.msgAudio.start();
		} else {
			this.msgAudio.play();
		}
	}

	vibrate() {
		if (this.isAndroid) {
			plus.device.vibrate(500);
		} else {
			plus.device.vibrate();
		}
	}

	vibrateFeedback() {
		if (this.isAndroid) {
			uni.vibrateLong();
		} else {
			const UIImpactFeedbackGenerator = plus.ios.importClass(
				'UIImpactFeedbackGenerator',
			);
			const impact = new UIImpactFeedbackGenerator();
			impact.prepare();
			impact.init(1);
			impact.impactOccurred();
		}
	}
}