# pragma once
# include "Common.hpp"

class BGMmanager
{
public:

	void init(const String& name);

	void Change(const String& name);

	void Stop();

	String GetCurrentBGM();


private:
	AudioGroup group;

	String current, before; //現在再生中の曲
	HashTable<String, int32> table; // 曲の名前とグループのインデックス

	// フェード時間
	const Duration FadeTime = 1.0s;
};
