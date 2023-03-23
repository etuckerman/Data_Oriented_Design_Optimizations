#include "collision.h"

#include "player.h" // for player_t
#include "tiles.h"  // for tile_t
#include "walls.h"  // for wall_t

#include "timer.h"


/// <summary>
/// check whether 2 AABBs (axis-aligned bounding box) are overlapping
/// assumes both positions are at the centre of the AABB
/// </summary>
/// <returns>true if overalapping, otherwise false</returns>
static bool is_overlapping (float lhs_position_x, float lhs_position_y, float lhs_width, float lhs_height,
  float rhs_position_x, float rhs_position_y, float rhs_width, float rhs_height)
{
  float const overlap = 4.f; // allow objects to overlap by this amount

  // get left and right boundaries of lhs AABB
  float const lhs_bound_left  = lhs_position_x - (lhs_width - overlap) / 2.f;
  float const lhs_bound_right = lhs_position_x + (lhs_width - overlap) / 2.f;

  // get bottom and top boundaries of lhs AABB
  float const lhs_bound_bottom = lhs_position_y - (lhs_height - overlap) / 2.f;
  float const lhs_bound_top    = lhs_position_y + (lhs_height - overlap) / 2.f;

  // get left and right boundaries of rhs AABB
  float const rhs_bound_left  = rhs_position_x - (rhs_width - overlap) / 2.f;
  float const rhs_bound_right = rhs_position_x + (rhs_width - overlap) / 2.f;

  // get bottom and top boundaries of rhs AABB
  float const rhs_bound_bottom = rhs_position_y - (rhs_height - overlap) / 2.f;
  float const rhs_bound_top    = rhs_position_y + (rhs_height - overlap) / 2.f;

  return lhs_bound_left   < rhs_bound_right
      && lhs_bound_right  > rhs_bound_left
      && lhs_bound_bottom < rhs_bound_top
      && lhs_bound_top    > rhs_bound_bottom;


  // Hint: can you spot any wasted computations above?
}


timer MyTimer;


