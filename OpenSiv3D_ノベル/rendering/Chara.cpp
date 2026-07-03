# include "rendering/Chara.hpp"

namespace
{
	double EaseInOutCubicLocal(double t)
	{
		t = Clamp(t, 0.0, 1.0);
		return (t < 0.5) ? (4.0 * t * t * t) : (1.0 - Math::Pow(-2.0 * t + 2.0, 3.0) / 2.0);
	}
}

Chara::Chara(const String& s_name, const String& portrait, const Vec2& pos, const double& scale, const bool& s_visible)
	: name{ s_name }
	, asset{ portrait }
	, asset_before{ portrait }
	, visible{ s_visible }
	, position{ pos }
	, position_before{ pos }
	, scale{ scale }
	, scale_before{ scale }
{
	TextureAsset::Load(name + asset);
	// DeltaTime積算方式のため、コンストラクタ内で startFadeIn を呼んでも安全
	if (visible)
	{
		m_fade.startFadeIn();
	}
}

Chara::Chara()
	: name{ U"Unknown" }
	, asset{ U"" }
	, asset_before{ U"" }
	, visible{ false }
	, position{ posJudge(U"L") }
	, position_before{ posJudge(U"L") }
	, scale{ scaleJudge(U"小") }
	, scale_before{ scaleJudge(U"小") }
{
}

void Chara::change(const String& portrait, const double& s_scale, const Vec2& pos)
{
	if (portrait != U"NAN")
	{
		asset = portrait;
		TextureAsset::Load(name + asset);
	}
	if (s_scale != 0.0)         scale = s_scale;
	if (pos != Vec2{ 0,0 }) position = pos;

	if (visible && ((asset_before != asset) || (scale_before != scale) || (position_before != position)))
	{
		m_fade.startCrossFade();
	}
	else
	{
		asset_before = asset;
		scale_before = scale;
		position_before = position;
	}
}

void Chara::setVisible(const bool& s_visible)
{
	visible = s_visible;
	if (visible && !visible_before)
	{
		m_fade.startFadeIn();
	}
	else if (!visible && visible_before)
	{
		m_fade.startFadeOut();
	}
}

void Chara::draw()
{
	const FadeState prevState = m_fade.state();
	const bool      done = m_fade.update();

	// クロスフェード中: 旧アセットをフェードアウトしながら描画
	if (prevState == FadeState::CrossFade)
	{
		const double oldAlpha = 1.0 - m_fade.alpha();
		if (oldAlpha > 0.0)
		{
			const auto& tex = TextureAsset(name + asset_before);
			if (!tex.isEmpty())
			{
				tex.scaled(scale_before)
					.draw(getDrawPos(tex, scale_before, position_before), ColorF{ 1.0, oldAlpha * alpha });
			}
		}
		if (done)
		{
			asset_before = asset;
			scale_before = scale;
			position_before = position;
		}
	}

	// フェード完了時の visible_before 更新
	if (done)
	{
		if (prevState == FadeState::FadeIn)  visible_before = true;
		if (prevState == FadeState::FadeOut) visible_before = false;
	}

	// 現在のアセットを alpha 付きで描画（0なら描画スキップ）
	const double a = m_fade.alpha();
	if (a > 0.0)
	{
		const auto& tex = TextureAsset(name + asset);
		if (!tex.isEmpty())
		{
			tex.scaled(scale)
				.draw(getDrawPos(tex, scale, position), ColorF{ 1.0, a * alpha });
		}
	}
}

