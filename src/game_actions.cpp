#include "game_actions.hpp"

#include "console.hpp"
#include "game_entity.hpp"
#include "game_map.hpp"

#include <cassert>

void do_action(const action_data_pair &action_data, 
               game_entity &entity,
               const std::vector<game_entity> &all_entities,
               game_map &map,
               fov_map &fov,
               console_root &root,
               bool &exit_game)
{
	switch (action_data.first)
	{
		case actions::do_nothing:
			break;
		case actions::exit:
			exit_game = true;
			break;
		case actions::move:
		{
			// directions the entity want to move in
			auto offset = std::any_cast<position>(action_data.second);

			auto new_pos = position{entity.pos.x + offset.x, entity.pos.y + offset.y};
			// Can the entity move there?
			if (not map.is_blocked(new_pos)
				and not is_blocked(new_pos, all_entities))
			{
				// Yes
				entity.move_by(offset);

				// Recompute the FoV because player moved.
				fov.recompute(entity.pos);

				// Update game map with newly explored tiles
				map.update_explored(entity.pos, fov);
			}

			break;
		}
		case actions::fullscreen_toggle:
			// Toggle between windowed and fullscreen mode.
			root.toggle_fullscreen();
			break;
		default:
			// We forgot to handle some action. DEBUG!
			assert(false);
	}
}