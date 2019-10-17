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

	constexpr auto max_rooms = 10;
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
		// Turn tile in to a hallway/tunnel tile, if 
		// it's not already a ground tile.
		auto make_hallway = [&](int x, int y)
		{
			auto idx = x + size.width * y;
			assert(idx < tiles.size());

			if (tiles[idx].type != tile_type::ground)
				tiles[idx].type = tile_type::tunnel;
		};

		// Make Vertical Hallway/Tunnels between two rooms
		auto vertical_hallway = [&](bool top_bottom, const room &room1, const room &room2)
		{
			auto r1 = (top_bottom) ? room1 : room2, // if top is primary make it r1
			     r2 = (top_bottom) ? room2 : room1; // else make it r2
			auto [x, y1] = r1.center();
			auto y2 = r2.center().y;
			
			if (y1 > y2)           // iter::range requires starting number 
			{                      // be lower than ending number
				std::swap(y1, y2); // so flip them if they aren't
			}

			// only change the Y axis value to draw the tunnel.
			for (auto y : iter::range(y1, y2))
			{
				make_hallway(x, y);
			}
		};

		// Make horizontal Hallway/Tunnels between two rooms
		// basically a copy of horizontal version.
		auto horizontal_hallway = [&](bool left_right, const room &room1, const room &room2)
		{
			auto r1 = (left_right) ? room1 : room2,
			     r2 = (left_right) ? room2 : room1;
			auto [x1, y] = r1.center();
			auto x2 = r2.center().x;
			
			if (x1 > x2)
			{
				std::swap(x1, x2);
			}

			// unlike vertial verison, this one works with only changes x axis
			for (auto x : iter::range(x1, x2 + 1))
			{
				make_hallway(x, y);
			}
		};

		// Random rumber to determine where the tunnel will start from
		std::random_device rd{};
		std::mt19937 gen(rd());
		std::bernoulli_distribution d(0.5);
	
		for (auto i = 1; i < rooms.size(); i++)
		{
			auto flip = d(gen);  // pick which room is primary

			horizontal_hallway(flip, rooms[i - 1], rooms[i]);   // make a hall from primary to secondary
			vertical_hallway(not flip, rooms[i - 1], rooms[i]); // start vertical hall from where horizontal stopped.
		}
	}
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