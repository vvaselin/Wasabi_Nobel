# include "Editor.hpp"

Editor::Editor(const InitData& init)
	: IScene{ init }
{
	AssetInit();
	Optional<FilePath> path_ = Dialog::OpenFile({ FileFilter::AllFiles() });
	s_manager.setFile(*path_, 0);

	TextReader reader{ *path_ };
	if (not reader) throw Error{ U"Failed to open script" };

	// 行の内容を読み込む変数
	String line;
	// 終端に達するまで 1 行ずつ読み込む
	while (reader.readLine(line))
	{
		listBoxState.items << line;
	}

	listBoxState.scroll = Largest<int32>;
}

void Editor::update()
{
	
	if (listBoxState.selectedItemIndex)
	{
		s_manager.UpdateQuick(*listBoxState.selectedItemIndex-1);
	}
	else
	{
		s_manager.UpdateQuick(0);
	}
	s_manager.StopBGM();
	s_manager.draw(true, true, getData().ms_add, getData().auto_w);

	SimpleGUI::ListBox(listBoxState, Vec2{ 1500, 200 }, 300, 200);

	bool enter = false;
	{
		const bool previous = textEditState.active;

		SimpleGUI::TextBox(textEditState, Vec2{ 1500, 560 }, 300);

		// エンターキーが押されてテキストボックスが非アクティブになったか
		enter = (previous && (textEditState.active == false) && textEditState.enterKey);
	}

	if (SimpleGUI::Button(U"Submit", Vec2{ 1800, 560 }, 100, true)
		|| enter)
	{
		if (listBoxState.selectedItemIndex)
		{
			listBoxState.items[*listBoxState.selectedItemIndex] = textEditState.text;
			s_manager.UpdateQuick(*listBoxState.selectedItemIndex-1);
		}
		else
		{
			// リストボックスにテキストを追加する
			listBoxState.items << textEditState.text;
			// 追加したテキストが見えるようにスクロール位置を最大にする（次の SimpleGUI::ListBox() で適切な値に補正される）
			listBoxState.scroll = Largest<int32>;
			s_manager.UpdateQuick(listBoxState.scroll-1);
		}

		// テキストボックスをクリアする
		textEditState.clear();

		// エンターキーが押されてテキストボックスが非アクティブになった場合、再びアクティブにする
		if (enter)
		{
			textEditState.active = true;
		}
	}
}

// 描画関数（オプション）
void Editor::draw() const
{

}
