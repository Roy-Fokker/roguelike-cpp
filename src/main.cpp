/*    SPDX-License-Identifier: MIT    */

#include "console.hpp"     // for our console_root and console_layer classes
#include "game_entity.hpp" // for out game_entity class

#include <libtcod.hpp>  // Provided by CMake from external\libtcod-1.14.0-x86_64-msvc\include
#include <string>       // for std::string_view, ""sv
#include <utility>      // for std::pair
#include <cassert>      // for assert()
#include <any>          // for std::any and std::any_cast<>

using namespace std::literals; // ensure we can use ""sv

// Actions user/player can do within context of the game
enum class actions
{
	do_nothing,        // user doesn't do anything
	exit,              // user wants to exit the game
	move,              // user wants to move
	fullscreen_toggle, // user wants game switch between fullscreen and windowed
};

// Check which key was pressed, and return a pair object with action requested and additional data.
auto handle_input() -> std::pair<actions, std::any>
{
	using key_code = TCOD_keycode_t;                // Alias TCOD_keycode_t to key_code
	auto key = TCODConsole::checkForKeypress();     // check if key was pressed, and if so which

	switch (key.vk)
	{
		case key_code::TCODK_ESCAPE:                // User pressed ESCAPE, wants to exit game
			return {actions::exit, nullptr};
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
				return {actions::fullscreen_toggle, nullptr};
			}
			break;
		}
	}

	return {}; // No key was pressed, return do_nothing.
}

int main()
{
	constexpr auto window_width = 80, 
	               window_height = 50;
	constexpr auto title = "Roguelike Tutorials - Part 2"sv,
	               font_path = "arial10x10.png"sv;
	
	// Root Console, this is the thing that controls the window.
	auto root = console_root(title,
	                         { window_width, window_height },
	                         font_path);
	
	// Console game layer, all game rendering will be done on layers
	auto game_layer = console_layer({ window_width, window_height });

	bool exit_game = false;	             // should we exit the game?
	game_entity player                   // Player Entity object
	{ 
		{ window_width / 2, window_height / 2 },  // Initial Position of Player
		'@',                             // player character looks like @
		TCODColor::white                 // Color of player character
	};
	
	// Loop while window exists and exit_game is not true
	while (not (root.is_window_closed() or exit_game))
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
				// directions the player want to move in
				player.move_by(std::any_cast<position>(action.second));
				break;
			case actions::fullscreen_toggle:
				// Toggle between windowed and fullscreen mode.
				root.toggle_fullscreen();
				break;
			default:
			 	// We forgot to handle some action. DEBUG!
				assert(false);
		}

		game_layer.clear();         // Clear the contents of the layer
		game_layer.draw(player);    // Draw the player to game_console layer

		root.blit(game_layer);  // Apply the game_console layer to Root Console
		root.present();         // Flush the root console to display to screen
	}

	return 0;
}
