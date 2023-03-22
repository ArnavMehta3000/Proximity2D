#pragma once
#include <xaudio2.h>
#include <xaudio2fx.h>

// Taken from: https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

namespace Proximity::Audio
{
	class AudioEngine : public Utils::IService
	{
	public:
		AudioEngine() = default;
		~AudioEngine() = default;

		bool Init();
		void Shutdown();

	private:
		ComPtr<IXAudio2>        m_xAudio;
		IXAudio2MasteringVoice* m_masteringVoice;

	};
}