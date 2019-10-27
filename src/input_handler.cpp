#include "input_handler.hpp"
#include "game_entity.hpp"

#include <libtcod.hpp>

auto handle_input() -> action_data_pair const
{
	using key_code = TCOD_keycode_t;                // Alias TCOD_keycode_t to key_code
	auto key = TCODConsole::checkForKeypress();     // check if key was pressed, and if so which

	switch (key.vk)
	{
		case key_code::TCODK_ESCAPE:                // User pressed ESCAPE, wants to exit game
			return {system_actions::exit, nullptr};
		case key_code::TCODK_UP:                    // User pressed UP arrow, wants to move up
			return {actions::move, position{0, -1}};
		case key_code::TCODK_DOWN:                  // User pressed DOWN arrow, wants to move down
			return {actions::move, position{0, 1}};
		case key_code::TCODK_LEFT:                  // User pressed LEFT arrow, wants to move left
			return {actions::move, position{-1, 0}};
		case key_code::TCODK_RIGHT:                 // User pressed RIGHT arrow, wants to move right
			return {actions::move, position{1, 0}};
		case key_code::TCODK_ENTER:                 // User pressed ENTER
		{
			if (key.lalt or key.ralt)               // and ALT key, toggle fullscreen mode
			{
				return {system_actions::fullscreen_toggle, nullptr};
			}
			break;
		}
	}

	return {}; // No key was pressed, return do_nothing.
}
