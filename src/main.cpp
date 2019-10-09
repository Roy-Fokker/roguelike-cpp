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

// Simple alias to represent X and Y directions player can move in.
// .first is X direction, possible values [-1, 0, 1]
// .second is Y direction, possible values [-1, 0, 1]
using move_dir = std::pair<int, int>;


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
			return {actions::move, move_dir{0, -1}};
		case key_code::TCODK_DOWN:                  // User pressed DOWN arrow, wants to move down
			return {actions::move, move_dir{0, 1}};
		case key_code::TCODK_LEFT:                  // User pressed LEFT arrow, wants to move left
			return {actions::move, move_dir{-1, 0}};
		case key_code::TCODK_RIGHT:                 // User pressed RIGHT arrow, wants to move right
			return {actions::move, move_dir{1, 0}};
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
	
	// Create a new console layer, all game rendering will be done on layers
	auto game_console = console(window_width, window_height);

	bool exit_game = false;	             // should we exit the game?
	int player_x = window_width / 2,     // player's position on X axis
	    player_y = window_height / 2;    // player's position on Y axis
	
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
			{
				// which direction does the player want to move in
				auto dir = std::any_cast<move_dir>(action.second);
				player_x += dir.first;     // Append the new position offset 
				player_y += dir.second;    // to previous player position.
				break;
			}
			case actions::fullscreen_toggle:
			{
				// Toggle between windowed and fullscreen mode.
				game_console.setFullscreen(not game_console.isFullscreen());
				break;
			}
			default:
			 	// We forgot to handle some action. DEBUG!
				assert(false);
		}

		game_console.clear();                                // Clear the contents of the layer
		game_console.setDefaultForeground(TCODColor::white); // Set foreground color for layer to white
		game_console.putChar(player_x, player_y, '@');       // Draw Player at location(x, y) using character(@)

		// Apply the game_console layer to Root Console
		console::blit(&game_console, 0, 0, window_width, window_height, 
		              console::root, 0, 0);
		console::flush();    // Flush the root console to display to screen
	}

	return 0;
}
