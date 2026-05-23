# include "rendering/Chara.hpp"

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
	TextureAsset::LoadAsync(name + asset);
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
	if (portrait != U"NAN")      asset = portrait;
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
			TextureAsset(name + asset_before).scaled(scale_before).drawAt(
				position_before, ColorF{ 1.0, oldAlpha });
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
		TextureAsset(name + asset).scaled(scale).drawAt(position, ColorF{ 1.0, a });
	}
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
	return !m_fade.isIdle();
}
