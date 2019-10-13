#include "game_map.hpp"

#include <libtcod.hpp>

namespace
{
	// set up some static colors to use for ground and wall
	static const auto ground_color = TCODColor(0, 0, 100);
	static const auto wall_color = TCODColor(50, 50, 150);
} 


auto game_map::is_blocked(position p) const -> bool
{
	auto [x, y] = p;
	auto [w, h] = size;
	if (x >= w       // Make sure we are within the
		or x < 0     // map boundaries
		or y >= h
		or y < 0)
	{                // if not within
		return true; // it's a blocked area
	}

	return tiles.at(x + w * y).blocked; // Simply return value in position's index.
}

// Make our game_map. 
// This version is pretty dumb. It put 2 walls on the Map
auto generate_map(map_size size) -> game_map
{
	auto [w, h] = size;
	auto tiles = std::vector<tile>{};

	for (auto i=0; i < (w * h); i++)
	{
		tiles.push_back({
			{ i % w, i / w },   // Populate x and y positions
			false, false,       // Make tile non-blocking
			ground_color        // Default is ground type
		});
	}

	// Lambda function to put a a blocking tile
	// in position provided.
	auto put_blocking_tile = [&](position p)
	{
		auto i = p.x + size.width * p.y;  // Figure out the index of this tile
		tiles[i].blocked = true;          // Set it to be blocking
		tiles[i].blocks_sight = true;
		tiles[i].color = wall_color;      // Set color to wall type
	};

	// First of two walls in our map
	put_blocking_tile({30, 22});
	put_blocking_tile({31, 22});
	put_blocking_tile({32, 22});

	// Second wall
	put_blocking_tile({40, 28});
	put_blocking_tile({41, 28});
	put_blocking_tile({42, 28});

	return {size, tiles}; // Return a map structure;
}