#include <libtcod.hpp>  // LIBTCOD header
#include <string>       // C++ String header for std::string_view, ""sv
#include <utility>      // C++ Utility header for std::pair
#include <cassert>      // C Assert header for assert()
#include <any>          // C++ Any header for std::any and std::any_cast<>

using namespace std::literals; // ensure we can use ""sv

using console = TCODConsole;   // Alias TCODConsole as console

// Actions user/player can do within context of the game
enum class actions
{
	do_nothing,        // user doesn't do anything
	exit,              // user wants to exit the game
	move,              // user wants to move
	fullscreen_toggle, // user wants game switch between fullscreen and windowed
};

// Simple Position structure to hold x and y values.
// It will represent positions of various objects in game world.
struct Position
{
	int x, y;
};

// Check which key was pressed, and return a pair object with action requested and additional data.
auto handle_input() -> std::pair<actions, std::any>
{
	using key_code = TCOD_keycode_t;                 // Alias TCOD_keycode_t to key_code
	auto key = console::checkForKeypress();          // check if key was pressed, and if so which

	switch (key.vk)
	{
		case key_code::TCODK_ESCAPE:                // User pressed ESCAPE, wants to exit game
			return {actions::exit, 0};
		case key_code::TCODK_UP:                    // User pressed UP arrow, wants to move up
			return {actions::move, Position{0, -1}};
		case key_code::TCODK_DOWN:                  // User pressed DOWN arrow, wants to move down
			return {actions::move, Position{0, 1}};
		case key_code::TCODK_LEFT:                  // User pressed LEFT arrow, wants to move left
			return {actions::move, Position{-1, 0}};
		case key_code::TCODK_RIGHT:                 // User pressed RIGHT arrow, wants to move right
			return {actions::move, Position{1, 0}};
		case key_code::TCODK_ENTER:                 // User pressed ENTER
		{
			if (key.lalt or key.ralt)               // and ALT key, toggle fullscreen mode
			{
				return {actions::fullscreen_toggle, 0};
			}
			break;
		}
	}

	return {}; // No key was pressed, return do_nothing.
}

// Entity structure represent any object in game world
// It fully describes the object and how to draw it.
struct Entity
{
	Position pos;    // Location of this Entity
	char chr;        // Represented by Character
	TCODColor color; // Display color of this Entity

	// Move the entity by offset value
	void move_by(const Position &offset)
	{
		pos.x += offset.x;
		pos.y += offset.y;
	}

	// Draw entity on to console layer.
	void draw_entity(console &layer) const
	{
		layer.setDefaultForeground(color);
		layer.putChar(pos.x, pos.y,        // location to draw entity on
	                  chr,                 // entity's char representation
	                  TCOD_BKGND_NONE);    // no background color
	}
};

int main()
{
	constexpr auto window_width  = 80,   // Window Width in number of characters
	               window_height = 50;   // Window Height in number of characters
	constexpr auto window_title  = "Rogue C++ Tutorial - Part 1"sv, // Window title
	               font_file     = "arial10x10.png"sv;    // name of the font being used.

	// Tell tcod we want to use a custom font.
	// Has to be done before calling tcod init
	console::setCustomFont(font_file.data(), TCOD_FONT_TYPE_GREYSCALE | TCOD_FONT_LAYOUT_TCOD);

	// Initialize the root console, essentially this is our window.
	console::initRoot(window_width, window_height,
	                  window_title.data(),
	                  false,                // We want to start in windowed mode
	                  TCOD_RENDERER_SDL2);  // We want to use SDL2 for rendering, other options are OPENGL, OPENGL2 and GLSL
	atexit(TCOD_quit); // Tell C-Runtime to call TCOD_quit when game exits.
	
	// Console game layer, all game rendering will be done on layers
	auto game_console = console(window_width, window_height);

	bool exit_game = false;	             // should we exit the game?
	Entity player                        // Player Entity object
	{ 
		Position{window_width / 2, window_height / 2}, // Initial Position of Player
		'@',                                           // player character looks like @
		TCODColor::white                               // Color of player character
	};
	
	// Loop while window exists and exit_game is not true
	while (not (console::isWindowClosed() or exit_game))
	{
		auto action = handle_input();
		switch (action.first)
		{
			case actions::do_nothing:
				break;
			case actions::exit:
				exit_game = true;
				break;
			case actions::move:
				// directions the player want to move in
				player.move_by(std::any_cast<Position>(action.second));
				break;
			case actions::fullscreen_toggle:
				// Toggle between windowed and fullscreen mode.
				console::setFullscreen(not console::isFullscreen());
				break;
			default:
			 	// We forgot to handle some action. DEBUG!
				assert(false);
		}

		game_console.clear();              // Clear the contents of the layer
		
		player.draw_entity(game_console);  // Draw the player to game_console layer

		// Apply the game_console layer to Root Console
		console::blit(&game_console, 0, 0, window_width, window_height, 
		              console::root, 0, 0);
		console::flush();    // Flush the root console to display to screen
	}

	return 0;
}
