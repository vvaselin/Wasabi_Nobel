# include "Config.hpp"

Slider::Slider(double value, const RectF& rect) :value{ value }, rect{ rect } {}

double Slider::update() {
	if (rect.stretched(15).mouseOver())Cursor::RequestStyle(CursorStyle::Hand);

	if (rect.stretched(15).leftClicked())activeFlg = true;

	if (activeFlg) {
		Cursor::RequestStyle(CursorStyle::Hand);
		value = Parse<double>(U"{:.1f}"_fmt(Clamp(Cursor::PosF().x - rect.x, 0.0, rect.w) / rect.w));
		if (MouseL.up())activeFlg = false;
	}
	return value;
}

void Slider::draw(const ColorF& color = { 0.3, 0.5, 1.0 })const {
	rect.draw(Palette::Gray);
	RectF{ rect.pos,value * rect.w,rect.h }.draw(color);
	RoundRect(Arg::center(value * rect.w + rect.x, rect.center().y), 10, 20, 10).draw();
}

void Slider::setValue(const double& value_)
{
	value = value_;
}

double Slider::getValue() {
	return value;
}

bool Slider::isActive() {
	return activeFlg;
}

Config::Config(const InitData& init)
	: IScene{ init }
{
	bool enabled = true;
	bool storyFlag = (getData().ScriptLine != 0);
	Title.cg_enabled(enabled);
	Story.cg_enabled(storyFlag);
	Reset.cg_enabled(enabled);

	if (!AudioAsset(U"テーマ").isPlaying())
	{
		AudioAsset(U"テーマ").play(MixBus0);
	}

	if (FileSystem::Exists(m_path))
	{
		const JSON json = JSON::Load(m_path);
		if (not json) throw Error{ U"Failed to load `config.json`" };

		master_v = json[U"Sound"][U"Master"].get<double>();
		Masterslider.setValue(master_v);
		GlobalAudio::SetVolume(master_v);

		bgm_v = json[U"Sound"][U"BGM"].get<double>();
		BGMslider.setValue(bgm_v);
		GlobalAudio::BusSetVolume(MixBus0, bgm_v);


		se_v = json[U"Sound"][U"SE"].get<double>();
		SEslider.setValue(se_v);
		GlobalAudio::BusSetVolume(MixBus1, se_v);

		ms_add = json[U"Text"][U"Message"].get<double>();
		MSslider.setValue(ms_add);
		getData().ms_add = ms_add;

		auto_w = json[U"Text"][U"Auto"].get<double>();
		AUTOslider.setValue(auto_w);
		getData().auto_w = auto_w;
	}
	else
	{
		JSON json;

		json[U"Sound"][U"Master"] = 1.0;
		json[U"Sound"][U"BGM"] = 0.5;
		json[U"Sound"][U"SE"] = 0.5;

		json[U"Text"][U"Message"] = 0.5;
		json[U"Text"][U"Auto"] = 0.5;

		json.save(m_path);
	}
}

