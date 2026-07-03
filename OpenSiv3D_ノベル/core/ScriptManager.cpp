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

bool ScriptManager::isWaitToken(const String& s) const
{
	return (s == U"wait" || s == U"WAIT" || s == U"Wait");
}

bool ScriptManager::parseWaitFlag(const Array<String>& order, size_t index, bool defaultWait) const
{
	if (order.size() <= index)
	{
		return defaultWait;
	}

	return isWaitToken(order[index]);
}

Chara* ScriptManager::findChara(const String& name)
{
	for (auto& chara : characters)
	{
		if (chara.judgeName(name))
		{
			return &chara;
		}
	}

	return nullptr;
}

Chara::MovePattern ScriptManager::parseMovePattern(const String& s) const
{
	if (s == U"Step") return Chara::MovePattern::Step;
	if (s == U"Jump") return Chara::MovePattern::Jump;
	if (s == U"BackLeft") return Chara::MovePattern::BackLeft;
	if (s == U"BackRight") return Chara::MovePattern::BackRight;

	return Chara::MovePattern::Default;
}

bool ScriptManager::anyBlockingChara() const
{
	return characters.any([](const Chara& c) { return c.isBlocking(); });
}

// -------------------------------------------------------
// コマンド実行（統合版）
// -------------------------------------------------------
Optional<Command> ScriptManager::parseCommandLine(const String& line) const
{
	if (line.isEmpty() || line[0] != atMark)
	{
		return none;
	}

	Array<String> tokens = line.split(colon);
	if (tokens.isEmpty())
	{
		return none;
	}

	tokens[0].pop_front();

	Command command;
	command.name = tokens[0];
	for (size_t i = 1; i < tokens.size(); ++i)
	{
		command.args << tokens[i];
	}

	return command;
}

Array<String> ScriptManager::toLegacyOrder(const Command& command) const
{
	Array<String> order{ command.name };
	for (const auto& arg : command.args)
	{
		order << arg;
	}
	return order;
}

bool ScriptManager::executeCommand(const Command& command, ExecMode mode, int32 targetLine)
{
	if (!Orders.contains(command.name))
	{
		Logger << U"[warning] ScriptManager: unknown command: " << command.name;
		return true;
	}

	Array<String> order = toLegacyOrder(command);

	switch (Orders[command.name])
	{
	case 1: executeNewChara(order); break;
	case 2: executeChange(order, mode); break;
	case 3: executeVisible(order, mode); break;
	case 4: return executeSelect(order, mode, targetLine);
	case 5: executeJump(order); break;
	case 6: executeLineEffect(order); break;
	case 7: executeLayer(order); break;
	case 8: executeMove(order, mode); break;
	case 9: executeEnd(); break;
	case 10: executeShake(order); break;
	case 11: executeBackground(order); break;
	case 12: executeCamera(order); break;
	case 13: executeBGM(order); break;
	case 14: executeScript(order); break;
	case 15: executeSE(order, mode, targetLine); break;
	case 16: executeCG(order); break;
	case 17: executeMovie(order); break;
	case 18: executeBlackout(order); break;
	case 20: executeMoveTo(order, mode); break;
	case 21: executeFadeTeleport(order, mode); break;
	default: break;
	}

	return true;
}

void ScriptManager::executeNewChara(const Array<String>& order)
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
		const Vec2 pos = posJudge(order[3]);
		const double scale = scaleJudge(order[4]);
		characters << Chara(order[1], order[2], pos, scale, visible);
	}
}

void ScriptManager::executeChange(const Array<String>& order, ExecMode mode)
{
	String portrait = U"NAN";
	Vec2 pos = Vec2{ 0,0 };
	double scale = 0.0;
	Chara::MovePattern movePattern = Chara::MovePattern::Default;

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
				else if (GetScriptConfig().hasPosition(order[i]))
				{
					pos = posJudge(order[i]);
				}
				else if (GetScriptConfig().hasScale(order[i]))
				{
					scale = scaleJudge(order[i]);
				}
				else
				{
					const Chara::MovePattern parsed = parseMovePattern(order[i]);
					if (parsed != Chara::MovePattern::Default)
					{
						movePattern = parsed;
					}
				}
			}

			chara.change(portrait, scale, pos);
			if (mode == ExecMode::Quick)
			{
				chara.applyMovePatternImmediate(movePattern);
			}
			else
			{
				chara.startMove(movePattern, true);
			}
			break;
		}
	}
}

void ScriptManager::executeVisible(const Array<String>& order, ExecMode mode)
{
	const bool visible = (order[2] == U"YES");
	for (auto& chara : characters)
	{
		if (chara.judgeName(order[1]))
		{
			chara.setVisible(visible);
			if (order.size() >= 4)
			{
				const Chara::MovePattern movePattern = parseMovePattern(order[3]);
				if (mode == ExecMode::Quick)
				{
					chara.applyMovePatternImmediate(movePattern);
				}
				else
				{
					chara.startMove(movePattern, true);
				}
			}
			break;
		}
	}
}

