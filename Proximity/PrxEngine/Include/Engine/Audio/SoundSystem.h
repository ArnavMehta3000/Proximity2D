#pragma once
#include "../External/DXTK/Audio/Audio.h"

namespace Proximity::Audio
{
	struct AudioSource;

	class SoundSystem : public Utils::IService
	{
	public:
		SoundSystem();
		~SoundSystem();

		const DirectX::AudioEngine* GetEngine() const noexcept;
		inline bool IsActive() const noexcept { return m_active; }

		Audio::AudioSource* CreateSource(const std::string_view name, const std::string& filename, bool createInstance = false);

		void Update() const noexcept;
		inline void Shutdown() { m_active = false; }

		void Suspend() const noexcept;
		void Resume() const noexcept;

		float GetMasterVolume() const noexcept;
		void SetMasterVolume(float volume) const noexcept;

		void SetReverb(DirectX::AUDIO_ENGINE_REVERB reverb) const noexcept;
		void SetMasteringLimit(Math::I32 release, Math::I32 loudness);

		auto GetMasterOutputFormat() const noexcept;

		Math::I32 GetOutputSampleRate() const noexcept;
		Math::U32 GetOutputChannelCount() const noexcept;

		DirectX::AudioStatistics GetStats() const noexcept;

	private:
		std::unique_ptr<DirectX::AudioEngine> m_audioEngine;
		bool m_active;
	};


	struct AudioSource
	{
		friend class SoundSystem;

		AudioSource()
			:
			m_soundEffect(nullptr),
			m_effectInstance(nullptr),
			m_volume(1.0f),
			m_pitch(1.0f),
			m_pan(1.0f)
		{}

		std::string Name;
		std::string Filename;


		bool HasInstance() const noexcept;
		bool InUse() const noexcept;
		bool IsLooped() const noexcept;

		void DestroyInstance() const noexcept;
		inline bool CreateIntance() const noexcept { return TryCreateInstance(); }
		inline float GetVolume()    const noexcept { return m_volume; }
		inline float GetPitch()	    const noexcept { return m_pitch; }
		inline float GetPan()	    const noexcept { return m_pan; }

		const DirectX::SoundState GetState()       const noexcept;
		const WAVEFORMATEX* GetFormat()            const noexcept;
		Math::U32           GetChannelCount()      const noexcept;
		Math::U64           GetSampleSizeInBytes() const noexcept;
		Math::U64           GetSampleDuration()    const noexcept;
		Math::U64           GetSampleDurationMS()  const noexcept;

		void Play(bool loop) const noexcept;
		void PlayOneShot() const noexcept;
		void PlayOneShot(float volume, float pitch, float pan) const noexcept;
		void Pause() const noexcept;
		void Stop(bool immediate) const noexcept;

		void SetVolume(float volume) const noexcept;
		void SetPitch(float pitch) const noexcept;
		void SetPan(float pan) const noexcept;

	private:
		inline bool TryCreateInstance() const noexcept
		{
			if (!m_soundEffect)
			{
				PRX_LOG_ERROR("Failed to create instance. Internal sound effect is nullptr");
				return false;
			}

			// Create instance if its not created already
			if (!m_effectInstance)
			{
				m_effectInstance.reset(m_soundEffect->CreateInstance().release());
			}

			return true;
		}

	private:
		mutable std::unique_ptr<DirectX::SoundEffect>         m_soundEffect;
		mutable std::unique_ptr<DirectX::SoundEffectInstance> m_effectInstance;

		mutable float m_volume;
		mutable float m_pitch;
		mutable float m_pan;
	};
}