#pragma once

#include "game_entity.hpp"

#include <vector>

class TCODColor;

// Simple Tile structure
// default values is {0, 0}, false, false
struct tile
{
	position p;        // Location of the tile
	bool blocked;      // Is tile blocking
	bool blocks_sight; // Is tile blocking sight

	TCODColor color;
};

struct map_size
{
	int width, 
	    height;
};

// Very simple map structure.
// it has size of the map, and tiles for each
// cell within the map
struct game_map
{
	map_size size;
	std::vector<tile> tiles;

	// Check if position provided is blocking tile
	auto is_blocked(const position p) const -> bool;
};



auto generate_map(const map_size size) -> game_map;