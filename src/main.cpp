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
		case key_code::TCODK_ENTER:
		{
			if (key.lalt or key.ralt)
			{
				return {actions::fullscreen_toggle, 0};
			}
			break;
		}
	}

	return {}; // will automatically return actions::do_nothing
}

int main()
{
	constexpr auto window_width  = 100,
	               window_height = 56;
	constexpr auto window_title  = "Rogue C++ Tutorial - Part 1"sv,
	               font_file     = "arial10x10.png"sv;

	console::setCustomFont(font_file.data(), TCOD_FONT_TYPE_GREYSCALE | TCOD_FONT_LAYOUT_TCOD);

	console::initRoot(window_width, window_height,
	                  window_title.data(),
	                  false, TCOD_RENDERER_SDL2);
	atexit(TCOD_quit); // the C-Runtime will automatically call this function.
	
	auto game_console = console(window_width, window_height);

	bool exit_game = false;
	int player_x = window_width / 2, 
	    player_y = window_height / 2;
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
			case actions::fullscreen_toggle:
			{
				game_console.setFullscreen(not game_console.isFullscreen());
				break;
			}
			default:
				assert(false); // We forgot to handle some action. DEBUG!
		}

		game_console.clear();
		game_console.setDefaultForeground(TCODColor::white);
		game_console.putChar(player_x, player_y, '@');

		console::blit(&game_console, 0, 0, window_width, window_height, 
		              console::root, 0, 0);
		console::flush();
	}

	return 0;
}
