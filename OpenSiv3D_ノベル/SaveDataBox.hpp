# pragma once
# include "Common.hpp"
# include "ScriptManager.hpp"

class SaveDataBox
{
public:

	enum class state
	{
		Save,
		Load,
	};

	struct palette_
	{
		ColorF tileColor1;

		ColorF tileColor2;

		ColorF borderColor1;
	};

	SaveDataBox() = default;

	SaveDataBox(const Font& font, double fontSize, const RectF& rect, const palette_& palette);

	SaveDataBox(const FilePath& path ,const Font& font, double fontSize, const RectF& rect, const palette_& palette);

	void setFile();

	void newSave(const FilePath& scriptfile, const int32& line);

	bool update(state s);

	void draw() const;

	String getText() const;

	FilePath getFile() const;

	int32 getLine() const;

private:

	FilePath m_path; //セーブファイル(bin)

	FilePath m_script; //スクリプトファイル

	int32 m_line = 0;

	ScriptManager m_manager;

	DateTime m_date = DateTime::Now();

	Font m_font;

	double m_fontSize = 0;

	String m_text;

	RectF m_rect;

	RectF message_rect; //セリフを表示する領域

	palette_ m_palette;

	bool m_pressed = false;
	bool m_over = false;

	bool se = false;
};
