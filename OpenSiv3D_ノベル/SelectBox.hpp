# pragma once
# include "Common.hpp"

//セレクトボックス
class SelectBox
{
public:
	//選択肢の登録
	void setSelects(const Array<String>& message);

	//描画
	void draw();

	//どれを選択中か
	void isSelected();

	int32 isClicked();

private:
	Color c1 = Palette::Lightblue;
	Array<String> selects;
	const Array<Point> pos{ {900, 300}, {900, 600} };
	Rect rect1{ Arg::center = pos[0], 800, 150 };
	Rect rect2{ Arg::center = pos[1], 800, 150 };
	Font s_font{ 40 };
	int32 num = 0;
	//
	size_t posnum = 0;

	//フェードイン・アウト用のストップウォッチと状態遷移
	Stopwatch m_stopwatch;
	int32 status = 0;

	bool over = false;;
};

