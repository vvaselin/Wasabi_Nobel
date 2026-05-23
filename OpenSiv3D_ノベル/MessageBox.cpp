#include "MessageBox.hpp"

void MessageBox::setText(const String& name, const String& text)
{
	countStart = 0;
	messageEnd = false;
	Reading = true;
	tmp.clear();
	RubyList.clear();
	i = 0;
	j = 0;

	while (text[i])
	{
		if (text[i] == partition[0]) // "{"だったら
		{
			indexBegin = tmp.size();
			i++;
			r_tmp.clear();
			while (text[i] != partition[1]) // "|"になるまで
			{
				indexEnd = tmp.size();
				tmp << text[i];
				i++; j++;
			}
			i++;
			while (text[i] != partition[2]) // "}"になるまで
			{
				r_tmp << text[i];
				i++;
			}
			ruby = { indexBegin,indexEnd, r_tmp };
			RubyList << ruby;
		}
		else //ruby処理以外
		{
			if ((j % 33 == 0) && j > 1)
			{
				tmp += U"\n";
				j = 0;
			}

			if (text[i] == nLine[0]) //"["があれば
			{
				i++;
				if ((text[i] == nLine[1]) && (text[i + 1] == nLine[2])) //"r]"があれば
				{
					tmp += U"\n";
					i += 2;
					j = 0;
				}
			}

			tmp += text[i];
		}
		i++; j++;
	}
	message = tmp; // 加工した文章を保存
	speaker = name;
	m_stopwatch.restart();
}

void MessageBox::addText(const String& text)
{
	messageEnd = false;
	Reading = true;
	tmp.clear();
	i = 0;
	while (text[i])
	{
		if (text[i] == partition[0]) // "{"だったら
		{
			indexBegin = tmp.size();
			i++;
			r_tmp.clear();
			while (text[i] != partition[1]) // "|"になるまで
			{
				indexEnd = tmp.size();
				tmp << text[i];
				i++; j++;
			}
			i++;
			while (text[i] != partition[2]) // "}"になるまで
			{
				r_tmp << text[i];
				i++;
			}
			ruby = { indexBegin+ countStart-1,indexEnd+ countStart-1, r_tmp };
			RubyList << ruby;
		}
		else //ruby処理以外
		{
			if ((j % 33 == 0) && j > 1)
			{
				tmp += U"\n";
				j = 0;
			}

			if (text[i] == nLine[0]) //"["があれば
			{
				i++;
				if ((text[i] == nLine[1]) && (text[i + 1] == nLine[2])) //"r]"があれば
				{
					tmp += U"\n";
					i += 2;
					j = 0;
				}
			}

			tmp += text[i];
		}
		i++; j++;
	}
	tmp.pop_front();
	message += tmp; // 加工した文章を保存
	m_stopwatch.start();
}

void MessageBox::draw(bool& clicked, const double& speed )
{
	//メッセージボックス
	Rect{ Arg::center(960, 980), 2000, 400 }.draw(Arg::top = ColorF(0.0, 0.7), Arg::bottom = ColorF(0.4, 0.2));
	mainFont(speaker).draw(ShadowPos, ColorF{ 0.0, 0.4 }); //キャラ名に影を入れる
	mainFont(speaker).draw(SpeakerPos);

	if (!isClicked) //一文字ずつ
	{
		const int32 count = Max(((m_stopwatch.ms()) / 50 * int32(Clamp(speed,0.2,1.0)*5)), countStart); //表示する文字数 (ストップウォッチ-遅延)/スピード
		DrawTextWithRuby(Vec2{ 200, 800 }, mainFont, rubyFont, message.substr(0, count), RubyList, RubyYOffset);
		
		isClicked = (clicked && count > 0) || (Reading && count >= message.length());
	}
	else // 文字全表示状態
	{
		m_stopwatch.pause();
		messageEnd = true;
		DrawTextWithRuby(Vec2{ 200, 800 }, mainFont, rubyFont, message, RubyList, RubyYOffset);
		//画面クリックしたら
		if (clicked)
		{
			countStart = message.length();
			isClicked = false;
			const int32 count = 0;
			Reading = false;
		}

		messageCircle();
	}
}

bool MessageBox::isReading()
{
	return Reading;
}

void MessageBox::SwitchOFF()
{
	Reading = false;
}

void MessageBox::SwitchON()
{
	Reading = true;
}

bool MessageBox::getEndFlag() const
{
	return messageEnd;
}
