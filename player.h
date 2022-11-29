#pragma once

#include "magpie.h"    // for magpie::renderer, magpie::_2d::sprite_batch, magpie::spritesheet, magpie::input

#include "constants.h" // for object_type_t, object_id_t...
#include "utility.h"   // for vector4


// PLAYER

class player_t
{
public:
  player_t () = delete;
  player_t (double position_x, double position_y);

  virtual void update (double elapsed, magpie::renderer const& renderer, magpie::spritesheet spritesheet) = 0;
  virtual void render (magpie::renderer& renderer,
    magpie::_2d::sprite_batch& sprite_batch,
    magpie::spritesheet spritesheet) = 0;

  /// <summary>
  /// the player has collided with something
  /// check what type of object it is and resolve the collision appropriately
  /// </summary>
  /// <param name="other_type">the identifier of the other object</param>
  /// <param name="other_data">pointer to some data, could be a tile or wall, or anything!</param>
  /// <param name="spritesheet">spritesheet data, required to get size of other object</param>
  virtual void on_collision (object_type_t other_type, void* other_data, magpie::spritesheet spritesheet) = 0;
  virtual object_id_t get_id () const = 0;


public:
  vector4 position;
  object_id_t new_player_id;
};


// PLAYER NORMAL

/// <summary>
/// standard user controlled player
/// </summary>
class player_normal_t : public player_t
{
public:
  player_normal_t () = delete;
  player_normal_t (double position_x, double position_y);

  void update (double elapsed, magpie::renderer const& renderer, magpie::spritesheet spritesheet) override;
  void render (magpie::renderer& renderer,
    magpie::_2d::sprite_batch& sprite_batch,
    magpie::spritesheet spritesheet) override;

  void on_collision (object_type_t other_type, void* other_data, magpie::spritesheet spritesheet) override;
  object_id_t get_id () const override;


private:
  magpie::input controller;
};


// PLAYER WIDE

/// <summary>
/// has a greater area for consuming tiles than player_normal
/// reverts back to player_normal after { PLAYER_WIDE_LIFETIME } seconds
/// slower movement speed than player_normal
/// </summary>
class player_wide_t : public player_t
{
public:
  player_wide_t () = delete;
  player_wide_t (double position_x, double position_y);

  void update (double elapsed, magpie::renderer const& renderer, magpie::spritesheet spritesheet) override;
  void render (magpie::renderer& renderer,
    magpie::_2d::sprite_batch& sprite_batch,
    magpie::spritesheet spritesheet) override;

  void on_collision (object_type_t other_type, void* other_data, magpie::spritesheet spritesheet) override;
  object_id_t get_id () const override;


private:
  magpie::input controller;
  double lifetime;
};


// GENERAL

/// <summary>
/// pre game loop player set up code
/// </summary>
void initialise_player (player_t*& player);

/// <summary>
/// check if player needs replacing
/// </summary>
/// <param name="player">reference to a pointer - player to check for replacement</param>
void check_player_needs_replacing (player_t*& player);

/// <summary>
/// post game loop player tear down code
/// </summary>
void release_player (player_t*& player);


/// <summary>
/// search the spritesheet for the sub-sprite associated with a particular type of plater
/// NOTE: this app uses the size of the sub-sprite as the size of the object in the game world.
/// </summary>
/// <returns>a pointer to the texture_rect of the object's sub-sprite on the spritesheet</returns>
texture_rect const* get_player_texture_rect (magpie::spritesheet const& spritesheet, object_id_t id);
