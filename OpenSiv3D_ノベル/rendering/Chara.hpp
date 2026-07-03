# pragma once
# include "core/Common.hpp"
# include "FadeController.hpp"

class Chara
{
public:
	enum class MovePattern
	{
		Default,
		Step,
		Jump,
		BackLeft,
		BackRight,
	};

	struct MoveAnim
	{
		Vec2 from;
		Vec2 to;
		double duration = 0.5;
		double time = 0.0;
		bool active = false;
	};

	struct FadeTeleportAnim
	{
		Vec2 to;
		double duration = 0.8;
		double time = 0.0;
		bool active = false;
		bool switched = false;
	};

	Chara(const String& s_name, const String& portrait, const Vec2& pos, const double& scale, const bool& s_visible);
	Chara();

	void change(const String& portrait, const double& s_scale, const Vec2& pos);

	void setVisible(const bool& s_visible);

	void draw();

	bool updateMove();

	void moveTo(const Vec2& to, double duration, bool wait);

	void fadeTeleportTo(const Vec2& to, double duration, bool wait);

	void startMove(MovePattern pattern, bool wait);

	void applyMovePatternImmediate(MovePattern pattern);

	void setPositionImmediate(const Vec2& pos);

	void MoveReset();

	void changeFlag();

	bool judgeName(const String& s_name);

	String getName() const;

	bool isAnimating() const;

	bool isBlocking() const;

	bool isFading() const;

private:
	Vec2 getDrawPos(const Texture& tex, double s, const Vec2& footPos) const;

	String name;
	double scale = 0.6, scale_before = 0.6;
	String asset, asset_before;
	bool   visible = false, visible_before = false;
	Vec2   position, position_before;

	bool moving = false;
	bool blocking = false;
	double alpha = 1.0;

	MoveAnim moveAnim;
	FadeTeleportAnim fadeTeleportAnim;

	MovePattern m_pattern = MovePattern::Default;
	double patternTime = 0.0;
	double patternDuration = 0.3;

	// fadeIn=0.5s, fadeOut=0.8s, crossFade=0.5s（元の Transition{0.5s,0.8s} と同等）
	FadeController m_fade{ 0.5, 0.8, 0.5 };
};
