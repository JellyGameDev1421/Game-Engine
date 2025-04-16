#include "DemoEngine_PCH.h"
#include "AudioPlayer.h"
#include <iostream>

namespace DemoEngine
{
	AudioPlayer::AudioPlayer()
	{
		FMOD_RESULT result = FMOD::System_Create(&System);
		int driverCount = 0;

		System->getNumDrivers(&driverCount);
		if (driverCount == 0)
		{
			System->release();
			System = nullptr;
			return;
		}

		result = System->init(512, FMOD_INIT_NORMAL, nullptr);

		if (result != FMOD_OK)
		{
			System->release();
			System = nullptr;
			return;
		}

	}

	void AudioPlayer::Init(std::filesystem::path path)
	{

		System->createSound(path.string().c_str(), FMOD_3D, nullptr, &Sound);
		Sound->setMode(FMOD_3D_INVERSETAPEREDROLLOFF);
		System->playSound(Sound, nullptr, false, &Channel);
		Channel->setMode(FMOD_3D_INVERSETAPEREDROLLOFF);

		bool isPlaying = true;
		while (isPlaying)
		{
			System->update();

			bool playing = false;
			if (Channel)
			{
				Channel->isPlaying(&playing);
				isPlaying = playing;

			}
			else
			{
				isPlaying = false;
			}
		}
	}

	void AudioPlayer::Close(std::filesystem::path path) 
	{
		System->createSound(path.string().c_str(), FMOD_3D, nullptr, &Sound);
		Sound->setMode(FMOD_3D_INVERSETAPEREDROLLOFF);
		System->playSound(Sound, nullptr, false, &Channel);
		Channel->setMode(FMOD_3D_INVERSETAPEREDROLLOFF);

		bool isPlaying = true;
		while (isPlaying)
		{
			System->update();

			bool playing = false;
			if (Channel)
			{
				Channel->isPlaying(&playing);
				isPlaying = playing;

			}
			else
			{
				isPlaying = false;
			}
		}
	}

	void AudioPlayer::StopSound()
	{
		if (Channel)
		{
			bool isPlaying = false;
			Channel->isPlaying(&isPlaying);
			if (isPlaying)
			{
				Channel->stop();
			}
		}
	}
}
