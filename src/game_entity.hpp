/*    SPDX-License-Identifier: MIT    */

#pragma once

#include <libtcod.hpp>
#include <cstdint>
#include <vector>
#include <utility>

// Simple Position structure to hold x and y values.
// It will represent positions of various objects in game world.
struct position
{
	int32_t x,
	        y;
};

// List of different types of characters that can exists 
// in the game.
enum class entity_type
{
	player,
	ogre,
	goblin
};

// Entity structure represent any object in game world
// It fully describes the object and how to move it.
struct game_entity
{
	position pos;
	entity_type type;

	// Move the entity by offset value
	void move_by(const position &offset);

	// Face is color and visual of this entity
	auto face() const -> std::pair<char, TCODColor>;
};

struct room;

// Free function that will generate all the enemies in each of the rooms on random
auto generate_enemies(const std::vector<room> &rooms) -> std::vector<game_entity>;

// check the entities list to see if any of them are in 
// position provided.
auto is_blocked(const position &p, const std::vector<game_entity> &entities) -> bool;