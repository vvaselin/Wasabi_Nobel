# pragma once
# include "Common.hpp"
# include "SaveDataBox.hpp"
# include "Button.hpp"

class Save : public App::Scene
{
public:
	Save(const InitData& init);

	void update() override;

	void draw() const override;
private:
	Array<FilePath> savefiles;
	Array<SaveDataBox> savebox;

	const Font font{ FontMethod::MSDF, 35 };
	const double FontSize = 35;


	const SaveDataBox::palette_ palette_{
		.tileColor1 = ColorF{1.0} ,
		.tileColor2 = ColorF{ 0.0 },
		.borderColor1 = ColorF{ 0.0 }
	};

	const Font TitleFont{200};
	const Font b_font{ 35 };

	int32 page = 0;
	int32 pageMax = 1;
	Camera2D camera{ Scene::CenterF(), 1.0, CameraControl::None_};

	WhiteButton Title{ Point{900,100},b_font,U"Title" };
	WhiteButton Load{ Point{ 1200,100 }, b_font, U"Load" };
	WhiteButton Story{ Point{ 1500,100 }, b_font, U"Story" };
};
