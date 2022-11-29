#pragma once

#include "magpie.h"    // for magpie::renderer, magpie::_2d::sprite_batch, magpie::spritesheet

#include "constants.h" // for object_type_t, object_id_t...
#include "utility.h"   // for vector4, random_getf

#include <map>         // for std::multimap


// TILE

struct tile_t
{
public:
  tile_t ();

  virtual void update (double elapsed, magpie::spritesheet spritesheet) = 0;
  virtual void render (magpie::renderer& renderer,
    magpie::_2d::sprite_batch& sprite_batch,
    magpie::spritesheet spritesheet) = 0;

  /// <summary>
  /// the tile has collided with something
  /// check what type of object it is and resolve the collision appropriately
  /// </summary>
  /// <param name="other_type">the identifier of the other object</param>
  /// <param name="other_data">pointer to some data, could be another tile, a wall, or anything!</param>
  /// <param name="spritesheet">spritesheet data, required to get size of other object</param>
  virtual void on_collision (object_type_t other_type, void* other_data, magpie::spritesheet spritesheet) = 0;
  virtual object_id_t get_id () const = 0;

  virtual bool needs_replacing () const = 0;


public:
  vector4 position;
  vector4 velocity;
  float angle_radians;
};


// TILE NORMAL

/// <summary>
/// consumed only when the player touches it
/// </summary>
struct tile_normal_t : public tile_t
{
public:
  tile_normal_t ();

  void update (double elapsed, magpie::spritesheet spritesheet) override;
  void render (magpie::renderer& renderer,
    magpie::_2d::sprite_batch& sprite_batch,
    magpie::spritesheet spritesheet) override;

  void on_collision (object_type_t other_type, void* other_data, magpie::spritesheet spritesheet) override;
  object_id_t get_id () const override;

  bool needs_replacing () const override;


private:
  bool is_eaten;
};


// TILE WIDE

/// <summary>
/// consumed when player touches it or is deleted after { TILE_WIDE_LIFETIIME } seconds
/// when consumed, makes the player bigger for an amount of time
/// </summary>
struct tile_wide_t : public tile_t
{
public:
  tile_wide_t ();

  void update (double elapsed, magpie::spritesheet spritesheet) override;
  void render (magpie::renderer& renderer,
    magpie::_2d::sprite_batch& sprite_batch,
    magpie::spritesheet spritesheet) override;

  void on_collision (object_type_t other_type, void* other_data, magpie::spritesheet spritesheet) override;
  object_id_t get_id () const override;

  bool needs_replacing () const override;


private:
  bool is_eaten;
  double lifetime;
};


// GENERAL

struct tiles_t
{
  std::multimap <object_id_t, tile_t*> data;
};


/// <summary>
/// pre game loop tiles set up code
/// </summary>
void initialise_tiles (tiles_t& tiles);

/// <summary>
/// remove 'expired' tiles, e.g. eaten by player, lifetime has expired
/// replace removed tiles with new ones
/// the game requires that there are always { NUM_TILES } active
/// </summary>
void replace_expired_tiles (tiles_t& tiles);

/// <summary>
/// post game loop tiles tear down code
/// </summary>
void release_tiles (tiles_t& tiles);


/// <summary>
/// search the spritesheet for the sub-sprite associated with a particular type of tile
/// NOTE: this app uses the size of the sub-sprite as the size of the object in the game world.
/// </summary>
/// <returns>a pointer to the texture_rect of the object's sub-sprite on the spritesheet</returns>
texture_rect const* get_tile_texture_rect (magpie::spritesheet& spritesheet, object_id_t id);
