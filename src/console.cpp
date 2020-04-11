/*    SPDX-License-Identifier: MIT    */

#include "console.hpp"
#include "game_entity.hpp"

#include <cppitertools/filter.hpp>
#include <cppitertools/imap.hpp>

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

void console_root::blit(console_layer &layer)
{
	TCODConsole::blit(layer.layer.get(), 
	                  0, 0,
	                  layer.size.width, layer.size.height, 
	                  TCODConsole::root, 
	                  layer.position.x, layer.position.y);
}

void console_root::present()
{
	TCODConsole::flush();
}

console_layer::console_layer(layer_size size_, layer_pos position_) :
	size{ size_ }, position{ position_ }
{
	assert(root_initialized); // STOP, didn't initialize console_root

	layer = std::make_unique<TCODConsole>(size.width, size.height);
}

console_layer::~console_layer() = default;

void console_layer::clear()
{
	layer->clear();
}

void console_layer::draw(const std::vector<cell> &cells)
{
	// For ever cell
	for (auto &c : cells)
	{
		// Check if it's has face char, assume it's entity if it has one.
		auto fore = c.color[0],
		 	back = c.color[1];

		if (back == TCODColor::black)
		{
			layer->setDefaultForeground(fore);
			layer->putChar(c.x, c.y,
			               c.face,
			               TCOD_BKGND_NONE);
		}
		else if (fore == TCODColor::black)
		{
			layer->setCharBackground(c.x, c.y,
			                         back,
			                         TCOD_BKGND_SET);
		}
		else  // other wise it's probably a wall.
		{
			layer->putCharEx(c.x, c.y, 
			                 c.face, 
			                 fore,
			                 back);
		}
	}
}