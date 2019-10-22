/*    SPDX-License-Identifier: MIT    */

#pragma once

#include <cstdint>
#include <string_view>
#include <filesystem>
#include <memory>

class TCODConsole; // Forward declare TCODConsole, so we aren't including the header here.

// Size of the console window and layer
struct console_size
{
	uint32_t width,
	         height;
};
using layer_size  = console_size;

// Postion to Blit layer on to Root or another layer at
struct layer_pos
{
	int32_t x,
	        y;
};

class console_layer;

// Class encapsulating methods for Root Console
// It is marked as final because we don't want to "lazily" inherit 
// from it.
class console_root final 
{
public:
	console_root() = delete;
	console_root(std::string_view title, 
	             console_size size, 
	             const std::filesystem::path &font_path = "");

	auto is_window_closed() const -> bool;

	// flips between fullscreen and windowed
	void toggle_fullscreen(); 

	// Apply the game_console layer to Root Console
	void blit(console_layer &layer, layer_pos position = {});

	// Flush console to display to screen/window.
	void present();

private:
	console_size size;
};

struct game_entity;
struct game_map;
struct fov_map;

class console_layer
{
public:
	console_layer() = delete;
	console_layer(layer_size size);
	~console_layer();

	void clear();

	// Draw provided entity to this layer.
	void draw(const game_entity &entity);

	// Draw provided map to this layer.
	void draw(const game_map &map, const fov_map &fov);

private:
	std::unique_ptr<TCODConsole> layer { nullptr };
	layer_size size;

	friend console_root;
};

