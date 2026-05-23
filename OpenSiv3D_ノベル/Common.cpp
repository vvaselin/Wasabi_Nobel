# include "Common.hpp"


//関数
//ゲーム初期化関数
void InitGame()
{
	Scene::SetResizeMode(ResizeMode::Keep);
	Window::Resize(1024, 576);
	Scene::Resize(1920, 1080);

	FontAsset::Register(U"TitleFont", 300, Typeface::Heavy);
}
//ルビ振り用関数
void DrawTextWithRuby(const Vec2& basePos, const Font& mainFont, const Font& rubyFont, const String& text, const Array<Ruby>& rubyList, double rubyYOffset)
{
	const Array<double> xAdvances = mainFont(text).getXAdvances();

	Array<Vec2> allPenPos;

	Vec2 penPos{ basePos };

	// 文字単位で描画を制御するためのループ
	for (const auto& glyph : mainFont.getGlyphs(text))
	{
		allPenPos << penPos;

		// 改行文字なら
		if (glyph.codePoint == U'\n')
		{
			// ペンの X 座標をリセット
			penPos.x = basePos.x;

			// ペンの Y 座標をフォントの高さ分進める
			penPos.y += mainFont.height();

			continue;
		}

		// 文字のテクスチャをペンの位置に文字ごとのオフセットを加算して描画
		// FontMethod がビットマップ方式の場合に限り、Math::Round() で整数座標にすると品質が向上
		glyph.texture.draw(Math::Round(penPos + glyph.getOffset()));

		// ペンの X 座標を文字の幅の分進める
		penPos.x += glyph.xAdvance;
	}

	for (const auto& ruby : rubyList)
	{
		if (ruby.indexEnd < text.size())
		{
			const Vec2 beginPenPos = (allPenPos[ruby.indexBegin] + Vec2{ 0, rubyYOffset });

			const Vec2 endPenPos = (allPenPos[ruby.indexEnd] + Vec2{ xAdvances[ruby.indexEnd], rubyYOffset });

			const Vec2 center = ((beginPenPos + endPenPos) / 2);

			rubyFont(ruby.text).draw(Arg::bottomCenter = center);
		}
	}
}

//スクリプト解析用(position)
Vec2 posJudge(const String& pos)
{
	if (pos == U"L")
	{
		return  Vec2{ Scene::Width() / 5, 0};
	}
	else if (pos == U"C")
	{
		return Vec2{ Scene::Center().x, 0};
	}
	else if (pos == U"R")
	{
		return Vec2{ Scene::Width() / 5*4 , 0};
	}
	else
	{
		return Vec2{ 0,0 };
	}
}

//スクリプト解析用(scale)
double scaleJudge(const String& scale)
{
	if (scale == U"大")
	{
		return 2;
	}
	else if (scale == U"中")
	{
		return 0.8;
	}
	else if (scale == U"小")
	{
		return 0.5;
	}
	else
	{
		return 0.0;
	}
}

//TextureAssetから探す
String SerchAsset(const String& line)
{
	for (auto [name, info] : TextureAsset::Enumerate())
	{
		if (name == line)
		{
			return line;
		}
	}
	return U"NAN";
}

//メッセージボックス端の円
void messageCircle()
{
	const double t = Scene::Time();
	for (auto i : step(12))
	{
		const double theta = (i * 30_deg + t * 120_deg);
		const Vec2 pos = OffsetCircular{ Scene::Size() - Vec2{50,50}, 20, theta};
		Circle{ pos, 3 }.draw(ColorF{ 0.8 });
	}
}

//画面内でクリックされたか(メッセージボックス更新用)
bool winClick()
{
	return ((MouseL.down() || Mouse::Wheel() == 1 || KeyEnter.down() || KeySpace.down()) && (Cursor::OnClientRect()));
}

