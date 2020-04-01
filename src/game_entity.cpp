/*    SPDX-License-Identifier: MIT    */

#include "game_entity.hpp"
#include "game_map.hpp"

#include <cppitertools/enumerate.hpp>
#include <fmt/format.h>

#include <random>
#include <array>
#include <algorithm>
#include <string_view>
#include <cassert>

using namespace std::literals;

namespace
{
	// We only want so-many enemies in a room, max.
	constexpr auto max_enemies_per_room = 5;

	// Visual representation of each type of character we have
	// Order maps to entity_type enum.
	const auto entity_faces = std::array{
		std::pair{ '@', TCODColor{ 255, 255, 255 } },
		std::pair{ 'o', TCODColor{ 255, 197,   0 } },
		std::pair{ 'g', TCODColor{  18, 102,   0 } },
	};

	const auto entity_fov = std::array{
		10,
		10,
		10
	};
}

auto to_string(species type) -> std::string_view
{
	switch (type)
	{
	case species::ogre:
		return "ogre"sv;
	case species::goblin:
		return "goblin"sv;
	case species::player:
		return "you"sv;
	default:
		assert(false); // missed an character type?
	}

	assert(false); // how'd you get here
}

void game_entity::move_by(const position &offset)
{
	pos.x += offset.x;
	pos.y += offset.y;
}

auto game_entity::face() const -> std::pair<char, TCODColor>
{
	// The face from our list of faces at index
	auto ent_face = entity_faces.at(static_cast<int>(type));

	if (stats.hitpoints_remaining <= 0)
	{
		ent_face.second = TCODColor{ 255, 0, 0 };
	}
	
	return ent_face;
}

auto game_entity::fov_radius() const -> int
{
	return entity_fov.at(static_cast<int>(type));
}

auto generate_enemies(const std::vector<room> &rooms) -> std::vector<game_entity>
{
	// List of enemies to return
	auto enemies = std::vector<game_entity>{};

	std::random_device rd{};      // Random number generator
	std::mt19937 gen(rd());       // generation algorithm we want to use
	std::uniform_int_distribution d_en(0, max_enemies_per_room); // distribution for number of enemies per room.
	std::bernoulli_distribution d_et(0.8); // goblins or ogres distribution. 80% goblins, 20% ogres

	// Lambda will add to enemies list for a given room
	auto place_enemies_in_room = [&](const room &r, const int count)
	{
		// thing to keep in mind, for a given room
		// +#####w the enemies can only exist in the
		// #*   #| blank spaces inside. But the room's
		// #   ~#| dimensions include the walls.
		// ######| Hence why x1 and y1 are +1 (*),
		// ------h and x2 and y2 are -2 (~).
		auto x1 = r.p.x + 1, x2 = r.p.x + r.size.width - 2;
		auto y1 = r.p.y + 1, y2 = r.p.y + r.size.height - 2;
		std::uniform_int_distribution d_x(x1, x2), // X and Y distribution
		                              d_y(y1, y2); // for enemies
		
		for (auto start_cnt = enemies.size();        // Take enemy count till now
		     enemies.size() < (start_cnt + count); ) // loop till it's increased by count.
		{
			int x = d_x(gen), // x and y coordinates
			    y = d_y(gen); // to put the enemy at.
			
			// Found another enemy at same spot?
			if (get_entity_at({x, y}, enemies) != std::end(enemies))
				continue;	// go back and get another position
			
			// Figure out what's the type of this enemy.
			auto type = d_et(gen) ? species::goblin : species::ogre;
			enemies.push_back({
				{x, y},
				type
			});
		}
	};

	// Loop through all the rooms
	for (auto &r : rooms)
	{
		// For this room generate random number of enemies.
		place_enemies_in_room(r, d_en(gen));
	}

	// For each enemy generate some random
	// vitals 
	std::uniform_int_distribution d_mhp(1, 10);
	std::uniform_int_distribution d_pow(2, 6);
	std::uniform_int_distribution d_def(1, 5);
	for (auto [i, e] : enemies | iter::enumerate)
	{
		using namespace fmt::literals;

		auto hp = d_mhp(gen);
		e.stats = { hp, hp, d_def(gen), d_pow(gen) };
		e.name = "{} {}"_format(to_string(e.type), i+1);
	}

	return enemies;
}

auto get_entity_at(const position &p, const game_entities_list &entities) -> game_entities_list::const_iterator
{
	return std::find_if(std::begin(entities), std::end(entities), [&](const auto &e)
	{
		return (e.stats.hitpoints_remaining > 0) 
		   and (p.x == e.pos.x) and (p.y == e.pos.y);
	});
}