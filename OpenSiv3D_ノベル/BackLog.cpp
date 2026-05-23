# include "BackLog.hpp"

//コンストラクタ
BackLog::BackLog(const SasaGUI::ScrollBar& vbar)
	:vbar{ vbar }
{

}
//バックログのメッセージを追加
void BackLog::addMessage(const String& message)
{
	
	const String::value_type dot{ U'.' }; //追加のセリフ

	if (message[0]==dot)
	{
		String s = AddLog(messages.back(), message);
		s.pop_front();
		messages.back() += s;
	}
	else
	{
		String s = GeneLog(message);
		messages << s;
	}
}
Array<String> BackLog::getMessages()
{
	return messages;
}
//バックログの領域を更新する
void BackLog::length_update()
{
	length = 0;

	for (const auto& s : messages)
	{
		length += (font(s).region().h + 100);
	}
}
//バックログのカーソルバーが一番下にあるか
bool BackLog::ReadLog()
{
	return (vbar.value() + rect.h + 5 >= vbar.maximum());
}
//バックログの情報更新
void BackLog::update()
{
	vbar.updateLayout({
			rect.w - SasaGUI::ScrollBar::Thickness, 0,
			SasaGUI::ScrollBar::Thickness,
			rect.h - SasaGUI::ScrollBar::Thickness
		});
	vbar.updateConstraints(0.0, length, rect.h);
	vbar.scroll(Mouse::Wheel() * 400);
	vbar.update();
}
//バックログ一番下に移動
void BackLog::ShowNew()
{
	if (length >= rect.h) { vbar.moveTo(vbar.maximum()); }
}
//メッセージ描画
void BackLog::draw()
{
	Title(U"Back Log").draw(Point{100, 50},ColorF{0.0});

	button.update();
	button.draw();

	const ScopedViewport2D viewport{ rect };

	{
		Transformer2D t(Mat3x2::Translate(0, -vbar.value()));
		
		int32 i = 0;
		for (const auto& s : messages)
		{
			i += (font(s).draw(Vec2{ 100,100 + i }, Palette::Black).h + 100);
		}
	}

	if (length >= rect.h)
	{
		vbar.draw();
	}
}

void BackLog::popback_Log()
{
	messages.pop_back();
}

bool BackLog::buttonClicked()
{
	return button.update();
}
