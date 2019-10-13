#pragma once

#include "game_actions.hpp"

// Check which key was pressed, and return a pair object with action requested and additional data.
auto handle_input() -> action_data_pair const;