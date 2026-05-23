#include"Button.hpp"

WhiteButton::WhiteButton(const Point& point, const Font& font, const String& text)
	:m_point{ point }, m_font{ font }, m_text{ text }{}

bool WhiteButton::update()
{
	if (m_enabled && m_rect.mouseOver())
	{
		if (!over)
		{
			AudioAsset(U"マウスオーバー").playOneShot(MixBus1);
			over = true;
		}
		t = 0.4;
		Cursor::RequestStyle(CursorStyle::Hand);
	}
	else
	{
		over = false;
		t = 0.0;
	}

	if (m_enabled)
	{
		m_rect.draw(ColorF{ 1.0, 0.6 }.lerp(Palette::Royalblue, t));
		m_rect.drawFrame(0, 1, ColorF{ 0.0 });
		m_font(m_text).drawAt(40, (m_rect.x + m_rect.w / 2), (m_rect.y + m_rect.h / 2), ColorF{ 0.0 });
	}
	else
	{
		m_rect.draw(ColorF{ 0.5 });
		m_font(m_text).drawAt(40, (m_rect.x + m_rect.w / 2), (m_rect.y + m_rect.h / 2), ColorF{ 0.7 });
	}

	bool clicked = false;

	if ((m_enabled && (m_rect.leftClicked() || (m_rect.mouseOver() && KeyEnter.down()))))
	{
		clicked = true;
		AudioAsset(U"選択").playOneShot(MixBus1);
	}

	return clicked;
}

void WhiteButton::cg_enabled(bool& enabled)
{
	m_enabled = enabled;
}
