# include "Common.hpp"
# include "Title.hpp"
# include "Game.hpp"
# include "Save.hpp"
# include "Load.hpp"
# include "Config.hpp"

//メイン関数
void Main()
{
	Window::SetTitle(U"Delphinium");

	InitGame();

	AssetInit();

	App manager;
	manager.add<Title>(State::Title);
	manager.add<Game>(State::Game);
	manager.add<Save>(State::Save);
	manager.add<Load>(State::Load);
	manager.add<Config>(State::Config);

	manager.init(State::Title);
	
	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
