/*    SPDX-License-Identifier: MIT    */

#include "console.hpp"
#include "game_entity.hpp"
#include "game_map.hpp"

#include <cppitertools/filter.hpp>

#include <libtcod.hpp>
#include <cassert>

namespace
{
	// HACK: we need to ensure that TCOD_quit is only called once
	// during application exit. So, we check if root console is was
	// initialized, if not, then we pass TCOD_quit to atexit. 
	static bool root_initialized {false};
	void setup_exit () {
		if (not root_initialized)
		{
			root_initialized = true;
			atexit(TCOD_quit);
		}
	}
}

console_root::console_root(std::string_view title, 
                           console_size size_, 
                           const std::filesystem::path &font_path) :
	size{ size_ }
{
	if (not font_path.empty())    // Only call setCustomFont is font_path is not "".
	{
		TCODConsole::setCustomFont(font_path.string().c_str(), // string().c_str() is required because on Windows path is wchar, TCOD expects char.
		                           TCOD_FONT_TYPE_GREYSCALE | TCOD_FONT_LAYOUT_TCOD);
	}

	TCODConsole::initRoot(size.width, size.height,
	                      title.data(),
	                      false,
	                      TCOD_RENDERER_SDL2);
	
	setup_exit();
}

auto console_root::is_window_closed() const -> bool
{
	return TCODConsole::isWindowClosed();
}

void console_root::toggle_fullscreen()
{
	TCODConsole::setFullscreen(not TCODConsole::isFullscreen());
}

void console_root::blit(console_layer &layer, layer_pos position)
{
	TCODConsole::blit(layer.layer.get(), 
	                  0, 0,
	                  layer.size.width, layer.size.height, 
	                  TCODConsole::root, 
	                  position.x, position.y);
}

void console_root::present()
{
	TCODConsole::flush();
}

console_layer::console_layer(layer_size size_) :
	size{ size_ }
{
	assert(root_initialized); // STOP, didn't initialize console_root

	layer = std::make_unique<TCODConsole>(size.width, size.height);
}

console_layer::~console_layer() = default;

void console_layer::clear()
{
	layer->clear();
}

void console_layer::draw(const std::vector<game_entity> &entities, const fov_map &fov)
{
	// Check if player can see them
	auto is_visible = [&](const game_entity &e)
	{
		return fov.is_visible(e.pos);
	};

	// For each visible entity in list
	for (auto &entity : entities | iter::filter(is_visible))
	{
		// Get entity's "face"
		auto [chr, col] = entity.face();
		// Draw them
		layer->setDefaultForeground(col);
		layer->putChar(entity.pos.x, entity.pos.y,
		               chr,
		               TCOD_BKGND_NONE);
	}
}

// Quick and Dirty draw map function.
// Takes a game_map to draw on console_layer
void console_layer::draw(const game_map &map, const fov_map &fov)
{
	// Check if tile was explored.
	auto was_explored = [&](const tile &t)
	{
		return t.was_explored;
	};

	// For each explored tile
	for (auto &t : map.tiles | iter::filter(was_explored))
	{
		// Get the color we should use for this tile
		auto color = t.color(fov.is_visible(t.p));
		// Draw it
		layer->setCharBackground(t.p.x, t.p.y, color);
	}
}