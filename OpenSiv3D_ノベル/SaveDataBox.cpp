# include "SaveDataBox.hpp"

SaveDataBox::SaveDataBox(const Font& font, double fontSize, const RectF& rect, const palette_& palette)
	: m_font{ font }
	, m_fontSize{ fontSize }
	, m_rect{ rect }
	, message_rect{ RectF{rect.leftX(), rect.bottomY() - rect.h / 2, rect.w, rect.h / 2 }.stretched(-20) }
	, m_palette{ palette } {}

SaveDataBox::SaveDataBox(const FilePath& path, const Font& font, double fontSize, const RectF& rect, const palette_& palette)
	: m_path{ path }
	, m_font{ font }
	, m_fontSize{ fontSize }
	, m_rect{ rect }
	, message_rect{ RectF{rect.leftX(), rect.bottomY() - rect.h/2, rect.w, rect.h / 2 }.stretched(-20)}
	, m_palette{ palette } {}

//既にあるセーブファイルを読み込む
void SaveDataBox::setFile()
{
	bool DataExists = false;

	{
		BinaryReader reader{ m_path };

		if (not reader) throw Error{ U"Failed to open save file" };

		// 読み込むテキストの長さを格納する変数
		uint64 length = 0;

		// 読み込んだテキストの格納先
		String text;

		// テキストの長さを読み込む
		reader.read(length);

		DataExists = (length>0);
	}

	if (DataExists)
	{
		// バイナリファイルをオープン
		Deserializer<BinaryReader> reader{ m_path };
		if (not reader) throw Error{ U"Failed to open save file" };

		reader(m_script);
		reader(m_line);
		reader(m_date);

		m_manager.setFile(m_script, 0);

		String text_ = m_manager.UpdateQuick(m_line).back();
		const String::value_type dot{ U'.' };
		if (text_[0] == dot) text_.pop_front();

		m_text = GeneLog( text_ );
		m_manager.StopBGM();
	}
}

void SaveDataBox::newSave(const FilePath& scriptfile, const int32& line)
{
	m_script = scriptfile;

	m_line = line;

	m_manager.setFile(m_script, 0);
	m_text = m_manager.UpdateQuick(m_line).back();
	m_manager.StopBGM();

	m_date = DateTime::Now();

	Serializer<BinaryWriter> writer{ m_path };
	if (not writer) throw Error{ U"Failed to open save file" };
	writer(m_script);
	writer(m_line);
	writer(m_date);
}

bool SaveDataBox::update(state s)
{
	const bool mouseOver = m_rect.mouseOver();

	bool pushed = false;

	if (mouseOver)
	{
		if (!se && !m_text.isEmpty())
		{
			AudioAsset(U"マウスオーバー").playOneShot(MixBus1);
			se = true;
		}
		Cursor::RequestStyle(CursorStyle::Hand);
		m_over = true;
	}
	else
	{
		se = false;
		m_over = false;
	}

	if (not m_pressed)
	{
		if (m_rect.leftClicked())
		{
			m_pressed = true;
		}
	}
	else
	{
		if (m_rect.leftReleased())
		{
			AudioAsset(U"選択").playOneShot(MixBus1);
			m_pressed = false;
			pushed = true;
		}
		else if (not m_rect.mouseOver())
		{
			m_pressed = false;
		}
	}
	if (s == state::Load)
	{
		return (not m_text.isEmpty() && pushed);
	}

	return pushed;
}

void SaveDataBox::draw() const
{
	if (!m_text.isEmpty())
	{
		// タイル
		double l = 0.0;
		if (m_over) l = 0.4;

		m_rect.draw(m_palette.tileColor1.lerp(Palette::Slateblue, l));
		m_rect.drawFrame(2, m_palette.borderColor1);

		// アイコン
		

		//テキスト
		m_font(m_date).drawAt(m_fontSize, m_rect.getRelativePoint(0.6, 0.2), m_palette.tileColor2);
		m_font(m_text).draw(m_fontSize, message_rect, m_palette.tileColor2);
	}
	else
	{
		m_rect.draw(m_palette.tileColor1);
		m_rect.drawFrame(2, m_palette.borderColor1);
		m_font(U"NO DATA").drawAt(50, m_rect.getRelativePoint(0.5, 0.5), m_palette.tileColor2);
	}
	
}

String SaveDataBox::getText() const
{
	return m_text;
}

FilePath SaveDataBox::getFile() const
{
	return m_script;
}

int32 SaveDataBox::getLine() const
{
	return m_line;
}

