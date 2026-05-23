# pragma once
# include "Common.hpp"

//メッセージボックス
class MessageBox//メッセージボックス
{
public:
	void setText(const String& name, const String& text);

	void addText(const String& text);

	void draw(bool& clicked, const double& speed);

	bool isReading();

	void SwitchOFF();

	void SwitchON();

	bool getEndFlag() const;

private:
	bool isClicked = false;// 全文字表示用クリック
	const Font mainFont{ 50 }, rubyFont{ 25 };

	const double RubyYOffset = 20;
	Vec2 SpeakerPos{ 200, 710 };
	Vec2 ShadowPos{ 204, 714 };

	String speaker, message, r_tmp, tmp; //セリフの主, 元の文章, ルビの文字列, ルビを抜いた文章
	const String partition{ U"{|}" }; // ルビ区切る用
	Array<String> parent;//ルビを振られる文字列
	Ruby ruby;//ルビ用文字列
	Array<Ruby> RubyList;
	int32 indexBegin, indexEnd; //ルビの始まりと終わりのインデックス

	Stopwatch m_stopwatch;//一文字ずつ表示様ストップウォッチ

	bool Reading = false;

	const String nLine = U"[r]";

	bool messageEnd = false;

	int32 countStart = 0;

	int32 i = 0, j = 0; //i:何番目の文字を処理しているか。 j:改行してから何文字目か
};
