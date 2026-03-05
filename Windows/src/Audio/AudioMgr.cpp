#include "stdafx.h"
#include "AudioMgr.h"
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QAudioInput>

AudioMgr &AudioMgr::instance()
{
	static AudioMgr ins;
	return ins;
}

WavPlayer::WavPlayer()
{
	_playDev = NULL;
	_wavFile = NULL;
	_playing = false;
	_loop = false;
	_bufferSize = 0;

	connect(this, &WavPlayer::s_startPlay, this, &WavPlayer::slot_startPlay);
	connect(this, &WavPlayer::s_stopPlay, this, &WavPlayer::slot_stopPlay);
}

WavPlayer::~WavPlayer()
{
	stopPlay();
}

void WavPlayer::startPlay(const QAudioDeviceInfo &aInfo, const QString &wavFilename, bool loop /*= true*/)
{
	emit s_startPlay(aInfo, wavFilename, loop);
}

void WavPlayer::slot_startPlay(const QAudioDeviceInfo &aInfo, const QString &wavFilename, bool loop)
{
	stopPlay();
	
	_loop = loop;
	_wavFile = new QFile(wavFilename);
    if(!_wavFile->open(QIODevice::ReadOnly))
    {
		MainLogDebug("failed to open wav file (%s)", wavFilename.toUtf8().constData());
        return;
    }
	_bufferSize = _wavFile->size();
 
    WAVFILEHEADER wavFileHeader;
    if(_wavFile->read((char*)&wavFileHeader, sizeof(wavFileHeader)) != sizeof(wavFileHeader))
    {
		MainLogDebug("failed to read wav file header (%s)", wavFilename.toUtf8().constData());
		_wavFile->close();
        return;
    }

    QAudioFormat format;
	format.setSampleRate(wavFileHeader.nSampleRate);
	format.setChannelCount(wavFileHeader.nChannleNumber);
	format.setSampleSize(wavFileHeader.nBitsPerSample);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);

	_playDev = new QAudioOutput(aInfo, format);
	connect(_playDev, &QAudioOutput::stateChanged, this, &WavPlayer::slot_stateChanged);
 	_playDev->setBufferSize(_bufferSize);
 	_playDev->start(_wavFile);
	_playing = true;
}

void WavPlayer::stopPlay()
{
	emit s_stopPlay();
}

void WavPlayer::slot_stopPlay()
{
	_loop = false;
	_bufferSize = 0;

	if(NULL != _playDev)
	{
		_playDev->stop();
		_playDev->deleteLater();
		_playDev = NULL;
	}
	if(NULL != _wavFile)
	{
		if(_wavFile->isOpen())
		{
			_wavFile->close();
		}
		_wavFile->deleteLater();
		_wavFile = NULL;
	}
	_playing = false;
}

void WavPlayer::slot_stateChanged(QAudio::State aState)
{
	if(aState == QAudio::IdleState)
    {
		if(_loop && (NULL != _wavFile) && (NULL != _playDev))
		{
			if(_wavFile->isOpen())
			{
				_wavFile->seek(0);
			}
			else
			{
	 			_wavFile->open(QIODevice::ReadOnly);
			}
			_playDev->reset();
			_playDev->setBufferSize(_bufferSize);
			_playDev->start(_wavFile);
		}
    }
}


AudioMgr::AudioMgr() : QObject()
{
	_bInited = false;

	_wavPlayer = new WavPlayer();
	_playThread = new QThread(this);
	_wavPlayer->moveToThread(_playThread);
	init();
}

AudioMgr::~AudioMgr()
{
	uninit();
	if(NULL != _wavPlayer)
	{
		_wavPlayer->deleteLater();
		_wavPlayer = NULL;
	}
}

void AudioMgr::init()
{
	_playThread->start();
	_bInited = true;
}

void AudioMgr::uninit()
{
	if(!_bInited)
	{
		MainLogDebug("AudioMgr::uninit, no need");
		return;
	}

	MainLogDebug("AudioMgr::uninit");
	stopPlaySound();

	_playThread->quit();
	_playThread->wait();
	_bInited = false;

	MainLogDebug("AudioMgr::uninit finish");
}

void AudioMgr::startPlaySound(const QString &wavFile, bool bLoop /*= true*/)
{
	MainLogDebug("startPlaySound: %s, bLoop:%d", wavFile.toUtf8().constData(), bLoop);
	QAudioDeviceInfo auInfo = QAudioDeviceInfo::defaultOutputDevice();
	if (NULL != _wavPlayer)
	{
		_wavPlayer->startPlay(auInfo, wavFile, bLoop);
	}
}

void AudioMgr::stopPlaySound()
{
	MainLogDebug("stopPlaySound");
	if (NULL != _wavPlayer)
	{
		_wavPlayer->stopPlay();
	}
}

void AudioMgr::setSpkVolume(qreal value)
{
	QAudioOutput output;
	output.setVolume(value);
}

void AudioMgr::setMicVolume(qreal value)
{
	QAudioInput input;
	input.setVolume(value);
}

qreal AudioMgr::getSpkVolume()
{
	QAudioOutput output;
	return output.volume();
}

qreal AudioMgr::getMicVolume()
{
	QAudioInput input;
	return input.volume();
}

