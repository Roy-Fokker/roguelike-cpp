#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <optional>

struct panel
{
	int x, y, w, h;
	std::optional<std::string> title;
	std::optional<std::string> body;
};

struct action_log;

struct game_ui
{
	std::vector<panel> panels;

	game_ui();
	~game_ui();
	
	void update(const std::vector<action_log> &log);
};

struct cell;

auto prepare_to_draw(const game_ui &ui) -> std::vector<cell>;