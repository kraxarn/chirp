#include "game.hpp"

class editor: public chirp::game
{
	void load_content() override
	{
	}

	void update(float delta) override
	{
	}

	void draw() override
	{
	}
};

auto main(int argc, char **argv) -> int
{
	editor editor;
	editor.run();
	return 0;
}
