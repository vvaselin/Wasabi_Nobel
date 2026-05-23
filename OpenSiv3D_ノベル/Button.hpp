# pragma once
# include "Common.hpp"

class WhiteButton
{
public:

	WhiteButton(const Point& point, const Font& font, const String& text);

	bool update();

	void cg_enabled(bool& enabled);

private:
	Point m_point;
	Font m_font;
	String m_text;
	bool m_enabled;

	double t = 0.0;
	RoundRect m_rect{ m_point, 300, 100, 10 };

	bool over = false;
};
