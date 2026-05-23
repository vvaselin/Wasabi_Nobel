# include "TileButton.hpp"

TileButton::TileButton(const Icon& icon, int32 iconSize, const Font& font, double fontSize, const String& text, const RectF& rect, const Palette& palette)
	: TileButton{ Texture{ icon, iconSize }, iconSize, font, fontSize, text, rect, palette } {}

TileButton::TileButton(const TextureRegion& textureRegion, int32 iconSize, const Font& font, double fontSize, const String& text, const RectF& rect, const Palette& palette)
	: m_icon{ textureRegion }
	, m_iconSize{ iconSize }
	, m_font{ font }
	, m_fontSize{ fontSize }
	, m_text{ text }
	, m_rect{ rect }
	, m_palette{ palette } {}

bool TileButton::update()

{
	const bool mouseOver = m_rect.mouseOver();

	bool pushed = false;

	if (mouseOver)
	{
		if (!over)
		{
			AudioAsset(U"マウスオーバー").playOneShot(MixBus1);
			over = true;
		}
		Cursor::RequestStyle(CursorStyle::Hand);
	}
	else
	{
		over = false;
	}

	if (not m_pressed)
	{
		if (m_rect.leftClicked())
		{
			m_pressed = true;
		}
	}
	else
	{
		if (m_rect.leftReleased())
		{
			AudioAsset(U"選択").playOneShot(MixBus1);
			m_pressed = false;
			pushed = true;
		}
		else if (not m_rect.mouseOver())
		{
			m_pressed = false;
		}
	}

	m_transitionPressed.update(m_pressed);

	return pushed;
}

void TileButton::draw() const
{
	const double t = m_transitionPressed.value();

	const Transformer2D transform{ Mat3x2::Scale((1 + t * 0.06), m_rect.center()) };

	// タイル
	{
		m_rect.draw(m_palette.tileColor1.lerp(m_palette.tileColor2, t));

		m_rect.stretched(Math::Lerp(-InnerBorderMargin, 0, t))
			.drawFrame(2, (1.0 + t * 2.0), m_palette.borderColor1.lerp(m_palette.borderColor2, t));
	}

	// アイコン
	{
		m_icon
			.drawAt(m_rect.getRelativePoint(0.5, 0.4), m_palette.tileColor2.lerp(m_palette.tileColor1, t));
	}

	// ラベル
	{
		m_font(m_text)
			.drawAt(m_fontSize, m_rect.getRelativePoint(0.5, 0.8), m_palette.tileColor2.lerp(m_palette.tileColor1, t));
	}
}

String TileButton::getText() const
{
	return m_text;
}
