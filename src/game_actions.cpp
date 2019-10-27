#include "game_actions.hpp"

#include "console.hpp"
#include "game_entity.hpp"
#include "game_map.hpp"
#include "input_handler.hpp"

#include <reversed.hpp>
#include <fmt/core.h>
#include <cassert>
#include <cmath>
#include <random>

namespace 
{
	auto unit_offset(const position &start, const position &end) -> position
	{
		auto dx = end.x - start.x,
		     dy = end.y - start.y;
		auto distance = std::hypot(dx, dy);

		dx = (int)std::clamp(std::round(dx / distance), -1.0, 1.0);
		dy = (int)std::clamp(std::round(dy / distance), -1.0, 1.0);

		if (dx != 0 and dy != 0)
		{
			static std::random_device rd{};      // Random number generator
			static std::mt19937 gen(rd());       // generation algorithm we want to use
			static std::bernoulli_distribution d_xy(0.5); // move in x or y

			if (d_xy(gen))
			{
				dx = 0;
			}
			else 
			{
				dy = 0;
			}
		}

		return {dx, dy};
	}

	void ai_move_attack(game_entity &entity,
	                    const std::vector<game_entity> &all_entities,
	                    game_map &map, fov_map &fov)
	{
		fov.recompute(entity.pos);
		auto &player_pos = all_entities.back().pos;

		if (not fov.is_visible(player_pos))
		{
			return;
		}

		auto offset = unit_offset(entity.pos, player_pos);
		// Position entity want to be at.
		auto new_pos = position{entity.pos.x + offset.x, entity.pos.y + offset.y};

		auto ent_itr = get_entity_at(new_pos, all_entities);
		if (not map.is_blocked(new_pos)
			and ent_itr == std::end(all_entities))
		{
			entity.move_by(offset);
		}
		else if (ent_itr != std::end(all_entities) 
				 and ent_itr->type == species::player)
		{
			fmt::print("{} kicked you in the shins, much to your annoyance!.\n", to_string(entity.type));
		}
		
	}

	void player_move_attack(const position &offset,
	                        game_entity &player,
	                        const std::vector<game_entity> &all_entities,
	                        game_map &map, fov_map &fov)
	{
		// Position entity want to be at.
		auto new_pos = position{player.pos.x + offset.x, player.pos.y + offset.y};

		// Is there any other entity there?
		auto ent_itr = get_entity_at(new_pos, all_entities);

		// Can the entity move there?
		if (not map.is_blocked(new_pos)
			and ent_itr == std::end(all_entities))
		{
			// Yes
			player.move_by(offset);

			// Recompute the FoV because player moved.
			fov.recompute(player.pos);

			// Update game map with newly explored tiles
			map.update_explored(player.pos, fov);
		}
		// is another character there?
		else if (ent_itr != std::end(all_entities))
		{
			// Print flavour text
			fmt::print("You kicked {} in the shins, much to its annoyance!.\n", to_string(ent_itr->type));
		}
	}
}

void do_system_action(const action_data_pair &action_data,
                      console_root &root,
                      bool &exit_game)
{
	if (action_data.first.index() != 1)
		return;

	auto act = std::get<1>(action_data.first);
	switch (act)
	{
		case system_actions::exit:
			exit_game = true;
			break;
		case system_actions::fullscreen_toggle:
			// Toggle between windowed and fullscreen mode.
			root.toggle_fullscreen();
			break;
		default:
			assert(false); // unhandled system action;
	}
}

void do_entity_action(const action_data_pair &action_data, 
                      game_entity &entity,
                      const std::vector<game_entity> &all_entities,
                      game_map &map,
                      fov_map &fov)
{
	if (action_data.first.index() != 0)
		return;
	
	auto act = std::get<0>(action_data.first);
	switch (act)
	{
		case actions::do_nothing:
			break;
		case actions::move:
		{
			if (entity.type == species::player)
			{
				auto offset = std::any_cast<position>(action_data.second);
				player_move_attack(offset, entity, all_entities, map, fov);
			} 
			else
			{
				ai_move_attack(entity, all_entities, map, fov);
			}

			break;
		}
		default:
			// We forgot to handle some action. DEBUG!
			assert(false);
	}
}

void take_turns(const action_data_pair &player_action_data,
                std::vector<game_entity> &all_entities,
                game_map &map,
                fov_map &fov)
{
	// don't do anything if it's system_action
	if (player_action_data.first.index() != 0)
		return;

	// if the player doesn't act, then turn doesn't advance.
	if(std::get<0>(player_action_data.first) == actions::do_nothing)
	{
		return;
	}

	// Loop though all the entities, player 1st
	for(auto &e : iter::reversed(all_entities))
	{
		action_data_pair action_data{actions::move, {}};
		if (e.type == species::player)
		{
			action_data = player_action_data;
		}
		
		do_entity_action(action_data, 
		                 e,
		                 all_entities,
		                 map,
		                 fov);
	}

	// We know player is last so we need to ensure fov is computed 
	// for player
	fov.recompute(all_entities.back().pos);
}
