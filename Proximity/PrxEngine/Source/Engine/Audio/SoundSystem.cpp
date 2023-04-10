#include "enginepch.h"
#include "Engine/Audio/SoundSystem.h"

namespace Proximity::Audio
{
	SoundSystem::SoundSystem()
		:
		m_active(false)
	{
		using namespace DirectX;

		AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default | AudioEngine_UseMasteringLimiter | AudioEngine_UseMasteringLimiter;
#ifdef _DEBUG
		eflags |= AudioEngine_Debug;
#endif
		m_audioEngine = std::make_unique<DirectX::AudioEngine>(eflags);
		
		m_active = true;
	}

	SoundSystem::~SoundSystem()
	{
	}

	const DirectX::AudioEngine* SoundSystem::GetEngine() const noexcept
	{
		return m_audioEngine.get();
	}

	void SoundSystem::Update() const noexcept
	{
		if (!m_audioEngine->Update())
		{
			if (m_audioEngine->IsCriticalError())
			{
				MSG_BOX("Proximity sound system encountered a criticl error. Check audio device output connection!",
					"Proximity Sound System Critical Error!")
			}
		}
	}

	void SoundSystem::Suspend() const noexcept
	{
		m_audioEngine->Suspend();
	}
	
	void SoundSystem::Resume() const noexcept
	{
		m_audioEngine->Resume();
	}

	float SoundSystem::GetMasterVolume() const noexcept
	{
		return m_audioEngine->GetMasterVolume();
	}

	void SoundSystem::SetMasterVolume(float volume) const noexcept
	{
		m_audioEngine->SetMasterVolume(volume);
	}

	void SoundSystem::SetReverb(DirectX::AUDIO_ENGINE_REVERB reverb) const noexcept
	{
		m_audioEngine->SetReverb(reverb);
	}

	void SoundSystem::SetMasteringLimit(Math::I32 release, Math::I32 loudness)
	{
		m_audioEngine->SetMasteringLimit(release, loudness);
	}

	auto SoundSystem::GetMasterOutputFormat() const noexcept
	{
		return m_audioEngine->GetOutputFormat();
	}
	
	Math::I32 SoundSystem::GetOutputSampleRate() const noexcept
	{
		return m_audioEngine->GetOutputSampleRate();
	}
	
	Math::U32 SoundSystem::GetOutputChannelCount() const noexcept
	{
		return m_audioEngine->GetOutputChannels();
	}

	DirectX::AudioStatistics SoundSystem::GetStats() const noexcept
	{
		return m_audioEngine->GetStatistics();
	}




	void AudioSource::Play(bool loop) const noexcept
	{
		if (!TryCreateInstance())
		{
			PRX_LOG_ERROR("Trying to play audio source without effect initialized!");
			return;
		}
		m_effectInstance->Play(loop);
	}

	void AudioSource::PlayOneShot() const noexcept
	{
		if (!m_soundEffect)
		{
			PRX_LOG_ERROR("Trying to play one shot without effect initialized!");
			return;
		}

		m_soundEffect->Play();
	}

	void AudioSource::PlayOneShot(float volume, float pitch, float pan) const noexcept
	{
		if (!m_soundEffect)
		{
			PRX_LOG_ERROR("Trying to play one shot without effect initialized!");
			return;
		}

		m_soundEffect->Play(volume, pitch, pan);
	}
	
	void AudioSource::Pause() const noexcept
	{
		if (!TryCreateInstance())
		{
			PRX_LOG_ERROR("Trying to pause audio without internal instance!");
			return;
		}

		m_effectInstance->Pause();
	}
	
	void AudioSource::Stop(bool immediate) const noexcept
	{
		if (!TryCreateInstance())
		{
			PRX_LOG_ERROR("Trying to stop audio without internal instance!");
			return;
		}

		m_effectInstance->Stop();
	}
	
	void AudioSource::SetVolume(float volume) const noexcept
	{
		if (!TryCreateInstance())
		{
			PRX_LOG_ERROR("Trying to set audio source volume without internal instance!");
			return;
		}

		m_effectInstance->SetVolume(volume);
	}
	
	void AudioSource::SetPitch(float pitch) const noexcept
	{
		if (!TryCreateInstance())
		{
			PRX_LOG_ERROR("Trying to set audio source pitch without internal instance!");
			return;
		}

		m_effectInstance->SetVolume(pitch);
	}
	
	void AudioSource::SetPan(float pan) const noexcept
	{
		if (!TryCreateInstance())
		{
			PRX_LOG_ERROR("Trying to set audio source pan without internal instance!");
			return;
		}

		m_effectInstance->SetVolume(pan);
	}

	bool AudioSource::InUse() const noexcept
	{
		return (m_soundEffect) ? m_soundEffect->IsInUse() : false;
	}
	
	bool AudioSource::IsLooped() const noexcept
	{
		return (m_effectInstance) ? m_effectInstance->IsLooped() : false;
	}
	
	DirectX::SoundState AudioSource::GetState() const noexcept
	{
		return (m_effectInstance) ? m_effectInstance->GetState() : DirectX::SoundState::STOPPED;
	}
	
	Math::U32 AudioSource::GetChannelCount() const noexcept
	{
		return (m_effectInstance) ? m_effectInstance->GetChannelCount() : 0;
	}
	
	Math::U64 AudioSource::GetSampleSizeInBytes() const noexcept
	{
		return (m_soundEffect) ? m_soundEffect->GetSampleSizeInBytes() : 0;
	}
	
	Math::U64 AudioSource::GetSampleDuration() const noexcept
	{
		return (m_soundEffect) ? m_soundEffect->GetSampleDuration() : 0;
	}
	
	Math::U64 AudioSource::GetSampleDurationMS() const noexcept
	{
		return (m_soundEffect) ? m_soundEffect->GetSampleDurationMS() : 0;
	}
}