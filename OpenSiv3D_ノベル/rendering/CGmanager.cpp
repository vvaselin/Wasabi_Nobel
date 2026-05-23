# include "rendering/CGmanager.hpp"

void CGmanager::change(const String& portrait)
{
	asset = portrait;
	if (visible && asset_before != asset)
	{
		m_fade.startCrossFade();
	}
	else
	{
		asset_before = asset; // 非表示中は即座に反映
	}
}

void CGmanager::setVisible(const bool& s_visible)
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

void CGmanager::draw()
{
	const FadeState prevState = m_fade.state();
	const bool      done = m_fade.update();

	if (visible)
	{
		if (prevState == FadeState::CrossFade)
		{
			// 旧CGは0.8倍だけフェードアウト（元の演出を維持）
			const double a = m_fade.alpha();
			TextureAsset(asset_before).drawAt(Scene::CenterF(), ColorF{ 1.0, 1.0 - a * 0.8 });
			TextureAsset(asset).drawAt(Scene::CenterF(), ColorF{ 1.0, a });
			if (done) asset_before = asset;
		}
		else if (prevState == FadeState::FadeIn)
		{
			TextureAsset(asset).drawAt(Scene::CenterF(), ColorF{ 1.0, m_fade.alpha() });
			if (done) visible_before = true;
		}
		else
		{
			TextureAsset(asset).drawAt(Scene::CenterF());
		}
	}
	else if (prevState == FadeState::FadeOut)
	{
		TextureAsset(asset).drawAt(Scene::CenterF(), ColorF{ 1.0, m_fade.alpha() });
		if (done)
		{
			visible_before = false;
			asset.clear();
			asset_before.clear();
		}
	}
}

void CGmanager::changeFlag()
{
	m_fade.startCrossFade();
}

bool CGmanager::getVisible() const
{
	return visible;
}

String CGmanager::getAsset() const
{
	return asset;
}
