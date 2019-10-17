#include "game_map.hpp"

#include <libtcod.hpp>
#include <enumerate.hpp>
#include <range.hpp>

#include <random>
#include <algorithm>
#include <cassert>

namespace
{
	// set up some static colors to use for ground and wall
	static const auto ground_color = TCODColor(0, 0, 100);
	static const auto tunnel_color = TCODColor(25, 25, 125);
	static const auto wall_color = TCODColor(50, 50, 150);

	constexpr auto max_rooms = 20;
	constexpr auto min_room_size = 5;
	constexpr auto max_room_size = 20;


	// Makes rooms of various sizes in the provided tile
	// array. Ensures rooms don't overlap. 
	// Returns array of rooms it created.
	auto make_rooms(const dimension size, std::vector<tile> &tiles) -> std::vector<room>
	{
		// change the tiles to be of ground type. 
		// assumes r is within bounds of size
		auto change_tiles = [&](room r)
		{
			auto x1 = r.p.x,
			     y1 = r.p.y;
			auto x2 = r.p.x + r.size.width - 1,  // figure out x2, and y2 position
			     y2 = r.p.y + r.size.height - 1; // range is 0 to (dimension - 1)
			
			for (auto y : iter::range(y1, y2))
			{
				auto start_idx = x1 + size.width * y,
				     end_idx = x2 + size.width * y;
			
				for (auto i : iter::range(start_idx, end_idx))
				{
					tiles[i].type = tile_type::ground;
				}
			}
		};

		std::random_device rd{};      // Random number generator
		std::mt19937 gen(rd());       // generation algorithm we want to use
		std::uniform_int_distribution d_rs(min_room_size, max_room_size), // distribution for room size
		                              d_rx(0, size.width - min_room_size - 1), // x and y position of the room
		                              d_ry(0, size.height - min_room_size - 1);// should be max size minus room size

		// Makes a random room within our map size
		auto make_random_room = [&]() -> room
		{
			auto w = d_rs(gen),
			     h = d_rs(gen),
			     x = d_rx(gen),
			     y = d_ry(gen);
			
			w = std::clamp(w + x, x, size.width - 1) - x; // clamp the output so 
			h = std::clamp(h + y, y, size.height - 1) - y; // we don't exceed the map size

			return {x, y, w, h};
		};

		std::vector<room> rooms{};

		while (rooms.size() < max_rooms)
		{
			auto room = make_random_room();  // Generate a room

			// Check to see if this room intersects with any other room
			auto exists = std::find_if(std::begin(rooms), std::end(rooms), [&](const auto &r)
			{
				return room.intersects(r);
			});
			
			// if it does not intersect, then add it to our list
			if (exists == std::end(rooms))
			{
				rooms.push_back(room);
			}
		}

		// Update the tiles for all the 
		// room we have so far.
		for (auto &r : rooms)
		{
			change_tiles(r);
		}

		return rooms;

		// change_tiles({20, 15, 10, 15});
		// change_tiles({35, 15, 10, 15});
		// change_tiles({10, 10, 5, 5});
		
		// return {
		// 	{20, 15, 10, 15},
		// 	{35, 15, 10, 15},
		// 	{10, 10, 5, 5},
		// };
	}

	// Connect all the rooms in order they got added to the list
	void connect_rooms(const dimension size, const std::vector<room> &rooms, std::vector<tile> &tiles)
	{

	}


	// // Connect all the rooms in order they got added to the list
	// void connect_rooms(const map_size size, const std::vector<room> &rooms, std::vector<tile> &tiles)
	// {
	// 	auto create_tunnel = [&](const room &r1, const room &r2)
	// 	{
	// 		auto [x1, y1] = r1.r.center();
	// 		auto [x2, y2] = r2.r.center();

	// 		auto dig_tunnel = [&](int idx)
	// 		{
	// 			if (not tiles[idx].blocked)
	// 				return;

	// 			tiles[idx].blocked = false;         // unblock this tile
	// 			tiles[idx].blocks_sight = false;
	// 			tiles[idx].color = tunnel_color;
	// 		};

	// 		auto horizontal_tunnel = [&](bool min_max)
	// 		{
	// 			auto x_start = std::min(x1, x2);
	// 			auto x_end = std::max(x1, x2);
	// 			auto y = (min_max) ? std::min(y1, y2) : std::max(y1, y2);

	// 			for (int x = x_start; x < x_end; x++)
	// 			{
	// 				auto idx = x + size.width * y;
	// 				dig_tunnel(idx);
	// 			}
	// 		};

	// 		auto vertical_tunnel = [&](bool min_max)
	// 		{
	// 			auto y_start = std::min(y1, y2);
	// 			auto y_end = std::max(y1, y2);
	// 			auto x = (min_max) ? std::min(x1, x2) : std::max(x1, x2);

	// 			for (int y = y_start; y < y_end; y++)
	// 			{
	// 				auto idx = x + size.width * y;
	// 				dig_tunnel(idx);
	// 			}
	// 		};
			
	// 		std::random_device rd{};
	// 		std::mt19937 gen(rd());
	// 		std::bernoulli_distribution d(0.5);
			
	// 		auto min_max = d(gen);
	// 		horizontal_tunnel(min_max);
	// 		vertical_tunnel(not min_max);
	// 	};

	// 	for (int i = 1; i < rooms.size(); i++)
	// 	{
	// 		create_tunnel(rooms[i-1], rooms[i]);
	// 	}
	// }
}

auto tile::is_blocked() const -> bool
{
	if (type == tile_type::wall)
	{
		return true;
	}

	return false;
}

auto tile::color() const -> TCODColor
{
	switch (type)
	{
		case tile_type::wall:
			return wall_color;
		case tile_type::ground:
			return ground_color;
		case tile_type::tunnel:
			return tunnel_color;
		default:
			assert(false);
			break;
	}
	assert(false); // how'd you end up here.
}

auto room::center() const -> position
{
	return {
		p.x + (size.width / 2),
		p.y + (size.height / 2)
	};
}

auto room::intersects(const room &other) const -> bool
{
	return not (
	    p.x + size.width < other.p.x 
	    or other.p.x + other.size.width < p.x
	    or p.y + size.height < other.p.y
	    or other.p.y + other.size.height < p.y);
}

auto game_map::is_blocked(const position p) const -> bool
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

	return tiles.at(x + w * y).is_blocked(); // Simply return value in position's index.
}

// Make our game_map. 
// Version 2 of our generator. Makes Rooms and Corridors.
auto generate_map(const dimension size) -> game_map
{
	auto [w, h] = size;               // bind size members to they are easier to use.
	auto tiles = std::vector<tile>{}; // create our tiles array
	tiles.resize(w * h);              // resize it to match our size
	for (auto [i, t] : iter::enumerate(tiles)) // enumerate using cppitertools
	{
		t = { (int)i % w, (int)i / w, // populate x and y values of position
		      tile_type::wall};       // set default tile type
	}

	auto rooms = make_rooms(size, tiles);
	connect_rooms(size, rooms, tiles);
	// auto rooms = make_rooms(size, tiles);
	// connect_rooms(size, rooms, tiles);

	return {size, tiles, rooms}; // Return a map structure;
}