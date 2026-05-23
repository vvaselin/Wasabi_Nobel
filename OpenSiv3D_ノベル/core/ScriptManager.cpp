# include "core/ScriptManager.hpp"

void ScriptManager::setFile(const FilePath& f, const size_t& n)
{
	bgm.Stop();
	TextReader reader{ f };
	if (not reader) throw Error{ U"Failed to open script" };
	ScriptFile = f;

	String line;
	messages.clear();
	while (reader.readLine(line))
	{
		messages << line;
	}

	scriptLine = n;
	End = false;
}

// -------------------------------------------------------
// 空行・コメント行をスキップ
// -------------------------------------------------------
void ScriptManager::skipBlankLines()
{
	while ((messages[scriptLine] == U"") || (messages[scriptLine] == U" ") || (messages[scriptLine][0] == landmark))
	{
		++scriptLine %= messages.size();
	}
}

// -------------------------------------------------------
// コマンド実行（統合版）
// -------------------------------------------------------
void ScriptManager::executeCommand(Array<String>& order, ExecMode mode, int32 targetLine)
{
	switch (Orders[order[0]])
	{
	case 1: // キャラの定義
	{
		bool exist = false;
		for (auto& chara : characters)
		{
			if (chara.judgeName(order[1]))
			{
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			const bool visible = (order[5] == U"YES");
			Vec2 pos = posJudge(order[3]);
			double scale = scaleJudge(order[4]);
			characters << Chara(order[1], order[2], pos, scale, visible);
		}
	}
	break;

	case 2: // 立ち絵の変更
	{
		String portrait = U"NAN";
		Vec2 pos = Vec2{ 0,0 };
		double scale = 0.0;

		for (auto& chara : characters)
		{
			if (chara.judgeName(order[1]))
			{
				for (size_t i = 2; i < order.size(); i++)
				{
					if (SerchAsset(chara.getName() + order[i]) != U"NAN")
					{
						portrait = order[i];
					}
					else if (posJudge(order[i]) != Vec2{ 0,0 })
					{
						pos = posJudge(order[i]);
					}
					else if (scaleJudge(order[i]) != 0.0)
					{
						scale = scaleJudge(order[i]);
					}
				}
				chara.change(portrait, scale, pos);
				break;
			}
		}
	}
	break;

	case 3: // キャラを表示するか切り替え
	{
		bool visible = (order[2] == U"YES");
		for (auto& chara : characters)
		{
			if (chara.judgeName(order[1]))
			{
				chara.setVisible(visible);
				break;
			}
		}
	}
	break;

	case 4: // 選択肢表示
	{
		if (mode == ExecMode::Quick && static_cast<int32>(scriptLine) != targetLine)
		{
			// Quick: 選択肢を通過済み → どちらの分岐に入ったか推定してジャンプ
			targets.clear();
			for (size_t i = 1; i <= 2; i++)
			{
				Array<String> c_split = messages[scriptLine + i].split(colon);
				targets << c_split[0];
			}

			String goal;
			int32 tmp = targetLine;
			while (tmp > static_cast<int32>(scriptLine))
			{
				if ((messages[tmp] == landmark + targets[0]) || (messages[tmp] == landmark + targets[1]))
				{
					goal = messages[tmp];
					break;
				}
				--tmp %= messages.size();
			}

			while (messages[scriptLine] != goal)
			{
				++scriptLine %= messages.size();
			}
		}
		else
		{
			// Normal、またはQuickで復元先がこの行 → 選択肢UIを表示
			Skip_ = false;
			saveLine = scriptLine;
			m_box.SwitchOFF();
			selecting = true;
			targets.clear();
			Array<String> selects;
			for (size_t i = 1; i <= 2; i++)
			{
				Array<String> c_split = messages[scriptLine + i].split(colon);
				targets << c_split[0];
				selects << c_split[1];
			}
			s_box.setSelects(selects);
		}
	}
	break;

	case 5: // ジャンプ機能
	{
		String goal = landmark + order[1];
		while (messages[scriptLine] != goal)
		{
			++scriptLine %= messages.size();
		}
	}
	break;

	case 6: // 集中線
	{
		if (order[1] == U"YES")
		{
			LFflag = true;
			l_effect.init();
		}
		else
		{
			LFflag = false;
		}
	}
	break;

	case 7: // キャラのレイヤー
	{
		const int32 num = Parse<int32>(order[2]);
		int32 i = 0;
		for (auto& chara : characters)
		{
			if (chara.judgeName(order[1]))
			{
				chara.changeFlag();
				Chara tmp = chara;
				characters.erase(characters.begin() + i);
				// erase後はsize が1減るので、numをその範囲内に収める
				const int32 insertPos = Min(num, static_cast<int32>(characters.size()));
				characters.insert(characters.begin() + insertPos, tmp);
				break;
			}
			i++;
		}
	}
	break;

	case 8: // キャラを動かす（未実装）
	{

	}
	break;

	case 9: // ゲーム終了
	{
		bgm.Stop();
		End = true;
	}
	break;

	case 10: // 画面揺れ
	{
		shake = true;
	}
	break;

	case 11: // 背景切り替え
	{
		bg.Change(order[1]);
	}
	break;

	case 12: // カメラ（未実装）
	{
		if (order[1] == U"OFF")
		{

		}
	}
	break;

	case 13: // BGM切り替え
	{
		if (order[1] == U"OFF")
		{
			bgm.Stop();
		}
		else
		{
			bgm.Change(order[1]);
		}
	}
	break;

	case 14: // 新しいスクリプト
	{
		int32 num = 0;
		if (order[2]) num = Parse<int32>(order[2]);
		this->setFile(order[1], num);
	}
	break;

	case 15: // 効果音設定
	{
		// Quick時は復元先の行でだけ鳴らす
		if (mode == ExecMode::Normal || static_cast<int32>(scriptLine) == targetLine)
		{
			SE_flag = true;
			SE_name = order[1];
		}
	}
	break;

	case 16: // イベントCG
	{
		if (order[1] == U"OFF")
		{
			cg.setVisible(false);
		}
		else
		{
			cg.change(order[1]);
			cg.setVisible(true);
		}
	}
	break;

	case 17: // ムービー再生（未実装）
	{

	}
	break;

	default:
		break;
	}
}

// -------------------------------------------------------
// セリフ行の処理（統合版）
// -------------------------------------------------------
void ScriptManager::processDialogue(ExecMode mode, Array<String>* outLog)
{
	Array<String> Dialogue = messages[scriptLine].split(colon);
	if (Dialogue.size() == 1) Dialogue.push_front(U""); //colon忘れたときの安全策

	saveLine = scriptLine;

	if (mode == ExecMode::Normal)
	{
		m_box.SwitchON();
	}

	if (Dialogue[1][0] == dot)
	{
		m_box.addText(Dialogue[1]);
	}
	else
	{
		m_box.setText(Dialogue[0], Dialogue[1]);
	}

	Log = Dialogue[0] + Dialogue[1];

	if (mode == ExecMode::Normal)
	{
		LogGen = true;
	}
	else if (outLog)
	{
		*outLog << Log;
	}
}

// -------------------------------------------------------
// 通常再生（コルーチン）
// -------------------------------------------------------
void ScriptManager::startScript()
{
	m_runner.reset(); // 前回のタスクを停止
	m_runner = std::make_shared<Co::ScopedTaskRunner>(runScript().runScoped());
}

// -------------------------------------------------------
// 通常再生（毎フレーム呼ばれる）
// -------------------------------------------------------
void ScriptManager::scriptUpdate()
{
	bool isRead = not m_box.isReading();

	if (isRead && !selecting)
	{
		skipBlankLines();

		if (messages[scriptLine][0] == atMark)
		{
			Array<String> order = messages[scriptLine].split(colon);
			order[0].pop_front();
			executeCommand(order, ExecMode::Normal);
		}
		else
		{
			processDialogue(ExecMode::Normal);
		}

		++scriptLine %= messages.size();
	}
}

// -------------------------------------------------------
// 高速再生（セーブ/ロード復元用）
// -------------------------------------------------------
Array<String> ScriptManager::UpdateQuick(const int32& line)
{
	Array<String> Out;
	scriptLine = 0;

	while (scriptLine <= line)
	{
		skipBlankLines();

		if (messages[scriptLine][0] == atMark)
		{
			Array<String> order = messages[scriptLine].split(colon);
			order[0].pop_front();
			executeCommand(order, ExecMode::Quick, line);
		}
		else
		{
			processDialogue(ExecMode::Quick, &Out);
		}

		++scriptLine %= messages.size();
	}

	return Out;
}

// -------------------------------------------------------
// 描画
// -------------------------------------------------------
void ScriptManager::draw(const bool& textON, const bool& menue, const double& speed, const double& autospeed)
{
	Mat3x2 mat = Mat3x2::Identity();
	if (shake)
	{
		transition.update(true);
		shake = false;
	}
	else
	{
		transition.update(false);
	}
	if (const double t = transition.value())
	{
		mat = Mat3x2::Translate(RandomVec2(t * 50));
	}

	{
		const Transformer2D transformer{ mat };

		bg.draw();
		for (auto& chara : characters)
		{
			chara.draw();
		}
		cg.draw();
	}

	if (LFflag)
	{
		l_effect.draw();
	}

	if (textON)
	{
		if (selecting)
		{
			s_box.draw();
			s_box.isSelected();
			if (s_box.isClicked() == 1)
			{
				String goal = landmark + targets[0];
				while (messages[scriptLine] != goal)
				{
					++scriptLine %= messages.size();
				}
				++scriptLine %= messages.size();
				selecting = false;
			}
			else if (s_box.isClicked() == 2)
			{
				String goal = landmark + targets[1];
				while (messages[scriptLine] != goal)
				{
					++scriptLine %= messages.size();
				}
				++scriptLine %= messages.size();
				selecting = false;
			}
		}
		else
		{
			bool clicked = winClick() && !menue;

			if (Auto_ && m_box.getEndFlag())
			{
				const double autotime = spawnTime * Clamp(autospeed, 0.1, 1.0) * 2;

				accumulator += Scene::DeltaTime();

				if (autotime <= accumulator)
				{
					accumulator -= autotime;

					clicked = true;
				}
			}
			else if (Skip_)
			{
				clicked = true;
			}

			m_box.draw(clicked, speed);
		}

		if (SE_flag)
		{
			AudioAsset(SE_name).playOneShot(MixBus1);
			SE_flag = false;
			SE_name.clear();
		}
	}
}

void ScriptManager::StopBGM()
{
	bgm.Stop();
}

String ScriptManager::getBGM()
{
	return bgm.GetCurrentBGM();
}

bool ScriptManager::getLogGen() const
{
	return LogGen;
}

String ScriptManager::getLog()
{
	if (LogGen)
	{
		LogGen = false;
	}
	return Log;
}

bool ScriptManager::getEnd() const
{
	return End;
}

String ScriptManager::getFile() const
{
	return ScriptFile;
}
int32 ScriptManager::getLine() const
{
	return saveLine;
}

void ScriptManager::setAuto(const bool& Flag)
{
	Auto_ = Flag;
}

bool ScriptManager::getAuto() const
{
	return Auto_;
}

void ScriptManager::setSkip(const bool& Flag)
{
	Skip_ = Flag;
}

bool ScriptManager::getSkip() const
{
	return Skip_;
}

Co::Task<> ScriptManager::runScript()
{
	while (!End)
	{
		skipBlankLines();

		if (messages[scriptLine][0] == atMark)
		{
			Array<String> order = messages[scriptLine].split(colon);
			order[0].pop_front();
			const bool advance = co_await executeCommandAsync(order);
			if (advance) ++scriptLine %= messages.size();
		}
		else
		{
			co_await processDialogueAsync();
			++scriptLine %= messages.size();
		}
	}
}

Co::Task<> ScriptManager::processDialogueAsync()
{
	Array<String> Dialogue = messages[scriptLine].split(colon);
	if (Dialogue.size() == 1) Dialogue.push_front(U"");

	saveLine = scriptLine;
	m_box.SwitchON();

	if (Dialogue[1][0] == dot)
		m_box.addText(Dialogue[1]);
	else
		m_box.setText(Dialogue[0], Dialogue[1]);

	Log = Dialogue[0] + Dialogue[1];
	LogGen = true;

	// クリックで進むまで待機（draw()が入力を処理する）
	co_await Co::WaitWhile([this] { return m_box.isReading(); });
}

Co::Task<bool> ScriptManager::executeCommandAsync(Array<String>& order)
{
	switch (Orders[order[0]])
	{
	case 1: // @new_chara — 即時
	{
		bool exist = false;
		for (auto& chara : characters)
		{
			if (chara.judgeName(order[1]))
			{
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			const bool visible = (order[5] == U"YES");
			Vec2 pos = posJudge(order[3]);
			double scale = scaleJudge(order[4]);
			characters << Chara(order[1], order[2], pos, scale, visible);
		}
		co_return true;
	}

	case 2: // @change — アニメーション完了まで待機
	{
		String portrait = U"NAN";
		Vec2 pos = Vec2{ 0,0 };
		double scale = 0.0;

		for (auto& chara : characters)
		{
			if (chara.judgeName(order[1]))
			{
				for (size_t i = 2; i < order.size(); i++)
				{
					if (SerchAsset(chara.getName() + order[i]) != U"NAN")
						portrait = order[i];
					else if (posJudge(order[i]) != Vec2{ 0,0 })
						pos = posJudge(order[i]);
					else if (scaleJudge(order[i]) != 0.0)
						scale = scaleJudge(order[i]);
				}
				chara.change(portrait, scale, pos);
				break;
			}
		}
		co_await Co::WaitWhile([this] {
			return characters.any([](const Chara& c) { return c.isAnimating(); });
		});
		co_return true;
	}

	case 3: // @visible — アニメーション完了まで待機
	{
		bool visible = (order[2] == U"YES");
		for (auto& chara : characters)
		{
			if (chara.judgeName(order[1]))
			{
				chara.setVisible(visible);
				break;
			}
		}
		co_await Co::WaitWhile([this] {
			return characters.any([](const Chara& c) { return c.isAnimating(); });
		});
		co_return true;
	}

	case 4: // @select — 選択されるまで待機
	{
		Skip_ = false;
		saveLine = scriptLine;
		m_box.SwitchOFF();
		selecting = true;
		targets.clear();
		Array<String> selects;
		for (size_t i = 1; i <= 2; i++)
		{
			Array<String> c_split = messages[scriptLine + i].split(colon);
			targets << c_split[0];
			selects << c_split[1];
		}
		s_box.setSelects(selects);
		co_await Co::WaitWhile([this] { return selecting; });
		co_return false;
	}

	case 5: // @jump
	{
		String goal = landmark + order[1];
		while (messages[scriptLine] != goal)
			++scriptLine %= messages.size();
		co_return true;
	}

	case 6: // @l_effect
	{
		if (order[1] == U"YES")
		{
			LFflag = true;
			l_effect.init();
		}
		else
		{
			LFflag = false;
		}
		co_return true;
	}

	case 7: // @layer
	{
		const int32 num = Parse<int32>(order[2]);
		int32 i = 0;
		for (auto& chara : characters)
		{
			if (chara.judgeName(order[1]))
			{
				chara.changeFlag();
				Chara tmp = chara;
				characters.erase(characters.begin() + i);
				const int32 insertPos = Min(num, static_cast<int32>(characters.size()));
				characters.insert(characters.begin() + insertPos, tmp);
				break;
			}
			i++;
		}
		co_return true;
	}

	case 8: // @move（未実装）
	{
		co_return true;
	}

	case 9: // @end
	{
		bgm.Stop();
		End = true;
		co_return true;
	}

	case 10: // @shake — 揺れが収まるまで待機
	{
		shake = true;
		co_await Co::WaitWhile([this] { return transition.value() > 0.001; });
		co_return true;
	}

	case 11: // @bg_image
	{
		bg.Change(order[1]);
		co_return true;
	}

	case 12: // @camera（未実装）
	{
		co_return true;
	}

	case 13: // @bgm
	{
		if (order[1] == U"OFF")
			bgm.Stop();
		else
			bgm.Change(order[1]);
		co_return true;
	}

	case 14: // @script
	{
		int32 num = 0;
		if (order.size() > 2 && order[2]) num = Parse<int32>(order[2]);
		this->setFile(order[1], num);
		co_return true;
	}

	case 15: // @se
	{
		SE_flag = true;
		SE_name = order[1];
		co_return true;
	}

	case 16: // @cg
	{
		if (order[1] == U"OFF")
			cg.setVisible(false);
		else
		{
			cg.change(order[1]);
			cg.setVisible(true);
		}
		co_return true;
	}

	case 17: // @movie（未実装）
	{
		co_return true;
	}

	default:
	{
		co_return true;
	}
	}
}
