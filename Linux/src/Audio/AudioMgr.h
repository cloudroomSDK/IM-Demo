#ifndef AudioMgr_H
#define AudioMgr_H

#include <QAudio>

// WAV struct
struct WAVFILEHEADER
{
    // RIFF header
    char RiffName[4];
    unsigned int nRiffLength;
 
    // data type flag
    char WavName[4];
 
    // format
    char FmtName[4];
    unsigned int nFmtLength;
 
    // audio formats
    unsigned short nAudioFormat;
    unsigned short nChannleNumber;
    unsigned int nSampleRate;
    unsigned int nBytesPerSecond;
    unsigned short nBytesPerSample;
    unsigned short nBitsPerSample;
 
    // data
    char    DATANAME[4];
    unsigned int   nDataLength;
};

class QAudioOutput;
class QAudioDeviceInfo;
class WavPlayer : public QObject
{
	Q_OBJECT

public:
	WavPlayer();
	~WavPlayer();

	void startPlay(const QAudioDeviceInfo &aInfo, const QString &wavFilename, bool loop = true);
	void stopPlay();

signals:
	void s_startPlay(const QAudioDeviceInfo &aInfo, const QString &wavFilename, bool loop);
	void s_stopPlay();

private slots:
	void slot_startPlay(const QAudioDeviceInfo &aInfo, const QString &wavFilename, bool loop);
	void slot_stopPlay();
	void slot_stateChanged(QAudio::State aState);

private:
	QFile*			_wavFile;
	QAudioOutput*	_playDev;
	bool			_playing;
	bool			_loop;
	qint64			_bufferSize;
};


class AudioMgr : public QObject
{
	Q_OBJECT

	AudioMgr();
	~AudioMgr();
public:
	static AudioMgr& instance();

	void init();
	void uninit();
	bool isInited() { return _bInited; }

	void setSpkVolume(qreal value);
	void setMicVolume(qreal value);
	qreal getSpkVolume();
	qreal getMicVolume();

	//播放音频（使用当前选择的设备）
	void startPlaySound(const QString &wavFile, bool bLoop = true);
	void stopPlaySound();


private:
	QThread* _playThread;
	bool	_bInited;

	WavPlayer*		_wavPlayer;
};

#define g_AudioMgr (AudioMgr::instance())

#endif // AudioMgr_H
