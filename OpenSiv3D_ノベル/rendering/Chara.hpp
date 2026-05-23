# pragma once
# include "core/Common.hpp"
# include "FadeController.hpp"

class Chara
{
public:
	Chara(const String& s_name, const String& portrait, const Vec2& pos, const double& scale, const bool& s_visible);
	Chara();

	void change(const String& portrait, const double& s_scale, const Vec2& pos);

	void setVisible(const bool& s_visible);

	void draw();

	void changeFlag();

	bool judgeName(const String& s_name);

	String getName() const;

	bool isAnimating() const;

private:
	String name;
	double scale = 0.6, scale_before = 0.6;
	String asset, asset_before;
	bool   visible = false, visible_before = false;
	Vec2   position, position_before;

	bool mooving = false;

	// fadeIn=0.5s, fadeOut=0.8s, crossFade=0.5s（元の Transition{0.5s,0.8s} と同等）
	FadeController m_fade{ 0.5, 0.8, 0.5 };
};
