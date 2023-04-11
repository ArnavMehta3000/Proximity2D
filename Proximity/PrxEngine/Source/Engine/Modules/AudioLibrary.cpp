#include "enginepch.h"
#include "Engine/Modules/AudioLibrary.h"

namespace Proximity::Modules
{
	void AudioLibrary::InitProjectLib()
	{
		PRX_LOG_DEBUG("Initializing audio library for audio");
		auto& rootPath = DirectoryManager::s_appDirectories.AudioPath;
		using recursiveDirIter = std::filesystem::recursive_directory_iterator;
		auto ss = PRX_RESOLVE(Audio::SoundSystem);

		for (const auto& dir : recursiveDirIter(rootPath))
		{
			// Check if file is a wav file (get extension)
			if (dir.path().extension() == ".wav" || dir.path().extension() == ".WAV")
			{
				// TODO: Read this from audio data file
				auto name = DirectoryManager::GetFileNameFromDir(dir.path(), false);
				auto& path = dir.path();

				std::shared_ptr<Audio::AudioSource> ptr(ss->CreateSource(name, path.string().c_str()));
				AddAudioSource(ptr);
			}
		}

		PRX_LOG_DEBUG("Finished initializing audio library for audio");
	}

	void AudioLibrary::Refresh()
	{
		bool found = false;
		PRX_LOG_DEBUG("Refreshing audio library for audio");

		auto& rootPath = DirectoryManager::s_appDirectories.AudioPath;
		using recursiveDirIter = std::filesystem::recursive_directory_iterator;
		auto ss = PRX_RESOLVE(Audio::SoundSystem);

		for (const auto& dir : recursiveDirIter(rootPath))
		{
			auto name = DirectoryManager::GetFileNameFromDir(dir.path(), false);
			if (!Exists(name))
			{
				if (dir.path().extension() == ".wav" || dir.path().extension() == ".WAV")
				{
					auto& path = dir.path();

					std::shared_ptr<Audio::AudioSource> ptr(ss->CreateSource(name, path.string().c_str()));
					AddAudioSource(ptr);
				}
				found = true;
			}
			else
			{
				// No new image files
			}
		}

		if (found)
			PRX_LOG_DEBUG("Refreshed audio library - added/removed audio files");
		else
			PRX_LOG_DEBUG("Refreshed audio library - no changes");
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