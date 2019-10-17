#pragma once

#include "game_entity.hpp"

#include <vector>

class TCODColor;

enum class tile_type
{
	wall,
	ground,
	tunnel
};

// Simple Tile structure
struct tile
{
	position p;        // Location of this tile
	tile_type type;    // Type of this tile

	// Returns tile color based on type
	auto color() const -> TCODColor;

	// Returns if tile blocks based on type
	auto is_blocked() const -> bool;
};

// Structure to hold size of the map
struct dimension
{
	int width, 
	    height;
};

// Simple room structure
struct room
{
	position p;
	dimension size;

	// get center of the room
	auto center() const -> position;

	// do the rooms intersect
	auto intersects(const room &other) const -> bool;
};

// Very simple map structure.
// it has size of the map, and tiles for each
// cell within the map
struct game_map
{
	dimension size;
	std::vector<tile> tiles;
	std::vector<room> rooms;

	// Check if position provided is blocking tile
	auto is_blocked(const position p) const -> bool;
};

auto generate_map(const dimension size) -> game_map;