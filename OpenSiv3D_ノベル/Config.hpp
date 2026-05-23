# pragma once
# include "Common.hpp"
# include "Button.hpp"

class Slider {
public:

	//double 初期値(0～1),RectF 配置する四角形
	Slider(double value, const RectF& rect);

	//返り値はスライダーの値
	double update();

	void draw(const ColorF& color)const;

	void setValue(const double& value_);

	//0～1を返す
	double getValue();

	//操作中ならtrue
	bool isActive();

private:
	double value = 1.0;

	RectF rect;

	bool activeFlg = false;
};

class Config : public App::Scene
{
public:
	Config(const InitData& init);

	void update() override;

	void draw() const override;
private:

	FilePath m_path = U"./Config/config.json";

	double master_v = 1.0;
	double bgm_v = 0.5;
	double se_v = 0.5;

	double ms_add = 0.0;
	double auto_w = 0.5;

	Slider Masterslider{ master_v,RectF{Arg::center = Vec2{300, 400},500,10} };
	Slider BGMslider{ bgm_v,RectF{Arg::center = Vec2{300, 600},500,10} };
	Slider SEslider{ se_v,RectF{Arg::center = Vec2{300, 800},500,10} };

	Slider MSslider{ 0.5,RectF{Arg::center = Vec2{1100, 400},500,10} };
	Slider AUTOslider{ 0.5,RectF{Arg::center = Vec2{1100, 600},500,10} };

	const Font TitleFont{ 200 };
	const Font b_font{ 35 };

	const Font c_font{ FontMethod::MSDF,50 };

	const Font d_font{ 40 };


	WhiteButton Title{ Point{900,100},b_font,U"Title" };
	WhiteButton Story{ Point{ 1500,100 }, b_font, U"Story" };
	WhiteButton Reset{ Point{ 1500,800 }, c_font, U"初期設定に戻す" };
};
