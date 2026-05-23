# pragma once
# include "Common.hpp"
# include "Button.hpp"


class Title : public App::Scene
{
public:
	Title(const InitData& init);

	void update() override;

	void draw() const override;


private:
	FilePath m_path = U"./Config/config.json";

	const size_t index_max = 4;

	size_t index = index_max -1;

	Font b_font{ 35 };

	Texture title{ Resource(U"タイトル.png") };

	WhiteButton NEWGAME{ Point{ 200, 500 }, b_font, U"NEW GAME" };
	WhiteButton LOAD{ Point{ 200, 600 }, b_font, U"LOAD" };
	WhiteButton CONFIG{ Point{ 200, 700 }, b_font, U"CONFIG" };
	WhiteButton LICENSES{ Point{ 200, 800 }, b_font, U"LICENSES" };
};
