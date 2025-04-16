#pragma once

#include <fmod.hpp>
#include <fmod_common.h>
#include <filesystem>

namespace DemoEngine
{
	class AudioPlayer
	{
	public:
		AudioPlayer();
		~AudioPlayer() { StopSound(); };

		void Init(std::filesystem::path path = std::filesystem::current_path() / "src/Audio/Files/Startup.mp3");
		void Close(std::filesystem::path path = std::filesystem::current_path() / "src/Audio/Files/Close.mp3");
		void StopSound();

	private:
		FMOD::System* System = nullptr;
		FMOD::Sound* Sound = nullptr;
		FMOD::Channel* Channel = nullptr;
	};
}
