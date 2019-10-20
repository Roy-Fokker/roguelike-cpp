/*    SPDX-License-Identifier: MIT    */

#include "console.hpp"       // for our console_root and console_layer classes
#include "game_entity.hpp"   // for our game_entity class
#include "game_actions.hpp"  // for our game_actions enum and actions<->entity function
#include "game_map.hpp"      // for out game_map class and tile structure
#include "input_handler.hpp" // for handle_input function

#include <libtcod.hpp>  // Provided by CMake from external\libtcod-1.14.0-x86_64-msvc\include
#include <string>       // for std::string_view, ""sv

#include <cassert>      // for assert()

using namespace std::literals; // ensure we can use ""sv

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

	// Get our map layout from generator
	auto map = generate_map({window_width, window_height});

	game_entity player                   // Player Entity object
	{ 
		map.rooms.at(0).center(),        // Initial Position of Player
		'@',                             // player character looks like @
		TCODColor::white                 // Color of player character
	};

	// Loop while window exists and exit_game is not true
	while (not (root.is_window_closed() or exit_game))
	{
		do_action(handle_input(),
		          player,
		          map,
		          root,
		          exit_game);

		game_layer.clear();      // Clear the contents of the layer

		game_layer.draw(map);    // Draw the map to game_console layer

		game_layer.draw(player); // Draw the player to game_console layer

		root.blit(game_layer);  // Apply the game_console layer to Root Console
		root.present();         // Flush the root console to display to screen
	}

	return 0;
}
