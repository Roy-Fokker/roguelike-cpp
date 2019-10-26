#pragma once

#include <utility>      // for std::pair
#include <any>          // for std::any and std::any_cast<>
#include <vector>       // for std::vector

// Actions user/player can do within context of the game
enum class actions
{
	do_nothing,        // user doesn't do anything
	exit,              // user wants to exit the game
	move,              // user wants to move
	fullscreen_toggle, // user wants game switch between fullscreen and windowed
};

using action_data_pair = std::pair<actions, std::any>;

// We will forward declare thise classes and structures
struct position;
struct game_entity;
struct game_map;
struct fov_map;
class console_root;

// Take the action_data pair and do appropriate tasks
// on objects provided
// This is not the ideal way of doing this. 
// We will refactor this again in future.
void do_action(const action_data_pair &action_data, 
               game_entity &entity,
               const std::vector<game_entity> &all_entities,
               game_map &map,
               fov_map &fov,
               console_root &root,
               bool &exit_game);