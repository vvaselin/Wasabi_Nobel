# pragma once
# include "Common.hpp"

//集中線エフェクト
class LineEffect
{
public:
	void init();

	void draw();

private:
	Ellipse target{ Scene::Center(), 800, 800 };
	Rect outer = Scene::Rect();
	double minThickness = 3.0, maxThickness = 30.0;
	double lineCount = 50;
	double offsetRange = 50.0;

	SaturatedLinework<Ellipse> linework{ target, outer };
};

