# pragma once
# include "Common.hpp"

class TileButton
{
public:

	struct Palette
	{
		ColorF tileColor1;

		ColorF tileColor2;

		ColorF borderColor1;

		ColorF borderColor2;
	};

	TileButton() = default;

	TileButton(const Icon& icon, int32 iconSize, const Font& font, double fontSize, const String& text, const RectF& rect, const Palette& palette);

	// Texture からアイコンを作成
	TileButton(const TextureRegion& textureRegion, int32 iconSize, const Font& font, double fontSize, const String& text, const RectF& rect, const Palette& palette);


	bool update();

	void draw() const;

	String getText() const;

private:

	static constexpr double InnerBorderMargin = 3.0;

	TextureRegion m_icon;

	int32 m_iconSize = 0;

	Font m_font;

	double m_fontSize = 0;

	String m_text;

	RectF m_rect;

	Transition m_transitionPressed{ 0.09s, 0.12s };

	Palette m_palette;

	bool m_pressed = false;

	bool over = false;
};
