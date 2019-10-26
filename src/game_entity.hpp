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

auto to_string(entity_type type) -> std::string_view;

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

// Create an alias to type less angle brackets
using game_entities_list = std::vector<game_entity>;

struct room;

// Free function that will generate all the enemies in each of the rooms on random
auto generate_enemies(const std::vector<room> &rooms) -> std::vector<game_entity>;

// check the entities list to see if any of them are in 
// position provided, if so return it's iterator idx.
auto get_entity_at(const position &p, const game_entities_list &entities) -> game_entities_list::const_iterator;