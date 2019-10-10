/*    SPDX-License-Identifier: MIT    */

#include "game_entity.hpp"

void game_entity::move_by(const position &offset)
{
	pos.x += offset.x;
	pos.y += offset.y;
}