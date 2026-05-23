# pragma once
# include "Common.hpp"
# include "FadeController.hpp"

class CGmanager
{
public:
	void setVisible(const bool& s_visible);

	void change(const String& portrait);

	void draw();

	void changeFlag();

	// セーブデータ用
	bool   getVisible() const;
	String getAsset()   const;

private:
	String asset, asset_before;
	bool visible = false;
	bool visible_before = false;
	// crossFade=0.5s（2倍速）, fadeIn/Out=1.0s
	FadeController m_fade{ 1.0, 1.0, 0.5 };
};
