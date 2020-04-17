#include "Game.h"



int main(int argc, char* argv[])

{
	auto game{ std::make_unique<Game>() };

	while (game->isRunning())
	{
		game->handle_events();
		game->handle_inputs();
		game->update();
		game->render();
	}
	return 0;

}