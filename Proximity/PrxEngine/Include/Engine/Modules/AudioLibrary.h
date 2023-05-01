#pragma once
#include "Engine/Audio/SoundSystem.h"

namespace Proximity::Modules
{
	class AudioLibrary : public Utils::IService
	{
		using AudioMap = std::unordered_map<std::string, std::shared_ptr<Audio::AudioSource>>;
	public:
		AudioLibrary() = default;

		void InitProjectLib();
		void Refresh();

		void AddAudioSource(const std::shared_ptr<Audio::AudioSource>& src);
		void RemoveAudioSource(const std::string& audioName);
		bool Exists(const std::string& audioName);
		std::shared_ptr<Audio::AudioSource> Get(const std::string audioName);

		inline Math::U64 Count()  const noexcept             { return m_audios.size(); }
		inline const AudioMap& GetMap() const noexcept             { return m_audios; }
		inline void UpdateSelected(std::string_view audio)         { m_selectedAudio = audio; OnAudioSelected(audio); }
		inline const std::string& GetSelectedName() const noexcept { return m_selectedAudio; }

	public:
		Utils::Action<std::string_view> OnAudioSelected;

	private:
		std::string m_selectedAudio;
		AudioMap m_audios;
	};
}