#pragma once
# include "SasaGUI.hpp"
# include "TileButton.hpp"

class BackLog
{
public:
	//コンストラクタ
	BackLog(const SasaGUI::ScrollBar& vbar);
	//バックログのメッセージを追加
	void addMessage(const String& message);
	Array<String> getMessages();
	//バックログの領域を更新する
	void length_update();
	//バックログのカーソルバーが一番下にあるか
	bool ReadLog();
	//バックログの情報更新
	void update();
	//バックログ一番下に移動
	void ShowNew();
	//メッセージ描画
	void draw();

	void popback_Log();

	bool buttonClicked();

private:
	SasaGUI::ScrollBar vbar;

	Rect rect{0, 200, Scene::Width(), Scene::Height() - 200};

	const Font Title{ 100 };

	const Font font{ 50 };

	double length = 0;

	Array<String> messages;

	const Font menufont{ FontMethod::MSDF, 35, U"example/font/DotGothic16/DotGothic16-Regular.ttf" };
	const int32 IconSize = 100;
	const double FontSize = 35;
	const TileButton::Palette p_{
		.tileColor1 = Palette::Slateblue,
		.tileColor2 = ColorF{ 1.0 },
		.borderColor1 = ColorF{ 1.0 },
		.borderColor2 = Palette::Slateblue
	};

	RectF b_rect{Scene::Width()-200, 50, 100, 100};

	TileButton button{ 0xF17B4_icon,IconSize, menufont, FontSize, U"", b_rect, p_ };

	int32 i = 0, j = 0; //i:何番目の文字を処理しているか。 j:改行してから何文字目か
};
