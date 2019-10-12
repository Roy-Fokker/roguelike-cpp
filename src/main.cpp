/*    SPDX-License-Identifier: MIT    */

#include "console.hpp"     // for our console_root and console_layer classes
#include "game_entity.hpp" // for out game_entity class

#include <libtcod.hpp>  // Provided by CMake from external\libtcod-1.14.0-x86_64-msvc\include
#include <string>       // for std::string_view, ""sv
#include <utility>      // for std::pair
#include <cassert>      // for assert()
#include <any>          // for std::any and std::any_cast<>
#include <vector>

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

// Simple Tile structure
// default values is {0, 0}, false, false
struct tile
{
	position p;
	bool blocked;
	bool blocks_sight;
};

// Very simple map structure.
// it has size of the map, and tiles for each
// cell within the map
struct game_map
{
	int width, height;
	std::vector<tile> tiles;

	// Check if position provided is blocking tile
	auto is_blocked(position p) const -> bool
	{
		auto [x, y] = p;
		if (x >= width       // Make sure we are within the
			or x < 0         // map boundaries
			or y >= height
			or y < 0)
		{                    // if not within
			return true;     // it's a blocked area
		}

		return tiles.at(x + width * y).blocked; // Simply return value in position's index.
	}
};

// Make our game_map. 
// This version is pretty dumb. It put 2 walls on the Map
auto generate_map(int width, int height) -> game_map
{
	auto map = game_map{width, height };  // Initialize the map
	map.tiles.resize(width * height);     // Make sure we have enough tiles

	for (auto i = 0; i < map.tiles.size(); i++)  // For each tile in the map
	{
		auto &t = map.tiles[i];           // Get a reference to a Tile at index i
		t.p = {i % width,                 // Populate x and 
		       i / width};                // y positions
	}

	// Make the tile in position provided 
	// a blocking tile.
	auto put_blocking_tile = [&](position p)
	{
		map.tiles[p.x + width * p.y].blocked = true;
		map.tiles[p.x + width * p.y].blocks_sight = true;
	};

	// First of two walls in our map
	put_blocking_tile({30, 22});
	put_blocking_tile({31, 22});
	put_blocking_tile({32, 22});

	// Second wall
	put_blocking_tile({40, 28});
	put_blocking_tile({41, 28});
	put_blocking_tile({42, 28});

	return map;
}

// Quick and Dirty draw map function.
// Takes a game_map to draw on console_layer
auto draw_map(console_layer &layer, game_map &map)
{
	// set up some static colors to use for ground and wall
	static const auto ground_color = TCODColor(0, 0, 100);
	static const auto wall_color = TCODColor(50, 50, 150);

	// Go through all the tiles
	for(auto &t : map.tiles)
	{
		if (t.blocks_sight)                // If it blocks sight
		{
			layer.draw(t.p, wall_color);   // Draw tile with wall_color
		}
		else
		{
			layer.draw(t.p, ground_color); // Draw tile with ground_color
		}
		
	}
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
		{ window_width / 2,              // Initial Position of Player
		  window_height / 2 },
		'@',                             // player character looks like @
		TCODColor::white                 // Color of player character
	};

	// Get our map layout from generator
	auto map = generate_map(window_width, window_height);
	
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
			{
				// directions the player want to move in
				auto offset = std::any_cast<position>(action.second);

				// Can the player move there?
				if (not map.is_blocked({player.pos.x + offset.x, player.pos.y + offset.y}))
				{
					// Yes
					player.move_by(offset);
				}
				break;
			}
			case actions::fullscreen_toggle:
				// Toggle between windowed and fullscreen mode.
				root.toggle_fullscreen();
				break;
			default:
			 	// We forgot to handle some action. DEBUG!
				assert(false);
		}

		game_layer.clear();         // Clear the contents of the layer

		draw_map(game_layer, map);  // Draw the map to game_console layer

		game_layer.draw(player);    // Draw the player to game_console layer

		root.blit(game_layer);  // Apply the game_console layer to Root Console
		root.present();         // Flush the root console to display to screen
	}

	return 0;
}
