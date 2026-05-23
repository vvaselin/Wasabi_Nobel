# pragma once
# include "Common.hpp"
# include "ScriptManager.hpp"
# include "Chara.hpp"
# include "SasaGUI.hpp"
# include "BackLog.hpp"
# include "TileButton.hpp"
# include "EndRoll.hpp"


class Game :public App::Scene
{
public:
	Game(const InitData& init);

	void update() override;

	void draw() const override;


private:

	ScriptManager s_manager;

	BackLog b_log;

	bool LogWatch = false;

	bool TextWatch = true;


	bool Menue = false;

	const Font menufont{ FontMethod::MSDF, 35, Resource(U"example/font/DotGothic16/DotGothic16-Regular.ttf") };
	const int32 IconSize = 100;
	const double FontSize = 35;
	const TileButton::Palette Palette{
		.tileColor1 = ColorF{ 0.2, 0.2, 0.8 },
		.tileColor2 = ColorF{ 1.0 },
		.borderColor1 = ColorF{ 1.0 },
		.borderColor2 = ColorF{ 0.5, 0.5, 1.0 }
	};

	Vec2 Standby{ 500, 0 };

	Array<TileButton> buttons = {
		{ 0xF0CFC_icon, IconSize, menufont, FontSize, U"Q.SAVE", Rect{1700, 40, 200}, Palette },
		{ 0xF0E28_icon, IconSize, menufont, FontSize, U"Q.LOAD", Rect{ 1700, 270, 200 }, Palette },
		{ 0xF14E7_icon, IconSize, menufont, FontSize, U"SAVE", Rect{1700, 500, 200}, Palette },
		{ 0xF0252_icon, IconSize, menufont, FontSize, U"LOAD", Rect{ 1700, 740, 200 }, Palette },
		{ 0xF1591_icon, IconSize, menufont, FontSize, U"AUTO", Rect{ 1450, 40, 200 }, Palette },
		{ 0xF04AC_icon, IconSize, menufont, FontSize, U"SKIP", Rect{ 1450, 270, 200 }, Palette },
		{ 0xF0214_icon, IconSize, menufont, FontSize, U"LOG", Rect{ 1450, 500, 200 }, Palette },
		{ 0xF1064_icon, IconSize, menufont, FontSize, U"CONFIG", Rect{ 1450, 740, 200 }, Palette },
	};

	HashTable<String, int32> MenueTable =
	{
		{ U"Q.SAVE", 1 },
		{ U"Q.LOAD", 2 },
		{ U"SAVE", 3 },
		{ U"LOAD", 4 },
		{ U"AUTO", 5 },
		{ U"SKIP", 6 },
		{ U"LOG", 7 },
		{ U"CONFIG", 8 },
	};

	Rect MenuON{ 1700, 0, 200, 1080 };
	Rect MenueOFF{ 1300, 0, 150, 1080 };
	Transition ts{ 1.0s,0.5s };
	Camera2D camera{ Scene::CenterF() - Standby, 1.0,CameraControl::None_ };
	bool clicked = false;

	int32 MenueNum = 0;

	EndRoll endroll{ Resource(U"クレジット.txt") };
	bool E_roll = false;
	Stopwatch e_watch;
	const double e_time = 3.0;
	double t = 0.0;

	Stopwatch m_watch;
	double m_t = 0.0;
	bool m_flag = false;
};
