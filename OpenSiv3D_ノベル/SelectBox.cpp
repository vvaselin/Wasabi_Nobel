# include "SelectBox.hpp"

//選択肢の登録
void SelectBox::setSelects(const Array<String>& message)
{
	status = 0;
	selects.clear();
	selects = message;
}
//描画
void SelectBox::draw()
{
	if (KeyUp.down() || KeyDown.down() || KeyLeft.down() || KeyRight.down())
	{
		++posnum %= pos.size();
		Cursor::SetPos(pos[posnum]);
		over = false;
	}

	if (status == 0) //フェードイン
	{
		double alpha = m_stopwatch.sF() * 5;
		if (!m_stopwatch.isRunning()) m_stopwatch.start();
		if (alpha >= 0.7)
		{
			m_stopwatch.reset();
			status = 1;
		}
		rect1.draw(ColorF{ c1 , alpha });
		s_font(selects[0]).drawAt(pos[0], ColorF{ 0, alpha });
		rect2.draw(ColorF{ c1 , alpha });
		s_font(selects[1]).drawAt(pos[1], ColorF{ 0, alpha });
	}
	else if (status == 2) //フェードアウト
	{
		double alpha = m_stopwatch.sF() * 5;
		if (!m_stopwatch.isRunning()) m_stopwatch.start();
		if (alpha >= 0.7)
		{
			m_stopwatch.reset();
		}
		rect1.draw(ColorF{ c1 , 0.7 - alpha });
		s_font(selects[0]).drawAt(pos[0], ColorF{ 0, 0.7 - alpha });
		rect2.draw(ColorF{ c1 , 0.7 - alpha });
		s_font(selects[1]).drawAt(pos[1], ColorF{ 0, 0.7 - alpha });
	}
	else if (status == 1) //通常表示 status=1
	{
		rect1.draw(ColorF{ c1, 0.7 });
		s_font(selects[0]).drawAt(pos[0], ColorF{ 0 });
		rect2.draw(ColorF{ c1, 0.7 });
		s_font(selects[1]).drawAt(pos[1], ColorF{ 0 });
	}
}
//どれを選択中か
void SelectBox::isSelected()
{
	if (rect1.mouseOver())
	{
		if (!over)
		{
			AudioAsset(U"マウスオーバー").playOneShot(MixBus1);
			over = true;
		}
		Cursor::RequestStyle(CursorStyle::Hand);
		num = 1;
	}
	else if (rect2.mouseOver())
	{
		if (!over)
		{
			AudioAsset(U"マウスオーバー").playOneShot(MixBus1);
			over = true;
		}

		Cursor::RequestStyle(CursorStyle::Hand);
		num = 2;
	}
	else
	{
		over = false;
		num = 0;
	}
}

int32 SelectBox::isClicked()
{
	if (rect1.leftClicked() || (num == 1 && KeyEnter.down()))
	{
		AudioAsset(U"選択").playOneShot(MixBus1);
		status = 2;
		return num;
	}
	else if (rect2.leftClicked() || (num == 2 && KeyEnter.down()))
	{
		AudioAsset(U"選択").playOneShot(MixBus1);
		status = 2;
		return num;
	}
	return -1;
}