bool Button(const Point& point, const Font& font, const String& text, bool enabled)
{
	RoundRect rect{point, 300, 100, 10};

	double t = 0.0;

	if (enabled && rect.mouseOver())
	{
		t = 0.4;
		Cursor::RequestStyle(CursorStyle::Hand);
	}
	else
	{
		t = 0.0;
	}

	if (enabled)
	{
		rect.draw( ColorF{ 1.0, 0.6 }.lerp(Palette::Royalblue,t));
		rect.drawFrame( 0, 1, ColorF{ 0.0 });
		font(text).drawAt(40, (rect.x + rect.w / 2), (rect.y + rect.h / 2),ColorF{0.0});
	}
	else
	{
		rect.draw(ColorF{ 0.5 });
		font(text).drawAt(40, (rect.x + rect.w / 2), (rect.y + rect.h / 2), ColorF{ 0.7 });
	}

	return (enabled && (rect.leftClicked()||(rect.mouseOver()&&KeyEnter.down())));
}

//バッグログ用の文生成
String GeneLog(const String message)
{
	const String charas = U"[r]";
	const String partition{ U"{|}" }; // ルビ区切る用

	String s = message.replaced(charas, U"");
	String tmp;

	int32 i = 0, j = 0; //i:何番目の文字を処理しているか。 j:改行してから何文字目か

	while (s[i])
	{
		if (s[i] == partition[0]) // "{"だったら
		{
			i++;
			while (s[i] != partition[1]) // "|"になるまで
			{
				tmp << s[i];
				i++; j++;
			}
			i++;
			while (s[i] != partition[2]) // "}"になるまで
			{
				i++;
			}
		}
		else
		{
			if ((j % 35 == 0) && j > 1)
			{
				tmp += U"\n";
				j = 0;
			}

			tmp += s[i];
		}
		i++; j++;
	}

	return tmp;
}

String AddLog(String& s_, const String message)

{
	const String charas = U"[r]";
	const String partition{ U"{|}" }; // ルビ区切る用

	String s = message.replaced(charas, U"");
	String tmp;

	int32 i = 0, j = s_.size(); //i:何番目の文字を処理しているか。 j:改行してから何文字目か

	while (s[i])
	{
		if (s[i] == partition[0]) // "{"だったら
		{
			i++;
			while (s[i] != partition[1]) // "|"になるまで
			{
				tmp << s[i];
				i++; j++;
			}
			i++;
			while (s[i] != partition[2]) // "}"になるまで
			{
				i++;
			}
		}
		else
		{
			if ((j % 35 == 0) && j > 1)
			{
				tmp += U"\n";
				j = 0;
			}

			tmp += s[i];
		}
		i++; j++;
	}

	return tmp;
}

bool IsUNI(const FilePath& path)
{
	const FilePath directory = Resource(U"uni/").lowercased();
	return path.lowercased().starts_with(directory);
}

bool IsYUZU(const FilePath& path)
{
	const FilePath directory = Resource(U"yuzu/").lowercased();
	return path.lowercased().starts_with(directory);
}

bool IsNIUM(const FilePath& path)
{
	const FilePath directory = Resource(U"nium/").lowercased();
	return path.lowercased().starts_with(directory);
}

bool IsDEL(const FilePath& path)
{
	const FilePath directory = Resource(U"del/").lowercased();
	return path.lowercased().starts_with(directory);
}

bool IsCAT(const FilePath& path)
{
	const FilePath directory = Resource(U"cat/").lowercased();
	return path.lowercased().starts_with(directory);
}

bool IsBG(const FilePath& path)
{
	const FilePath directory = Resource(U"background/").lowercased();
	return path.lowercased().starts_with(directory);
}

bool IsBGM(const FilePath& path)
{
	const FilePath directory = Resource(U"bgm/").lowercased();
	return path.lowercased().starts_with(directory);
}

bool IsCG(const FilePath& path)
{
	const FilePath directory = Resource(U"cg/").lowercased();
	return path.lowercased().starts_with(directory);
}

bool IsSE(const FilePath& path)
{
	const FilePath directory = Resource(U"se/").lowercased();
	return path.lowercased().starts_with(directory);
}