bool ScriptManager::executeSelect(const Array<String>&, ExecMode mode, int32 targetLine)
{
	if (mode == ExecMode::Quick && static_cast<int32>(scriptLine) != targetLine)
	{
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

		return true;
	}

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

	return true;
}

void ScriptManager::executeJump(const Array<String>& order)
{
	const String goal = landmark + order[1];
	while (messages[scriptLine] != goal)
	{
		++scriptLine %= messages.size();
	}
}

void ScriptManager::executeLineEffect(const Array<String>& order)
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

void ScriptManager::executeLayer(const Array<String>& order)
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
}

void ScriptManager::executeMove(const Array<String>& order, ExecMode mode)
{
	if (Chara* chara = findChara(order[1]))
	{
		const Chara::MovePattern movePattern = parseMovePattern(order[2]);
		if (mode == ExecMode::Quick)
		{
			chara->applyMovePatternImmediate(movePattern);
		}
		else
		{
			chara->startMove(movePattern, true);
		}
	}
}

void ScriptManager::executeEnd()
{
	bgm.Stop();
	End = true;
}

void ScriptManager::executeShake(const Array<String>& order)
{
	shake = true;
	shakeBlocking = parseWaitFlag(order, 1, true);
}

void ScriptManager::executeBackground(const Array<String>& order)
{
	bg.Change(order[1]);
}

void ScriptManager::executeCamera(const Array<String>&)
{
}

void ScriptManager::executeBGM(const Array<String>& order)
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

void ScriptManager::executeScript(const Array<String>& order)
{
	int32 num = 0;
	if (order.size() > 2 && order[2])
	{
		num = Parse<int32>(order[2]);
	}
	this->setFile(order[1], num);
}

void ScriptManager::executeSE(const Array<String>& order, ExecMode mode, int32 targetLine)
{
	if (mode == ExecMode::Normal || static_cast<int32>(scriptLine) == targetLine)
	{
		SE_flag = true;
		SE_name = order[1];
	}
}

void ScriptManager::executeCG(const Array<String>& order)
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

void ScriptManager::executeBlackout(const Array<String>&)
{
}

void ScriptManager::executeMovie(const Array<String>&)
{
}

void ScriptManager::executeMoveTo(const Array<String>& order, ExecMode mode)
{
	if (Chara* chara = findChara(order[1]))
	{
		const Vec2 to = posJudge(order[2]);
		const double duration = ParseOr<double>(order[3], 0.5);
		const bool wait = parseWaitFlag(order, 4, true);
		if (mode == ExecMode::Quick)
		{
			chara->setPositionImmediate(to);
		}
		else
		{
			chara->moveTo(to, duration, wait);
		}
	}
}

void ScriptManager::executeFadeTeleport(const Array<String>& order, ExecMode mode)
{
	if (Chara* chara = findChara(order[1]))
	{
		const Vec2 to = posJudge(order[2]);
		const double duration = ParseOr<double>(order[3], 0.8);
		const bool wait = parseWaitFlag(order, 4, true);
		if (mode == ExecMode::Quick)
		{
			chara->setPositionImmediate(to);
		}
		else
		{
			chara->fadeTeleportTo(to, duration, wait);
		}
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
			if (const auto command = parseCommandLine(messages[scriptLine]))
			{
				executeCommand(*command, ExecMode::Normal);
			}
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
			if (const auto command = parseCommandLine(messages[scriptLine]))
			{
				executeCommand(*command, ExecMode::Quick, line);
			}
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
	else
	{
		shakeBlocking = false;
	}

	for (auto& chara : characters)
	{
		chara.updateMove();
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
			const auto command = parseCommandLine(messages[scriptLine]);
			const bool advance = command ? co_await executeCommandAsync(*command) : true;
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

Co::Task<bool> ScriptManager::executeCommandAsync(const Command& command)
{
	const bool advance = executeCommand(command, ExecMode::Normal);

	if (command.name == U"select")
	{
		co_await Co::WaitWhile([this] { return selecting; });
		co_return false;
	}

	if (command.name == U"change" || command.name == U"visible")
	{
		co_await Co::WaitWhile([this] {
			return characters.any([](const Chara& c) { return c.isFading(); }) || anyBlockingChara();
		});
	}
	else if (command.name == U"move")
	{
		co_await Co::WaitWhile([this] { return anyBlockingChara(); });
	}
	else if (command.name == U"shake" && shakeBlocking)
	{
		co_await Co::WaitWhile([this] { return shake || transition.value() > 0.001; });
	}
	else if ((command.name == U"move_to" || command.name == U"fade_teleport")
		&& parseWaitFlag(toLegacyOrder(command), 4, true))
	{
		co_await Co::WaitWhile([this] { return anyBlockingChara(); });
	}

	co_return advance;
}
