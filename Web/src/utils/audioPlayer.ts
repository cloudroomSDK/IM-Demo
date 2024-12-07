import { v4 as uuidv4 } from "uuid";

const taskID: Record<string, AudioPlayer> = {};

class AuidoControl {
  private player?: HTMLAudioElement;
  private curPlayID?: string;
  constructor() {}
  create() {
    if (!this.player) {
      this.player = document.createElement("audio");
      this.player.onended = this.ended.bind(this);
      this.player.onerror = this.error.bind(this);
      document.body.appendChild(this.player);
    }
  }
  play(id: string, url: string, lastPauseTime = 0) {
    if (this.curPlayID) {
      taskID[this.curPlayID]._onPause(~~this.player!.currentTime);
    }
    this.create();
    this.curPlayID = id;
    this.player!.src = url;
    if (lastPauseTime) {
      this.player!.currentTime = lastPauseTime;
    }
    this.player!.play();
  }
  error() {
    const curPlayID = this.curPlayID;
    curPlayID && taskID[curPlayID].onError?.();
    this.ended();
  }
  ended() {
    const curPlayID = this.curPlayID;
    this.stop();
    try {
      curPlayID && taskID[curPlayID].onEnded?.();
    } catch (error) {
      console.error(error);
    }
  }
  stop(id?: string) {
    if (!id || this.curPlayID === id) {
      this.curPlayID = undefined;
      if (this.player) {
        document.body.removeChild(this.player);
        this.player = undefined;
      }
    }
  }
  getCurrentTime(id: string) {
    if (this.curPlayID === id) return this.player?.currentTime;
  }
}

const auidoControl = new AuidoControl();

export class AudioPlayer {
  private id: string;
  private url: string;
  private lastPauseTime?: number;
  onEnded?: () => void;
  onPause?: () => void;
  onError?: () => void;
  state: "play" | "pause" | "stop" = "stop";
  constructor(url: string) {
    this.id = uuidv4();
    taskID[this.id] = this;
    this.url = url;
  }
  play() {
    auidoControl.play(this.id, this.url);
    this.state = "play";
  }
  pause() {
    this.lastPauseTime = auidoControl.getCurrentTime(this.id);
    this.state = "pause";
    auidoControl.stop(this.id);
  }
  continue() {
    auidoControl.play(this.id, this.url, this.lastPauseTime);
    this.state = "play";
  }
  destroy() {
    this.state = "stop";
    auidoControl.stop(this.id);
    delete taskID[this.id];
  }
  _onPause(curTime: number) {
    this.lastPauseTime = curTime;
    this.state = "pause";
    this.onPause?.();
  }
}
