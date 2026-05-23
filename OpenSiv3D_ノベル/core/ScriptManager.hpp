# pragma once
# include "core/Common.hpp"
# include "rendering/LineEffect.hpp"
# include "rendering/Chara.hpp"
# include "ui/MessageBox.hpp"
# include "ui/SelectBox.hpp"
# include "rendering/BackGround.hpp"
# include "audio/BGMmanager.hpp"
# include "rendering/CGmanager.hpp"
# include "rendering/BackLog.hpp"

// 実行モード
enum class ExecMode
{
	Normal, // 通常再生（1行ずつ、描画・SE等の副作用あり）
	Quick   // 高速再生（セーブ/ロード復元用、副作用を抑制）
};

//スクリプトの読み込み
class ScriptManager
{
public:
	void setFile(const FilePath& f, const size_t& n);

	void scriptUpdate();

	Array<String> UpdateQuick(const int32& line);

	void draw(const bool& text, const bool& menue, const double& speed, const double& autospeed);

	void StopBGM();

	String getBGM();

	bool getLogGen() const;

	String getLog();

	bool getEnd() const;

	void setAuto(const bool& Flag);
	bool getAuto() const;

	void setSkip(const bool& Flag);
	bool getSkip() const;

	//セーブデータ用
	String getFile() const;//今進めているスクリプトファイルを返す
	int32 getLine() const; //何行目か

private:

	// === 統合されたコア処理 ===

	void skipBlankLines();

	void executeCommand(Array<String>& order, ExecMode mode, int32 targetLine = -1);

	void processDialogue(ExecMode mode, Array<String>* outLog = nullptr);

	// === メンバ変数 ===

	Array<String> messages;
	Array<Chara> characters;
	String Log;
	MessageBox m_box;
	SelectBox s_box;
	BackGround bg;
	BGMmanager bgm;
	LineEffect l_effect;
	bool LFflag = false;
	CGmanager cg;

	size_t scriptLine = 0;
	int32 saveLine = 0;
	HashTable<String, int32> Orders =
	{
		{U"new_chara",1},
		{U"change",2},
		{U"visible",3},
		{U"select",4},
		{U"jump",5},
		{U"l_effect",6},
		{U"layer",7},
		{U"move",8},
		{U"end",9},
		{U"shake",10},
		{U"bg_image",11},
		{U"camera", 12},
		{U"bgm", 13},
		{U"script", 14},
		{U"se", 15},
		{U"cg", 16},
		{U"movie",17},
	};
	const String::value_type atMark{ U'@' };
	const String::value_type colon{ U':' };
	const String::value_type landmark{ U'*' };
	const String::value_type dot{ U'.' };

	bool selecting = false;
	Array<String> targets;

	FilePath ScriptFile;

	bool LogGen = false;

	bool LogDel = false;
	Array<String> DelText;

	bool Start = false;
	bool End = false;

	bool SE_flag = false;
	String SE_name;

	Transition transition{ 0.1s, 0.3s };
	bool shake = false;

	bool Auto_ = false;
	const double spawnTime = 1.0;
	double accumulator = 0.0;

	bool Skip_ = false;
};
