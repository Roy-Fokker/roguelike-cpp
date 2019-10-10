#pragma once

#include <libtcod.hpp>
#include <cstdint>

// Simple Position structure to hold x and y values.
// It will represent positions of various objects in game world.
struct position
{
	int32_t x,
	        y;
};

// Entity structure represent any object in game world
// It fully describes the object and how to move it.
struct game_entity
{
	position pos;
	char chr;
	TCODColor color;

	// Move the entity by offset value
	void move_by(const position &offset);
};