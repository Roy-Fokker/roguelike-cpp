#pragma once

#include "game_entity.hpp"

#include <vector>

class TCODColor;
class TCODMap;

struct fov_map; // Need to forward declare for game_map to use

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
	bool was_explored; // Did player see this tile?

	// Returns tile color based on type and is_lit value
	auto color(bool is_lit = false) const -> TCODColor;

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

	void update_explored(const position p, const fov_map &fov);
};

// Field of view map
// It a simple wrapper around TCODMap type.
struct fov_map 
{
	// Delete default constructor, we need a game_map
	fov_map() = delete;
	// Create Field of View based on game_map
	fov_map(const game_map &map);
	~fov_map();

	// Recompute the field of view, using provided 
	// position as center point
	void recompute(const position &p);

	// Is the provided position, within 
	// field of view?
	auto is_visible(const position &p) const -> bool;

private:
	std::unique_ptr<TCODMap> fov;
};

auto generate_map(const dimension size) -> game_map;
