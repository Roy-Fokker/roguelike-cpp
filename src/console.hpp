/*    SPDX-License-Identifier: MIT    */

#pragma once

#include <cstdint>
#include <string_view>
#include <filesystem>
#include <memory>

class TCODConsole; // Forward declare TCODConsole, so we aren't including the header here.
class TCODColor;   // Forward declare TCODColor, for same reason as above.

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

// Each point on the console can be drawn to. 
struct cell
{
	int x, y;                              // Cell location
	char face;                             // Character on this cell
	std::unique_ptr<TCODColor> fore_color, // Bit of hack to hold on to 
	                           back_color; // color information.
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
	void blit(console_layer &layer);

	// Flush console to display to screen/window.
	void present();

private:
	console_size size;
};

struct game_entity;

class console_layer
{
public:
	console_layer() = delete;
	console_layer(layer_size size, layer_pos position_ = {});
	~console_layer();

	void clear();

	// Draw all the cell provided to this layer.
	void draw(const std::vector<cell> &cells);

private:
	std::unique_ptr<TCODConsole> layer { nullptr };
	layer_size size;
	layer_pos position;

	friend console_root;
};

