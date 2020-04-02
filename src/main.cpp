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
	constexpr auto title = "Roguelike Tutorials - Part 5"sv,
	               font_path = "arial10x10.png"sv;
	
	// Root Console, this is the thing that controls the window.
	auto root = console_root(title,
	                         { window_width, window_height },
	                         font_path);
	
	// Height of UI layer
	constexpr auto ui_height = 15;
	// Location were map layer ends and ui layer starts.
	constexpr auto map_ui_split_at = window_height - ui_height;

	// Console game layer, all game rendering will be done on layers
	// This layer is for map and entities.
	auto map_layer = console_layer({ window_width, map_ui_split_at });
	// This layer is for UI elements
	auto ui_layer = console_layer({ window_width, ui_height }, { 0, map_ui_split_at });

	bool exit_game = false;	        // should we exit the game?

	// Get our map layout from generator
	auto map = generate_map({window_width, map_ui_split_at});
	// Use the map to generate a Field of View
	auto fov = fov_map(map);

	// Get location and types of all the enemies
	auto entities = generate_enemies(map.rooms);

	// Add a player entity to end of our entities list
	entities.push_back({
		map.rooms.at(0).center(),     // Initial Position of Player
		species::player,              // type of entity
		{ 10, 10, 5, 6 },             // some basic stats
		"Player"                      // name of the player
	});
	auto &player = entities.back();  // Reference to Player Entity object

	fov.recompute(player.pos);            // Compute the starting Field of View
	map.update_explored(player.pos, fov); // Update initial exploration state

	// Loop while window exists and exit_game is not true
	while (not (root.is_window_closed() or exit_game))
	{
		auto action_data = handle_input();

		do_system_action(action_data,
		                 root,
		                 exit_game);

		take_turns(action_data, 
		           entities,
		           map,
		           fov);

		map_layer.clear();              // Clear the contents of the Map layer
		map_layer.draw(map, fov);       // Draw the rooms and corridors to map layer
		map_layer.draw(entities, fov);  // Draw all the entities to map layer

		ui_layer.clear();        // Clear the contents of the UI Layer

		root.blit(map_layer);   // Apply the map layer to Root Console
		root.blit(ui_layer);    // Apply the UI layer to Root Console
		root.present();         // Flush the root console to display to screen
	}

	return 0;
}
