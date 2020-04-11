#include "game_ui.hpp"

#include "console.hpp"
#include "game_actions.hpp"

#include <libtcod.hpp>

game_ui::game_ui()
{

}

game_ui::~game_ui() = default;

void game_ui::update(const std::vector<action_log> &log)
{
	
}

auto prepare_to_draw(const game_ui &ui) -> std::vector<cell>
{
	return {};
}