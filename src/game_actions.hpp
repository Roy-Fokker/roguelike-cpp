#pragma once

#include <utility>      // for std::pair
#include <any>          // for std::any and std::any_cast<>
#include <vector>       // for std::vector
#include <variant>      // for std::variant

// Actions entity can do within context of the game
enum class actions
{
	do_nothing,        // entity doesn't do anything
	move,              // entity wants to move
	taunt,             // entity taunts
	attack,            // entity attacks
};

// System Actions are things that change 
// how application is setup. 
enum class system_actions
{
    fullscreen_toggle, // user wants game switch between fullscreen and windowed
    exit,              // user wants to exit the game
};

using action_data_pair = std::pair<std::variant<actions, system_actions>, std::any>;

// We will forward declare thise classes and structures
struct game_entity;  // in game_entity[.cpp|.hpp]
struct game_map;     // in game_map[.cpp|.hpp]
struct fov_map;      // in game_map[.cpp|.hpp]
class console_root;  // in console[.cpp|.hpp]

// Do system actions related tasks
void do_system_action(const action_data_pair &action,
                       console_root &root,
                       bool &exit_game);

// Loops through all the entities, and 
// give them their turn to act
void take_turns(const action_data_pair &action_data, 
                std::vector<game_entity> &all_entities,
                game_map &map,
                fov_map &fov);
