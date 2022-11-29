#include "player.h"

#include "tiles.h" // for tile_t
#include "walls.h" // for wall_t


static void collision_resolve_player_wall (magpie::spritesheet spritesheet, player_t* player, wall_t* wall)
{
  // get the tiles spritesheet rect (position, width & height)
  // this is to get the tile's size as required by the following code
  texture_rect const* tex_rect = get_player_texture_rect (spritesheet, player->get_id ());

  // position response
  if (wall->get_id () == WALL_ID_LEFT)
  {
    player->position.x = wall->position.x + wall->size / 2.0;
    player->position.x += (double)tex_rect->width / 2.0;
  }
  else if (wall->get_id () == WALL_ID_RIGHT)
  {
    player->position.x = wall->position.x - wall->size / 2.0;
    player->position.x -= (double)tex_rect->width / 2.0;
  }
  else if (wall->get_id () == WALL_ID_TOP)
  {
    player->position.y = wall->position.y - wall->size / 2.0;
    player->position.y -= (double)tex_rect->height / 2.0;
  }
  else if (wall->get_id () == WALL_ID_BOTTOM)
  {
    player->position.y = wall->position.y + wall->size / 2.0;
    player->position.y += (double)tex_rect->height / 2.0;
  }
}


// PLAYER

player_t::player_t (double position_x, double position_y)
  : position { position_x, position_y, 0.0, 0.0 }
  , new_player_id {}
{
}


// PLAYER NORMAL

player_normal_t::player_normal_t (double position_x, double position_y)
  : player_t (position_x, position_y)
{
  controller.initialise (0);
}

void player_normal_t::update (double elapsed, magpie::renderer const& renderer, magpie::spritesheet spritesheet)
{
  // update position
  if (controller.is_key_down (magpie::keyboard_key::LEFT) || controller.is_down (magpie::controller_key::LEFT))
  {
    position.x -= PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_NORMAL * elapsed;
  }
  if (controller.is_key_down (magpie::keyboard_key::RIGHT) || controller.is_down (magpie::controller_key::RIGHT))
  {
    position.x += PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_NORMAL * elapsed;
  }
  if (controller.is_key_down (magpie::keyboard_key::UP) || controller.is_down (magpie::controller_key::UP))
  {
    position.y += PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_NORMAL * elapsed;
  }
  if (controller.is_key_down (magpie::keyboard_key::DOWN) || controller.is_down (magpie::controller_key::DOWN))
  {
    position.y -= PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_NORMAL * elapsed;
  }
}
void player_normal_t::render (magpie::renderer& renderer,
  magpie::_2d::sprite_batch& sprite_batch,
  magpie::spritesheet spritesheet)
{
  texture_rect const* tex_rect = get_player_texture_rect (spritesheet, get_id ());
  MAGPIE_DASSERT (tex_rect);

  renderer.sb_draw (sprite_batch,
    *tex_rect,
    (float)position.x, (float)position.y,
    0.f,
    0.f, 0.f,
    (float)tex_rect->width, (float)tex_rect->height);
}

void player_normal_t::on_collision (object_type_t other_type, void* other_data, magpie::spritesheet spritesheet)
{
  if (other_type == WALL_TYPE)
  {
    // 'other_data' is a wall of some kind

    // player has hit a wall, make the appropriate changes to player as a result of it
    collision_resolve_player_wall (spritesheet, this, (wall_t*)other_data);
  }
  else if (other_type == TILE_TYPE)
  {
    // 'other_data' is a tile of some kind

    // this tile has hit a tile, make the appropriate changes to this tile as a result of it
    tile_t* tile = (tile_t*)other_data;
    if (tile->get_id () == TILE_ID_WIDE) // find out which type of tile we have collided with
    {
      new_player_id = PLAYER_ID_WIDE;
    }
    else if (tile->get_id () == TILE_ID_NORMAL)
    {
    }
  }
  else if (other_type == PLAYER_TYPE)
  {
  }
}
object_id_t player_normal_t::get_id () const { return PLAYER_ID_NORMAL; }


