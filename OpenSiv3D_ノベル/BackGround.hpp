# pragma once
# include "Common.hpp"
# include "FadeController.hpp"

class BackGround
{
public:
	void init(const String& s_asset);

	void Change(const String& s_asset);

	void draw();

	String getAsset();

private:
	String asset;
	String asset_before;
	// クロスフェードのみ使用（背景にフェードイン/アウトは不要）
	FadeController m_fade{ 1.0, 1.0, 1.0 };
};
