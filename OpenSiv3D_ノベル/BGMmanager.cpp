#include "BGMmanager.hpp"

void BGMmanager::init(const String& name)
{
	AudioAsset(name).stop();
	AudioAsset(name).play(MixBus0, FadeTime);
	current = name;
}

void BGMmanager::Change(const String& name)
{
	if (current.isEmpty())
	{
		current = name;
	}
	AudioAsset(current).stop(FadeTime);
	AudioAsset(name).play(MixBus0, FadeTime);
	current = name;
}

void BGMmanager::Stop()
{
	AudioAsset(current).stop(FadeTime);
}

String BGMmanager::GetCurrentBGM()
{
	return current;
}