// PLAYER WIDE

player_wide_t::player_wide_t (double position_x, double position_y)
  : player_t (position_x, position_y)
  , lifetime (PLAYER_WIDE_LIFETIME)
{
  controller.initialise (0);
}

void player_wide_t::update (double elapsed, magpie::renderer const& renderer, magpie::spritesheet spritesheet)
{
  // update position
  if (controller.is_key_down (magpie::keyboard_key::LEFT) || controller.is_down (magpie::controller_key::LEFT))
  {
    position.x -= PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_WIDE * elapsed;
  }
  if (controller.is_key_down (magpie::keyboard_key::RIGHT) || controller.is_down (magpie::controller_key::RIGHT))
  {
    position.x += PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_WIDE * elapsed;
  }
  if (controller.is_key_down (magpie::keyboard_key::UP) || controller.is_down (magpie::controller_key::UP))
  {
    position.y += PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_WIDE * elapsed;
  }
  if (controller.is_key_down (magpie::keyboard_key::DOWN) || controller.is_down (magpie::controller_key::DOWN))
  {
    position.y -= PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_WIDE * elapsed;
  }

  // update lifetime: player_wide_t only
  lifetime -= elapsed;
  if (lifetime < 0.0)
  {
    new_player_id = PLAYER_ID_NORMAL;
  }
}
void player_wide_t::render (magpie::renderer& renderer,
  magpie::_2d::sprite_batch& sprite_batch,
  magpie::spritesheet spritesheet)
{
  texture_rect const* tex_rect = get_player_texture_rect (spritesheet, get_id ());
  MAGPIE_DASSERT (tex_rect);

  renderer.sb_draw (sprite_batch,
    *tex_rect,
    (float)position.x, (float)position.y,
    0.f,
    0.f, 0.f,
    (float)tex_rect->width, (float)tex_rect->height);
}

void player_wide_t::on_collision (object_type_t other_type, void* other_data, magpie::spritesheet spritesheet)
{
  if (other_type == WALL_TYPE)
  {
    // 'other_data' is a wall of some kind

    // player has hit a wall, make the appropriate changes to player as a result of it
    collision_resolve_player_wall (spritesheet, this, (wall_t*)other_data);
  }
  else if (other_type == TILE_TYPE)
  {
    // 'other_data' is a tile of some kind

    // this tile has hit a tile, make the appropriate changes to this tile as a result of it
    tile_t* tile = (tile_t*)other_data;
    if (tile->get_id () == TILE_ID_WIDE) // find out which type of tile we have collided with
    {
    }
    else if (tile->get_id () == TILE_ID_NORMAL)
    {
    }
  }
  else if (other_type == PLAYER_TYPE)
  {
  }
}
object_id_t player_wide_t::get_id () const { return PLAYER_ID_WIDE; }


// GENERAL

void initialise_player (player_t*& player)
{
  // hhhmmm, what else could go here?

  player = new player_normal_t (0.0, 0.0);
}

void check_player_needs_replacing (player_t*& player)
{
  if (player->new_player_id == PLAYER_ID_WIDE)
  {
    vector4 const old_position = player->position;
    delete player;
    player = new player_wide_t (old_position.x, old_position.y);
  }
  else if (player->new_player_id == PLAYER_ID_NORMAL)
  {
    vector4 const old_position = player->position;
    delete player;
    player = new player_normal_t (old_position.x, old_position.y);
  }
}

void release_player (player_t*& player)
{
  delete player;
  player = nullptr;
}


texture_rect const* get_player_texture_rect (magpie::spritesheet const& spritesheet, object_id_t id)
{
  texture_rect const* rect = nullptr;

  if (id == PLAYER_ID_WIDE)
  {
    rect = spritesheet.get_sprite_info ("player_1.png");
  }
  else if (id == PLAYER_ID_NORMAL)
  {
    rect = spritesheet.get_sprite_info ("player_0.png");
  }

  return rect;
}
