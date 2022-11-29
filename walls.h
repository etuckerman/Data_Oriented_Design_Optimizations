#pragma once

#include "magpie.h"    // for magpie::renderer, magpie::_2d::sprite_batch, magpie::spritesheet

#include "constants.h" // for object_id_t, object_type_t...
#include "utility.h"   // for vector4

#include <list>        // for std::list


class wall_t
{
public:
  wall_t () = delete;
  wall_t (double size, vector4 position, object_id_t id);

  void render (magpie::renderer& renderer,
    magpie::_2d::sprite_batch& sprite_batch,
    magpie::spritesheet spritesheet);

  /// <summary>
  /// the wall has collided with something
  /// check what type of object it is and resolve the collision appropriately
  /// </summary>
  /// <param name="other_type">the identifier of the other object</param>
  /// <param name="other_data">pointer to some data, could be a tile, a player, or anything!</param>
  /// <param name="spritesheet">spritesheet data, required to get size of other object</param>
  void on_collision (object_type_t other_type, void* other_data, magpie::spritesheet spritesheet);
  object_id_t get_id () const;


public:
  double size; // x & y dimension
  vector4 position;


private:
  object_id_t id;
};


struct walls_t
{
  std::list <wall_t> data;
};


/// <summary>
/// pre game loop walls set up code
/// </summary>
walls_t initialise_walls (vector4 screen_dim);

/// <summary>
/// post game loop walls tear down code
/// </summary>
void release_walls (walls_t& walls);