void Config::update()
{

	if (Title.update())
	{
		getData().File = getData().NewGameFile;
		getData().ScriptLine = 0;
		changeScene(State::Title);
	}

	if (Story.update())
	{
		AudioAsset(U"テーマ").stop(0.5s);
		changeScene(State::Game,0.2s);
	}

	//マスターボリューム
	Rect{ Arg::center = Point{300, 350}, 550, 150 }.draw(ColorF{ 0.4 });
	c_font(U"マスターボリューム").draw(TextStyle::Outline(0.2, ColorF{0.0}), Vec2{80,300});

	if (Masterslider.update() != master_v)
	{
		JSON json = JSON::Load(m_path);
		if (not json) throw Error{ U"Failed to load `config.json`" };

		GlobalAudio::SetVolume(Masterslider.update());
		master_v = Masterslider.update();

		json[U"Sound"][U"Master"] = master_v;
		json.save(m_path);
	}
	Masterslider.draw();
	d_font(Masterslider.getValue()).draw(Vec2{600,370 }, ColorF{0.0});
	
	//BGM
	Rect{ Arg::center = Point{300, 550}, 550, 150 }.draw(ColorF{ 0.4 });
	c_font(U"BGM").draw(TextStyle::Outline(0.2, ColorF{ 0.0 }),Vec2{80,500});

	if (BGMslider.update() != bgm_v)
	{
		JSON json = JSON::Load(m_path);
		if (not json) throw Error{ U"Failed to load `config.json`" };

		GlobalAudio::BusSetVolume(MixBus0, BGMslider.update());
		bgm_v = BGMslider.update();

		json[U"Sound"][U"BGM"] = bgm_v;
		json.save(m_path);
	}
	BGMslider.draw();
	d_font(BGMslider.getValue()).draw(Vec2{ 600,570 }, ColorF{ 0.0 });

	//SE
	Rect{ Arg::center = Point{300, 750}, 550, 150 }.draw(ColorF{ 0.4 });
	c_font(U"SE").draw(TextStyle::Outline(0.2, ColorF{ 0.0 }),Vec2{ 80,700 });

	if (SEslider.update() != se_v)
	{
		JSON json = JSON::Load(m_path);
		if (not json) throw Error{ U"Failed to load `config.json`" };

		GlobalAudio::BusSetVolume(MixBus1, SEslider.update());
		se_v = SEslider.update();

		json[U"Sound"][U"SE"] = se_v;
		json.save(m_path);
	}
	SEslider.draw();
	d_font(SEslider.getValue()).draw(Vec2{ 600,770 }, ColorF{ 0.0 });

	//メッセージスピード
	Rect{ Arg::center = Point{1100, 350}, 550, 150 }.draw(ColorF{ 0.4 });
	c_font(U"メッセージスピード").draw(TextStyle::Outline(0.2, ColorF{ 0.0 }),Vec2{ 880,300 });

	if (MSslider.update() != ms_add)
	{
		JSON json = JSON::Load(m_path);
		if (not json) throw Error{ U"Failed to load `config.json`" };

		ms_add = MSslider.update();
		getData().ms_add = ms_add;

		json[U"Text"][U"Message"] = ms_add;
		json.save(m_path);
	}
	MSslider.draw();
	d_font(MSslider.getValue()*100).draw(Vec2{ 1400,370 }, ColorF{ 0.0 });

	//AUTOのウェイト
	Rect{ Arg::center = Point{1100, 550}, 550, 150 }.draw(ColorF{ 0.4 });
	c_font(U"オートスピード").draw(TextStyle::Outline(0.2, ColorF{ 0.0 }),Vec2{ 880,500 });

	if (AUTOslider.update() != auto_w)
	{
		JSON json = JSON::Load(m_path);
		if (not json) throw Error{ U"Failed to load `config.json`" };

		auto_w = AUTOslider.update();
		getData().auto_w = auto_w;

		json[U"Text"][U"Auto"] = auto_w;
		json.save(m_path);
	}
	AUTOslider.draw();
	d_font(AUTOslider.getValue()*100).draw(Vec2{ 1400,570 }, ColorF{ 0.0 });

	//初期設定
	if (Reset.update())
	{
		master_v = 1.0;
		Masterslider.setValue(master_v);
		GlobalAudio::SetVolume(master_v);

		bgm_v = 0.5;
		BGMslider.setValue(bgm_v);
		GlobalAudio::BusSetVolume(MixBus0, bgm_v);

		se_v = 0.5;
		SEslider.setValue(se_v);
		GlobalAudio::BusSetVolume(MixBus1, se_v);

		ms_add = 0.5;
		MSslider.setValue(ms_add);
		getData().ms_add = ms_add;

		auto_w = 0.5;
		AUTOslider.setValue(auto_w);
		getData().auto_w = auto_w;

		JSON json;

		json[U"Sound"][U"Master"] = master_v;
		json[U"Sound"][U"BGM"] = bgm_v;
		json[U"Sound"][U"SE"] = se_v;

		json[U"Text"][U"Message"] = ms_add;
		json[U"Text"][U"Auto"] = auto_w;

		json.save(m_path);
	}
}

void Config::draw() const
{
	TitleFont(U"Config").drawAt(300, 100,ColorF{0.0});
	Scene::SetBackground(ColorF{1.0});
}
