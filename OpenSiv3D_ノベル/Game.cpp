# include "Game.hpp"

SasaGUI::ScrollBar vbar(SasaGUI::Orientation::Vertical);

Game::Game(const InitData& init)
	: IScene{ init }
	, b_log{ vbar }
{
	if (AudioAsset(U"テーマ").isPlaying())
	{
		AudioAsset(U"テーマ").stop(0.5s);
	}

	SasaGUI::ScrollBar vbar(SasaGUI::Orientation::Vertical);
	
	s_manager.setFile(getData().File, 0);

	Array<String> b_log_ = s_manager.UpdateQuick(getData().ScriptLine);

	b_log.getMessages().clear();

	for (const auto& message : b_log_)
	{
		b_log.addMessage(message);
	}
}

void Game::update()
{
	
	if (E_roll)
	{
		AudioAsset(U"エンディング").play(MixBus0);

		endroll.draw();

		if (!endroll.isRunning())
		{
			if (e_watch.isPaused()) e_watch.restart();

			t += Scene::DeltaTime();

			if (t >= e_time)
			{
				AudioAsset(U"エンディング").stop(1.0s);

				changeScene(State::Title,1.0s);
			}
		}
	}
	else
	{
		//スクリプトによる描画
		s_manager.scriptUpdate();

		s_manager.draw(TextWatch, Menue, getData().ms_add, getData().auto_w);
	}
	
	//End命令が来たらエンドロールへ
	if (s_manager.getEnd() && s_manager.getLine() >= 1)
	{
		E_roll = true;

		endroll.start();
	}

	if (LogWatch && !E_roll) //バックログ描画
	{
		Rect{ 0, 0, 1920, 1080 }.draw(Palette::White.withA(200));

		if ((b_log.ReadLog() && Mouse::Wheel()>=1)||b_log.buttonClicked() >= 1)
		{
			LogWatch = false;
			TextWatch = true;
		}

		//バックログ更新と描画
		b_log.update();

		b_log.draw();
	}
	else if (Mouse::Wheel() <= -1) //バックログ起動
	{
		//バックログの情報更新
		b_log.length_update();

		LogWatch = true;

		TextWatch = false;

		Menue = false;

		b_log.update();

		b_log.ShowNew();
	}
	else//通常描画時はバックログの生成
	{
		if (s_manager.getLogGen())//ログの生成
		{
			b_log.addMessage(s_manager.getLog()); // ログの登録
		}


		//セリフ非表示
		if (TextWatch)
		{
			TextWatch = not MouseR.down();
		}
		else
		{
			TextWatch = (MouseR.down() || winClick());
		}
	}

	//メニュー画面
	if (MenuON.mouseOver() && !Menue && !clicked && !LogWatch && !E_roll)
	{
		Menue = true;

		clicked = false;
	}
	else if (clicked)
	{
		Menue = false;
	}
	if ((MenueOFF.mouseOver()) || not Cursor::OnClientRect())
	{
		Menue = false;

		clicked = false;
	}

	camera.update();
	{
		const auto t = camera.createTransformer();

		for (const auto& button : buttons)
		{
			button.draw();
		}

		if (Menue)
		{
			camera.setTargetCenter(Scene::CenterF());
		}
		else
		{
			camera.setTargetCenter(Scene::CenterF() - Standby);
		}
	}
	//メニューのボタン押されたかどうか
	for (auto& button : buttons)
	{
		if (Menue&&button.update())
		{
			getData().File = s_manager.getFile();

			getData().ScriptLine = s_manager.getLine();

			MenueNum = MenueTable[button.getText()];

			clicked = true;
		}
	}
	//メニューのボタン押されたら
	switch (MenueNum)
	{
	case 1: //Q.SAVE
	{
		getData().File = s_manager.getFile();

		getData().ScriptLine = s_manager.getLine();

		Serializer<BinaryWriter> writer{ U"saveData/Qsave.bin" };

		if (not writer) throw Error{ U"Failed to open `Qsave.bin`" };

		writer(s_manager.getFile(), s_manager.getLine());

		const DateTime t = DateTime::Now();

		writer(t);

		Print << U"セーブしました";
		m_flag = true;
	}
	MenueNum = 0;
	break;
	case 2: //Q.LOAD
	{
		AudioAsset(s_manager.getBGM()).stop(0.5s);

		FilePath qsave = U"saveData/Qsave.bin";

		if (FileSystem::FileSize(qsave)>0)
		{
			Deserializer<BinaryReader> reader{ qsave };
			if (not reader) throw Error{ U"Failed to open `Qsave.bin`" };

			FilePath path_;
			int32 line_;

			reader(path_, line_);

			getData().File = path_;
			getData().ScriptLine = line_;

			changeScene(State::Game,1.0s);
		}
		else
		{
			Print << U"セーブデータがありません";
			m_flag = true;
		}
		
	}
	MenueNum = 0;
	break;
	case 3: //SAVE
	{
		AudioAsset(s_manager.getBGM()).stop(0.5s);
		changeScene(State::Save,0s);
	}
	MenueNum = 0;
	break;
	case 4: //LOAD
	{
		AudioAsset(s_manager.getBGM()).stop(0.5s);
		changeScene(State::Load,0s);
	}
	MenueNum = 0;
	break;
	case 5: //AUTO
	{
		if (s_manager.getAuto())
		{
			s_manager.setAuto(false);
		}
		else
		{
			s_manager.setAuto(true);
			s_manager.setSkip(false);
		}
	}
	MenueNum = 0;
	break;
	case 6: //SKIP
	{
		if (s_manager.getSkip())
		{
			s_manager.setSkip(false);
		}
		else
		{
			s_manager.setSkip(true);
			s_manager.setAuto(false);
		}
	}
	MenueNum = 0;
	break;
	case 7: //LOG
	{
		//バックログの情報更新
		b_log.length_update();

		LogWatch = true;

		TextWatch = false;

		b_log.update();

		b_log.ShowNew();
	}
	MenueNum = 0;
	break;
	case 8: //CONFIG
	{
		s_manager.StopBGM();

		changeScene(State::Config,0s);
	}
	MenueNum = 0;
	break;
	default:
		break;
	}

	if (m_flag)
	{
		m_t += Scene::DeltaTime();

		if (m_t>=e_time)
		{
			m_t -= e_time;

			ClearPrint();

			m_flag = false;

		}
	}
}
void Game::draw() const
{
	Scene::SetBackground(Palette::Black);
}

