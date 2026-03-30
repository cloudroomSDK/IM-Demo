import { v4 as uuidv4 } from "uuid";

const taskID: Record<string, AudioPlayer> = {};

class AuidoControl {
  private player?: HTMLAudioElement;
  private curPlayID?: string;
  private curPriority = 0;
  private curCanPause: boolean = false;
  constructor() {}
  create() {
    if (!this.player) {
      this.player = document.createElement("audio");
      this.player.onended = this.ended.bind(this);
      this.player.onerror = this.error.bind(this);
      document.body.appendChild(this.player);
    }
  }
  play(
    id: string,
    url: string,
    priority: number,
    curCanPause: boolean,
    loop: boolean,
    lastPauseTime = 0,
  ) {
    if (this.curPlayID) {
      if (priority < this.curPriority) return;

      if (this.curCanPause) {
        taskID[this.curPlayID]._onPause(~~this.player!.currentTime);
      } else {
        taskID[this.curPlayID].destroy();
      }
    }

    this.create();
    this.curPlayID = id;
    this.player!.loop = loop;
    this.player!.src = url;
    this.curPriority = priority;
    this.curCanPause = curCanPause;
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
    this.curPriority = 0;
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
  private canPause: boolean; //被打断时允许暂停，可继续播放
  private priority: number; //声音优先级，低级别会被高级别打断并取代
  private loop: boolean; //声音优先级，低级别会被高级别打断并取代
  onEnded?: () => void;
  onPause?: () => void;
  onError?: () => void;
  state: "play" | "pause" | "stop" = "stop";
  constructor(
    url: string,
    {
      priority = 0,
      canPause = false, //声音可以被暂停
      loop = false, //循环播放
    }: { priority?: number; canPause?: boolean; loop?: boolean } = {},
  ) {
    this.id = uuidv4();
    taskID[this.id] = this;
    this.url = url;
    this.priority = priority;
    this.canPause = canPause;
    this.loop = loop;
  }
  play() {
    auidoControl.play(
      this.id,
      this.url,
      this.priority,
      this.canPause,
      this.loop,
    );
    this.state = "play";
  }
  pause() {
    this.lastPauseTime = auidoControl.getCurrentTime(this.id);
    this.state = "pause";
    auidoControl.stop(this.id);
  }
  continue() {
    auidoControl.play(
      this.id,
      this.url,
      this.priority,
      this.canPause,
      this.loop,
      this.lastPauseTime,
    );
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