/// @brief リソースファイルの拡張子を取得します。
/// @param path リソースファイルパス
/// @return リソースファイルの拡張子
String ResourceExtension(const FilePathView path)
{
	if (not path)
	{
		return{};
	}

	const size_t dotPos = path.lastIndexOf(U'.');

	if (dotPos == String::npos)
	{
		return{};
	}

	const size_t sepPos = path.lastIndexOfAny(U"/\\");

	if ((sepPos != String::npos) && (dotPos < sepPos))
	{
		return{};
	}

	return String{ path.substr(dotPos + 1) }.lowercase();
}

/// @brief リソースファイル用の FileSystem::BaseName() です。
/// @param path リソースファイルパス
/// @return リソースファイルのファイル名
String ResourceBaseName(const FilePathView path)
{
	const size_t sepPos = path.lastIndexOf(U'/');

	String fileName;

	if (sepPos == String::npos)
	{
		fileName = path;
	}
	else
	{
		fileName = String((path.begin() + sepPos + 1), path.end());
	}

	const size_t dotPos = fileName.lastIndexOf(U'.');

	if (dotPos == String::npos)
	{
		return fileName;
	}

	return String(fileName.begin(), fileName.begin() + dotPos);
}



void AssetInit()
{
	//キャラのアセット
	//UNI
	for (const auto& path : EnumResourceFiles().filter(IsUNI))
	{
		if (ResourceExtension(path) == U"png") {
			TextureAsset::Register(U"UNI" + ResourceBaseName(path), path, TextureDesc::Mipped);
		}
	}
	//YUZU
	for (const auto& path : EnumResourceFiles().filter(IsYUZU))
	{
		if (ResourceExtension(path) == U"png") {
			TextureAsset::Register(U"YUZU" + ResourceBaseName(path), path, TextureDesc::Mipped);
		}
	}

	//NIUM
	for (const auto& path : EnumResourceFiles().filter(IsNIUM))
	{
		if (ResourceExtension(path) == U"png") {
			TextureAsset::Register(U"NIUM" + ResourceBaseName(path), path, TextureDesc::Mipped);
		}
	}

	//DEL
	for (const auto& path : EnumResourceFiles().filter(IsDEL))
	{
		if (ResourceExtension(path) == U"png") {
			TextureAsset::Register(U"DEL" + ResourceBaseName(path), path, TextureDesc::Mipped);
		}
	}

	//CAT
	for (const auto& path : EnumResourceFiles().filter(IsCAT))
	{
		if (ResourceExtension(path) == U"png") {
			TextureAsset::Register(U"CAT" + ResourceBaseName(path), path, TextureDesc::Mipped);
		}
	}

	//背景のアセット
	for (const auto& path : EnumResourceFiles().filter(IsBG))
	{
		if ((ResourceExtension(path) == U"jpg") || (ResourceExtension(path) == U"png")) {
			TextureAsset::Register(ResourceBaseName(path), path);
		}
	}

	//BGMのアセット
	for (const auto& path : EnumResourceFiles().filter(IsBGM))
	{
		if ((ResourceExtension(path) == U"wav") || (ResourceExtension(path) == U"mp3")) {
			AudioAsset::Register(ResourceBaseName(path), Audio::Stream, path, Loop::Yes);
		}
	}

	//SEのアセット
	for (const auto& path : EnumResourceFiles().filter(IsSE))
	{
		if ((ResourceExtension(path) == U"wav") || (ResourceExtension(path) == U"mp3")) {
			AudioAsset::Register(ResourceBaseName(path), Audio::Stream, path);
		}
	}

	//CGのアセット
	for (const auto& path : EnumResourceFiles().filter(IsCG))
	{
		if ((ResourceExtension(path) == U"jpg") || (ResourceExtension(path) == U"png")) {
			TextureAsset::Register(ResourceBaseName(path), path);
		}
	}

}
