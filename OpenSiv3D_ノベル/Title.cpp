# include "Title.hpp"

Title::Title(const InitData& init)
	: IScene{ init }
{
	bool enabled = true;

	NEWGAME.cg_enabled(enabled);
	LOAD.cg_enabled(enabled);
	CONFIG.cg_enabled(enabled);
	LICENSES.cg_enabled(enabled);

	if (!AudioAsset(U"テーマ").isPlaying())
	{
		AudioAsset(U"テーマ").play(MixBus0);
	}

	if (FileSystem::Exists(m_path))
	{
		const JSON json = JSON::Load(m_path);
		if (not json) throw Error{ U"Failed to load `config.json`" };

		//コンフィグ読み込み
		GlobalAudio::SetVolume(json[U"Sound"][U"Master"].get<double>());

		GlobalAudio::BusSetVolume(MixBus0, json[U"Sound"][U"BGM"].get<double>());

		GlobalAudio::BusSetVolume(MixBus1, json[U"Sound"][U"SE"].get<double>());

		getData().ms_add = json[U"Text"][U"Message"].get<double>();

		getData().auto_w = json[U"Text"][U"Auto"].get<double>();
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

void Title::update()
{
	TextureAsset(U"サ室_明").drawAt(Scene::CenterF());

	if (KeyDown.down()||KeyUp.down())
	{
		++index %= index_max;

		Cursor::SetPos(Point{350, 550}.movedBy(Point{0,100*index}));
	}

	if (NEWGAME.update())
	{
		AudioAsset(U"テーマ").stop(2.5s);
		// ゲームシーンに遷移
		changeScene(State::Game);
	}
	if (LOAD.update())
	{
		// ロード
		changeScene(State::Load,0.2s);
	}
	if (CONFIG.update())
	{
		// コンフィグ
		changeScene(State::Config,0.2s);
	}
	if (LICENSES.update())
	{
		//ライセンス表示
		LicenseManager::ShowInBrowser();
	}

	title.scaled(0.43).drawAt(Vec2{ 600,300 });
}

void Title::draw() const
{
	Scene::SetBackground(ColorF{1.0});

}
