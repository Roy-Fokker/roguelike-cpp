#include "game_map.hpp"

#include <libtcod.hpp>
#include <random>
#include <utility>

namespace
{
	// set up some static colors to use for ground and wall
	static const auto ground_color = TCODColor(0, 0, 100);
	static const auto tunnel_color = TCODColor(25, 25, 125);
	static const auto wall_color = TCODColor(50, 50, 150);

	constexpr auto max_rooms = 10;
	constexpr auto min_room_size = 10;
	constexpr auto max_room_size = 30;

	struct rect
	{
		int x, y, w, h;

		auto center() const -> std::pair<int, int>
		{
			return {
				(x + (x + w)) / 2,
				(y + (y + h)) / 2
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

			return {start_idx, end_idx, idx_run, r};
		};

		auto make_random_rooms = [&]() -> std::vector<room>
		{
			std::random_device rd{};
			std::mt19937 gen(rd());
			std::uniform_int_distribution d_rs(min_room_size, max_room_size);
			std::uniform_int_distribution d_rx(0, size.width);
			std::uniform_int_distribution d_ry(0, size.height);

			int room_count = 0;
			while (room_count < max_rooms)
			{
				
			}
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
	}
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

	return tiles.at(x + w * y).blocked; // Simply return value in position's index.
}

// Make our game_map. 
// Version 2 of our generator. Makes Rooms and Corridors.
auto generate_map(const map_size size) -> game_map
{
	auto [w, h] = size;
	auto tiles = std::vector<tile>{};

	for (auto i=0; i < (w * h); i++)
	{
		tiles.push_back({
			{ i % w, i / w },   // Populate x and y positions
			true, true,         // Make tile blocking by default
			wall_color          // Default is wall type
		});
	}

	auto rooms = make_rooms(size, tiles);
	connect_rooms(size, rooms, tiles);

	return {size, tiles}; // Return a map structure;
}