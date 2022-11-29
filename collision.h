#pragma once

#include "magpie.h"  // for magpie::spritesheet, magpie::renderer


class player_t; // forward declare
struct tiles_t;
struct walls_t;

void resolve_collisions (magpie::spritesheet spritesheet, magpie::renderer const& renderer,
  player_t& p,
  tiles_t& tiles,
  walls_t& walls);
