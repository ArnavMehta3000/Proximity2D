#include "enginepch.h"
#include "Engine/Modules/AudioLibrary.h"

namespace Proximity::Modules
{
	void AudioLibrary::InitProjectLib()
	{
		throw Proximity::Execptions::MethodNotImplemented("Cannot init from project lib");
	}

	void AudioLibrary::Refresh()
	{
		throw Proximity::Execptions::MethodNotImplemented("Refresh not implemented");
	}

	void AudioLibrary::AddAudioSource(const std::shared_ptr<Audio::AudioSource>& src)
	{
		auto& name = src->Name;
		if (Exists(name))
		{
			PRX_LOG_WARN("Audio source with the same name [%s] already exists in audio library", name.c_str());
			return;
		}
		m_audios[name] = src;
	}

	void AudioLibrary::RemoveAudioSource(const std::string& audioName)
	{
		if (m_audios.erase(audioName) == 1)
			PRX_LOG_INFO("Successfully removed audio [%s] from library", audioName.c_str());
		else
			PRX_LOG_INFO("Failed to remove audio [%s] from library", audioName.c_str());
	}

	bool AudioLibrary::Exists(const std::string& audioName)
	{
		return (m_audios.find(audioName) != m_audios.end());
	}

	std::shared_ptr<Audio::AudioSource> AudioLibrary::Get(const std::string audioName)
	{
		if (!Exists(audioName))
		{
			PRX_LOG_ERROR("Audios [%s] not found", audioName.c_str());
			return nullptr;
		}
		else
			return m_audios[audioName];
	}
}