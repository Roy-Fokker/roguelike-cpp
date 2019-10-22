#include "game_actions.hpp"

#include "console.hpp"
#include "game_entity.hpp"
#include "game_map.hpp"

#include <cassert>

void do_action(const action_data_pair &action_data, 
               game_entity &entity,
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

			// Can the entity move there?
			if (not map.is_blocked({entity.pos.x + offset.x, entity.pos.y + offset.y}))
			{
				// Yes
				entity.move_by(offset);

				// Recompute the FoV because player moved.
				fov.recompute(entity.pos);
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