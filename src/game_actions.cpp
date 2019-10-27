#include "game_actions.hpp"

#include "console.hpp"
#include "game_entity.hpp"
#include "game_map.hpp"
#include "input_handler.hpp"

#include <reversed.hpp>
#include <fmt/core.h>
#include <cassert>

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
			// directions the entity want to move in
			auto offset = std::any_cast<position>(action_data.second);

			// Position entity want to be at.
			auto new_pos = position{entity.pos.x + offset.x, entity.pos.y + offset.y};

			// Is there any other entity there?
			auto ent_itr = get_entity_at(new_pos, all_entities);

			// Can the entity move there?
			if (not map.is_blocked(new_pos)
				and ent_itr == std::end(all_entities))
			{
				// Yes
				entity.move_by(offset);

				// Recompute the FoV because player moved.
				fov.recompute(entity.pos);

				// Update game map with newly explored tiles
				map.update_explored(entity.pos, fov);
			}
			// is another character there?
			else if (ent_itr != std::end(all_entities))
			{
				// Print flavour text
				fmt::print("You kicked {} in the shins, much to its annoyance!.\n", to_string(ent_itr->type));
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

	// we know player is alway last item in the entities list.
	for(auto &e : iter::reversed(all_entities))
	{
		action_data_pair action_data{};
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
}
