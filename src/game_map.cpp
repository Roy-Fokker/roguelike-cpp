#include "game_map.hpp"

#include <easy_iterator.h>
#include <libtcod.hpp>
#include <random>
#include <utility>
#include <algorithm>

namespace
{
	// set up some static colors to use for ground and wall
	static const auto ground_color = TCODColor(0, 0, 100);
	static const auto tunnel_color = TCODColor(25, 25, 125);
	static const auto wall_color = TCODColor(50, 50, 150);

	constexpr auto max_rooms = 100;
	constexpr auto min_room_size = 10;
	constexpr auto max_room_size = 30;

	struct rect
	{
		int x, y, w, h;

		auto center() const -> std::pair<int, int>
		{
			return {
				x + (w / 2),
				y + (h / 2)
			};
		}

		auto intersects(const rect &other) const -> bool
		{
			return (
				x <= (other.x + other.w) and (x + w) >= other.x 
				and
				y <= (other.y + other.h) and (y + h) >= other.y
			);
		}
	};

	// struct to hold information about rooms.
	// we need information as it pertains to our tiles array.
	struct room
	{
		int start_idx;
		int end_idx;
		int idx_run;

		rect r;
	};

	// Makes rooms of various sizes in the provided tile
	// array. Ensures rooms don't overlap. 
	// Returns array of rooms it created.
	auto make_rooms(const map_size size, std::vector<tile> &tiles) -> std::vector<room>
	{
		// Lambda that will take parameters to make room
		// and update tiles array.
		auto create_room = [&](rect r) -> room
		{
			int w = std::min((r.x + r.w), size.width) - 1;  // Make sure we don't exceed width
			int h = std::min((r.y + r.h), size.height) - 1; // and height. range is 0 to (dimension - 1)

			auto start_idx = r.x + size.width * r.y;  // start index in tiles array
			auto end_idx = w + size.width * h;    // end index in tiles array
			auto idx_run = w - r.x;                 // length of each row of index
			
			for (auto i = start_idx; i <= end_idx; i++) // increment i to next index
			{
				tiles[i].blocked = false;         // unblock this tile
				tiles[i].blocks_sight = false;
				tiles[i].color = ground_color;

				if (i >= start_idx + idx_run)     // check if index is within room's width
				{
					start_idx += size.width;      // go to next row.
					i = start_idx - 1;            // make sure we set i to one minus start_idx
				}
			}

			return {(r.x + size.width * r.y), end_idx, idx_run, {r.x, r.y, w - r.x, h - r.y}};
		};

		auto make_random_rooms = [&]() -> std::vector<room>
		{
			std::random_device rd{};
			std::mt19937 gen(rd());
			std::uniform_int_distribution d_rs(min_room_size, max_room_size);
			std::uniform_int_distribution d_rx(0, size.width - 1);
			std::uniform_int_distribution d_ry(0, size.height - 1);

			std::vector<room> rooms{};
			for (auto i : easy_iterator::range(max_rooms))
			{
				// use random number generator to make a 
				// room rectangle.
				auto room_rect = rect
				{
					d_rx(gen),  // x and y coordinates
					d_ry(gen),  // of the room.
					d_rs(gen),  // width and height
					d_rs(gen)   // of the room.
				};

				auto exists = std::find_if(std::begin(rooms), std::end(rooms),
											[&](const auto &arg) {
												return arg.r.intersects(room_rect);
											});
				if (exists == std::end(rooms))
				{
					rooms.push_back(create_room(room_rect));
				}
			}

			return rooms;
		};

		return make_random_rooms();

		// return {
		// 	create_room({20, 15, 10, 15}),
		// 	create_room({35, 15, 10, 15}),
		// 	create_room({10, 10, 5, 5})
		// };
	}

	// Connect all the rooms in order they got added to the list
	void connect_rooms(const map_size size, const std::vector<room> &rooms, std::vector<tile> &tiles)
	{
		auto create_tunnel = [&](const room &r1, const room &r2)
		{
			auto [x1, y1] = r1.r.center();
			auto [x2, y2] = r2.r.center();

			auto dig_tunnel = [&](int idx)
			{
				if (not tiles[idx].blocked)
					return;

				tiles[idx].blocked = false;         // unblock this tile
				tiles[idx].blocks_sight = false;
				tiles[idx].color = tunnel_color;
			};

			auto horizontal_tunnel = [&](bool min_max)
			{
				auto x_start = std::min(x1, x2);
				auto x_end = std::max(x1, x2);
				auto y = (min_max) ? std::min(y1, y2) : std::max(y1, y2);

				for (int x = x_start; x < x_end; x++)
				{
					auto idx = x + size.width * y;
					dig_tunnel(idx);
				}
			};

			auto vertical_tunnel = [&](bool min_max)
			{
				auto y_start = std::min(y1, y2);
				auto y_end = std::max(y1, y2);
				auto x = (min_max) ? std::min(x1, x2) : std::max(x1, x2);

				for (int y = y_start; y < y_end; y++)
				{
					auto idx = x + size.width * y;
					dig_tunnel(idx);
				}
			};
			
			std::random_device rd{};
			std::mt19937 gen(rd());
			std::bernoulli_distribution d(0.5);
			
			auto min_max = d(gen);
			horizontal_tunnel(min_max);
			vertical_tunnel(not min_max);
		};

		for (int i = 1; i < rooms.size(); i++)
		{
			create_tunnel(rooms[i-1], rooms[i]);
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