bool Chara::updateMove()
{
	if (fadeTeleportAnim.active)
	{
		fadeTeleportAnim.time += Scene::DeltaTime();

		const double t = Clamp(fadeTeleportAnim.time / fadeTeleportAnim.duration, 0.0, 1.0);

		if (t < 0.5)
		{
			alpha = 1.0 - (t / 0.5);
		}
		else
		{
			if (!fadeTeleportAnim.switched)
			{
				position = fadeTeleportAnim.to;
				position_before = position;
				fadeTeleportAnim.switched = true;
			}

			alpha = (t - 0.5) / 0.5;
		}

		if (t >= 1.0)
		{
			alpha = 1.0;
			fadeTeleportAnim.active = false;
			moving = false;
			blocking = false;
		}

		return moving;
	}

	if (moveAnim.active)
	{
		moveAnim.time += Scene::DeltaTime();

		const double t = Clamp(moveAnim.time / moveAnim.duration, 0.0, 1.0);
		const double e = EaseInOutCubicLocal(t);

		position = moveAnim.from.lerp(moveAnim.to, e);

		if (t >= 1.0)
		{
			position = moveAnim.to;
			position_before = position;
			moveAnim.active = false;
			moving = false;
			blocking = false;
		}

		return moving;
	}

	if (m_pattern != MovePattern::Default)
	{
		patternTime += Scene::DeltaTime();

		const double t = Clamp(patternTime / patternDuration, 0.0, 1.0);
		const double e = EaseInOutCubicLocal(t);

		switch (m_pattern)
		{
		case MovePattern::Step:
			position = position_before.movedBy(0, -20.0 * Math::Sin(t * Math::Pi * 2.0));
			break;
		case MovePattern::Jump:
			position = position_before.movedBy(0, -90.0 * Math::Sin(t * Math::Pi));
			break;
		case MovePattern::BackLeft:
			position = position_before.lerp(position_before.movedBy(-50, 0), e);
			break;
		case MovePattern::BackRight:
			position = position_before.lerp(position_before.movedBy(50, 0), e);
			break;
		default:
			break;
		}

		if (t >= 1.0)
		{
			position_before = position;
			m_pattern = MovePattern::Default;
			moving = false;
			blocking = false;
		}

		return moving;
	}

	return false;
}

void Chara::moveTo(const Vec2& to, double duration, bool wait)
{
	moveAnim.from = position;
	moveAnim.to = to;
	moveAnim.duration = Max(duration, 0.001);
	moveAnim.time = 0.0;
	moveAnim.active = true;

	fadeTeleportAnim.active = false;
	m_pattern = MovePattern::Default;
	moving = true;
	blocking = wait;
}

void Chara::fadeTeleportTo(const Vec2& to, double duration, bool wait)
{
	fadeTeleportAnim.to = to;
	fadeTeleportAnim.duration = Max(duration, 0.001);
	fadeTeleportAnim.time = 0.0;
	fadeTeleportAnim.active = true;
	fadeTeleportAnim.switched = false;

	moveAnim.active = false;
	m_pattern = MovePattern::Default;
	alpha = 1.0;
	moving = true;
	blocking = wait;
}

void Chara::startMove(MovePattern pattern, bool wait)
{
	if (pattern == MovePattern::Default)
	{
		return;
	}

	position_before = position;
	patternTime = 0.0;
	patternDuration = (pattern == MovePattern::Jump) ? 0.35 : 0.25;
	m_pattern = pattern;

	moveAnim.active = false;
	fadeTeleportAnim.active = false;
	alpha = 1.0;
	moving = true;
	blocking = wait;
}

void Chara::applyMovePatternImmediate(MovePattern pattern)
{
	switch (pattern)
	{
	case MovePattern::BackLeft:
		position = position.movedBy(-50, 0);
		break;
	case MovePattern::BackRight:
		position = position.movedBy(50, 0);
		break;
	default:
		break;
	}

	position_before = position;
}

void Chara::setPositionImmediate(const Vec2& pos)
{
	position = pos;
	position_before = pos;
	moveAnim.active = false;
	fadeTeleportAnim.active = false;
	fadeTeleportAnim.switched = false;
	m_pattern = MovePattern::Default;
	moving = false;
	blocking = false;
	alpha = 1.0;
}

void Chara::MoveReset()
{
	position_before = position;
	moveAnim.active = false;
	fadeTeleportAnim.active = false;
	fadeTeleportAnim.switched = false;
	m_pattern = MovePattern::Default;
	patternTime = 0.0;
	moving = false;
	blocking = false;
	alpha = 1.0;
}

void Chara::changeFlag()
{
	m_fade.startCrossFade();
}

bool Chara::judgeName(const String& s_name)
{
	return name == s_name;
}

String Chara::getName() const
{
	return name;
}

bool Chara::isAnimating() const
{
	return moving || !m_fade.isIdle();
}

bool Chara::isBlocking() const
{
	return blocking && moving;
}

bool Chara::isFading() const
{
	return !m_fade.isIdle();
}

Vec2 Chara::getDrawPos(const Texture& tex, double s, const Vec2& footPos) const
{
	const double verticalAnchorScale = Min(s, 0.8);

	return Vec2{
		footPos.x - tex.width() * s / 2,
		footPos.y - tex.height() * verticalAnchorScale
	};
}
