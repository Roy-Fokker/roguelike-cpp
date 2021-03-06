/*    SPDX-License-Identifier: MIT    */

#pragma once

#include <libtcod.hpp>
#include <cstdint>
#include <vector>
#include <utility>
#include <string>

struct cell;    // Forward declare to avoid including console.hpp.
struct fov_map; // Forward declare to avoud including game_map.hpp.

// Simple Position structure to hold x and y values.
// It will represent positions of various objects in game world.
struct position
{
	int32_t x,
	        y;
};

// List of different types of characters that can exists 
// in the game.
enum class species
{
	player,
	ogre,
	goblin
};

// Translate species enum into string
auto to_string(species type) -> std::string_view;

// each entity will have some stats, like hp, attack etc
// this structure will hold those values
struct vitals
{
	int max_hitpoints;
	mutable int hitpoints_remaining;
	int defense;
	int power;
};

// Entity structure represent any object in game world
// It fully describes the object and how to move it.
struct game_entity
{
	position pos;
	species type;
	vitals stats;
	std::string name;

	// Move the entity by offset value
	void move_by(const position &offset);

	// Face is color and visual of this entity
	auto face() const -> std::pair<char, TCODColor>;

	// Field of view range
	auto fov_radius() const -> int;
};

// Create an alias to type less angle brackets
using game_entities_list = std::vector<game_entity>;

// Forward declare room, defined in game_map[.cpp|.hpp]
struct room;

// Free function that will generate all the enemies in each of the rooms on random
auto generate_enemies(const std::vector<room> &rooms) -> std::vector<game_entity>;

// check the entities list to see if any of them are in 
// position provided, if so return it's iterator idx.
auto get_entity_at(const position &p, const game_entities_list &entities) -> game_entities_list::const_iterator;

// Take entities list and fov provided, and convert them into 
// console-cells so that console layer can draw them out.
auto prepare_to_draw(const game_entities_list &entities, const fov_map &fov) -> std::vector<cell>;