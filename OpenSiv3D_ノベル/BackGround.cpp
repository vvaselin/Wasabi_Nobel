# include "BackGround.hpp"

void BackGround::init(const String& s_asset)
{
	asset = s_asset;
	asset_before = U"";
	TextureAsset::LoadAsync(asset);
	m_fade.startCrossFade(); // 最初の背景をフェードインで表示
}

void BackGround::Change(const String& s_asset)
{
	if (s_asset == asset) return; // 同じ背景なら何もしない

	asset_before = asset;          // 現在表示中を退避
	asset = s_asset;
	TextureAsset::LoadAsync(asset);
	m_fade.startCrossFade();
}

void BackGround::draw()
{
	const FadeState prevState = m_fade.state();
	const bool      done = m_fade.update();

	if (prevState == FadeState::CrossFade)
	{
		// クロスフェード中: 旧背景を徐々に消しながら新背景を表示
		if (!TextureAsset(asset_before).isEmpty())
		{
			TextureAsset(asset_before).drawAt(Scene::CenterF(), ColorF{ 1.0, 1.0 - m_fade.alpha() });
		}
		if (!TextureAsset(asset).isEmpty())
		{
			TextureAsset(asset).drawAt(Scene::CenterF(), ColorF{ 1.0, m_fade.alpha() });
		}
		if (done)
		{
			asset_before = asset; // 完了時にコミット
		}
	}
	else
	{
		if (!TextureAsset(asset).isEmpty())
		{
			TextureAsset(asset).drawAt(Scene::CenterF());
		}
	}
}

String BackGround::getAsset()
{
	return asset;
}
