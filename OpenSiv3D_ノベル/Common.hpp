#pragma once
# include <Siv3D.hpp> // OpenSiv3D v0.6.11

// シーンの名前
enum class State
{
	Title,
	Game,
	Load,
	Save,
	Config,
};

//構造体
//ルビ
struct Ruby
{
	int32 indexBegin; //テキスト中のルビの始まりのインデックス
	int32 indexEnd; //ルビの終わりのインデックス
	String text; //ルビの内容
};

//GameData
struct GameData
{
	//セーブデータ
	FilePath File = Resource(U"script/Script.txt"); // 進行中のスクリプトファイルのパス

	FilePath NewGameFile = Resource(U"script/Script.txt");

	int32 ScriptLine = 0; //何行目か

	//コンフィグ

	double ms_add = 0.0;

	double auto_w = 1.0;

	bool TrueEnd = false;
};


//関数
//ゲーム初期化関数
void InitGame();
//ルビ振り用関数
void DrawTextWithRuby(const Vec2& basePos, const Font& mainFont, const Font& rubyFont, const String& text, const Array<Ruby>& rubyList, double rubyYOffset);
//スクリプト解析用(position)
Vec2 posJudge(const String& pos);
//スクリプト解析用(scale)
double scaleJudge(const String& scale);
//TextureAssetから探す
String SerchAsset(const String& line);
//メッセージボックス端の円
void messageCircle();
//画面内でクリックされたか(メッセージボックス更新用)
bool winClick();
//バッグログ用の文生成
String GeneLog(const String message);
String AddLog(String& s ,const String message);


String ResourceExtension(const FilePathView path);
String ResourceBaseName(const FilePathView path);

//Asset登録
void AssetInit();

bool Button(const Point& point, const Font& font, const String& text, bool enabled);

//シーン管理
using App = SceneManager<State, GameData>;