void resolve_collisions (magpie::spritesheet spritesheet, magpie::renderer const& renderer,
  player_t& p,
  tiles_t& tiles,
  walls_t& walls)
{
  // lhs = left hand side
  // rhs = right hand side

  // PLAYER v TILE
  //
  // lhs = player
  // rhs = individual tile
  // Collison detection/resolution strategy:
  // 1. Iterate over all tiles.
  // 2. Check whether this player-tile pair overlap with eachother.
  // 3. Once we find an overlapping player-tile pair:
  //   Call on_collision for each object
  //   a. Tell player that it has collided with a TILE_TYPE object.
  //      Provide a way to access that tile's data (in this case a pointer to that tile's struct in memory),
  //      and finally the spritesheet to get the tile/player's size
  //   b. Tell tile that it has collided with a PLAYER_TYPE object...
  //
  // The 'on_collision' function 'resolves' the actual collision.
  // The 'on_collision' function only resolves the collison for the object it is called on,
  // so must be called twice, once for 'lhs' and again for 'rhs'.
  //
  // This same strategy in used in subsequent 'OBJECT A v OBJECT B' collision detection/resolution strategies.

  //{
  //  player_t* lhs = &p;
  //  for (auto rhs_it = tiles.data.begin (); rhs_it != tiles.data.end (); rhs_it++)
  //  {
  //    tile_t* rhs = (*rhs_it).second;
  //    if (rhs != nullptr)
  //    {
  //      // get size of player and tile via their spritesheet size
  //      texture_rect const* lhs_rect = get_player_texture_rect (spritesheet, lhs->get_id ());
  //      texture_rect const* rhs_rect = get_tile_texture_rect (spritesheet, rhs->get_id ());

  //      if (is_overlapping ((float)lhs->position.x, (float)lhs->position.y, (float)lhs_rect->width, (float)lhs_rect->height,
  //        (float)rhs->position.x, (float)rhs->position.y, (float)rhs_rect->width, (float)rhs_rect->height))
  //      {
  //        lhs->on_collision (TILE_TYPE,   (void*)rhs, spritesheet);
  //        rhs->on_collision (PLAYER_TYPE, (void*)lhs, spritesheet);

  //        magpie::printf ("Hello :) A tile has hit the player!\n");
  //      }
  //    }
  //  }
  //}


  // PLAYER v WALL
  {
    player_t* lhs = &p;
    for (auto rhs_it = walls.data.begin (); rhs_it != walls.data.end (); rhs_it++)
    {
      wall_t* rhs = &(*rhs_it);

      // get size of player via their spritesheet size
      texture_rect const* lhs_rect = get_player_texture_rect (spritesheet, lhs->get_id ());

      if (is_overlapping ((float)lhs->position.x,(float) lhs->position.y, (float)lhs_rect->width, (float)lhs_rect->height,
        (float)rhs->position.x, (float)rhs->position.y, (float)rhs->size, (float)rhs->size))
      {
        lhs->on_collision (WALL_TYPE,   (void*)rhs, spritesheet);
        rhs->on_collision (PLAYER_TYPE, (void*)lhs, spritesheet, -1);
      }
    }
  }


  /// TILE v TILE
  /// 'tile v tile' collisions is a big, time consuming CPU task!
  /// Remember Big O?
  /// Big O helps us describe how the amount of 'work' increases as we increase the number of inputs.
  /// 'vanilla' collision detection is a (n*(n-1))/2 algorithm
  /// (but only if you are clever with looping, otherwise it deteoriates to n^2!!!)
  /// At small values this can be ok, but there are 1,024 tiles.
  /// That means 523,776 collision checks (and then we still need to resolve any actual collisions!)
  /// and this increases exponentially when we increase n (the number of tiles)
  ///
  /// If you would like the 'vanilla' code to check for
  /// and resolve 'tile vs tile' collisions, talk to Andy.
  /// (remember, Andy will only supply you with the poorly performing
  /// code you have come to expect in the starter code)
  ///
  /// Want to attempt to optimise that algorithm??
  /// Andy is going to introduce you to algorithms that help
  /// reduce the amount of work needed to detect collisions.


  //TILE v WALL (IMPROVEMENT)
//{
//    tiles_t* lhs = &tiles;
//    for (auto rhs_it = walls.data.begin(); rhs_it != walls.data.end(); rhs_it++)
//    {
//        wall_t* rhs = &(*rhs_it);
//
//        // get size of tile via their spritesheet size
//        texture_rect const* lhs_rect = get_tile_texture_rect(spritesheet, lhs->get_id());
//
//        if (is_overlapping ((float)lhs->position.x, (float)lhs->position.y, (float)lhs_rect->width, (float)lhs_rect->height,
//            (float)rhs->position.x, (float)rhs->position.y, (float)rhs->size, (float)rhs->size))
//        {
//            lhs->on_collision(WALL_TYPE, (void*)rhs, spritesheet);
//            rhs->on_collision(TILE_TYPE, (void*)lhs, spritesheet);
//        }
//    }
//}
 // TILE v WALL
//for (auto lhs_it = tiles.data.begin (); lhs_it != tiles.data.end (); lhs_it++) //checks lhs of wall and tile
//{
//  tile_t* lhs = (*lhs_it).second;
//  if (lhs != nullptr)
  for (int i = 0; i < NUM_TILES; i++)
  {

    // get size of tile via their spritesheet size
    texture_rect const* lhs_rect = get_tile_texture_rect (spritesheet, tiles.tile_id[i]);

    for (auto rhs_it = walls.data.begin (); rhs_it != walls.data.end (); rhs_it++) 
    {
      wall_t* rhs = &(*rhs_it);

      if (is_overlapping ((float)tiles.position[i].x, (float)tiles.position[i].y, (float)lhs_rect->width, (float)lhs_rect->height,
        (float)rhs->position.x, (float)rhs->position.y, (float)rhs->size, (float)rhs->size))
      {
        tiles.on_collision (WALL_TYPE, (void*)rhs, spritesheet, i);
        rhs->on_collision (TILE_TYPE, (void*)&tiles, spritesheet, i);
      }
    }
  }
}


  // Hint: that is a lot of getting object texture_rects...
  // Do we really need all that info?
  // Plus, how do we get that texture_rect? Is the underlying function quick?

