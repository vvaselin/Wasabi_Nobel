# pragma once
# include "core/Common.hpp"
# include "core/ScriptManager.hpp"

class Editor : public App::Scene
{
public:
	Editor(const InitData& init);

	void update() override;

	void draw() const override;
private:

	ScriptManager s_manager;

	TextEditState textEditState;

	ListBoxState listBoxState;

	Optional<FilePath> path;

	Array<String> lists;
};
