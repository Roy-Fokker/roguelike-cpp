#include <libtcod.hpp>
#include <string>
#include <utility>
#include <cassert>
#include <any>

using namespace std::literals;

using console = TCODConsole;

enum class actions
{
	do_nothing,
	exit,
	move,
	fullscreen_toggle,
};

using move_dir = std::pair<int, int>;

auto handle_input() -> std::pair<actions, std::any>
{
	using key_code = TCOD_keycode_t;
	auto key = console::checkForKeypress();

	switch (key.vk)
	{
		case key_code::TCODK_ESCAPE:
			return {actions::exit, 0};
		case key_code::TCODK_UP:
			return {actions::move, move_dir{0, -1}};
		case key_code::TCODK_DOWN:
			return {actions::move, move_dir{0, 1}};
		case key_code::TCODK_LEFT:
			return {actions::move, move_dir{-1, 0}};
		case key_code::TCODK_RIGHT:
			return {actions::move, move_dir{1, 0}};
	}

	return {}; // will automatically return actions::do_nothing
}

int main()
{
	constexpr auto window_width  = 80,
	               window_height = 50;
	constexpr auto window_title = "Rogue C++ Tutorial - Part 1"sv;

	console::initRoot(window_width, window_height,
	                  window_title.data(),
	                  false, TCOD_RENDERER_SDL2);
	
	bool exit_game = false;
	int player_x = 40, 
	    player_y = 25;
	while (not (console::isWindowClosed() or exit_game))
	{
		auto action = handle_input();
		switch (action.first)
		{
			case actions::do_nothing:
				break;
			case actions::exit:
				exit_game = true;
				break;
			case actions::move:
			{
				auto dir = std::any_cast<move_dir>(action.second);
				player_x += dir.first;
				player_y += dir.second;
				break;
			}
			default:
				assert(false); // default case, if we forgot to handle some action in future
		}

		console::root->clear();
		console::root->putChar(player_x, player_y, '@');
		console::flush();
	}

	TCOD_quit(); // must call manually for now.

	return 0;
}
