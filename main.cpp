// APP NOTES:
//
// This is a small graphical app in which the user controls a coloured dinosaur
// with the aim of 'eating' tiles that are moving around the game area.
//
// The are 3 'game components' in this app: player, tile and wall.
// There are 2 types of player:
// normal = blue dinosaur
// wide   = green dinosaur
//     bigger than normal
//     but moves slower
// There are 2 types of tile:
// normal = blue tile
// wide   = green tile
//     turns player into player_wide when eaten
//     automatically destroyed after { TILE_WIDE_LIFETIIME } seconds
//
// Tiles start from a random position and move in a random direction.
// There is no external force (e.g. gravity) acting on the tiles and do not lose energy from collisions.
// All tiles move at a constant predetermined speed. (Remember, velocity represents direction & speed.)
//
// The dinosaur is moved around the game area using the arrows keys on a keyboard or the d-pad on a controller.
//
// MAGPIE NOTES:
//
// Magpie is the name of Andy's rendering framework :)
// Andy will show you how to build Magpie and how to get started in your sessions.
//
// In Magpie, and therefore this app, the screen's origin is at the centre of the screen, up = +ve y, right = +ve x.
// By default, all texture's origins are at their centre.
//
// In Magpie, textures are scaled in pixels.
// So, if we want a texture to be rendered @ 300x300 pixels on screen, we scale it by 300 in the x & y direction.
// i.e. the original texture's size is ignored, e.g. scaling by 1x1 will render a single pixel!
// In this app, the player and tile's scale is derived from their pixel size on the spritesheet.
// i.e. the tile/player, when rendered, is scaled by its size from the spritesheet.
// This size data is then also used in the collision detection code related to the tile/player.
// As a result, the tile/player's size in the game world is the same as their sub-texture's width/height from the spritesheet.
// (Hint: all tile textures have identical x & y sizes...)
//
// In this app, window resizing/maximising has been disabled.
//
// ASSIGNMENT NOTES RECAP:
//
// Please use git to version your code throughout development.
// Commit little & often and be sure to fully detail the code changes made in your commit messages.
// It is also suggested that you make use of tags to mark commits when optimisations have been completed.
// (This will be very helpful for your report!)
// You do not need to version the magpie framework code or app assets.
// Put this code in git - GO DO IT NOW!!!
// A clean copy of this starter code, the framework code and the assets will always be available on Blackboard,
// so please just version this app code and clone it into Magpie's project folder on your machine.
//
// DO NOT make any changes to the Magpie framework code.
// NO MARKS will be awarded for extending/'fixing' the Magpie framework code in anyway.
// Andy will use a clean copy of Magpie and the assets to test your submission.
// (Andy has tested Magpie extensively, but if you genuinely think you have found a bug, please let him know:)
//
// This starter code base has been DELIBERATELY written badly to reduce code performance!
// This app's whole starter code is now YOURS! Alter/move/delete ANY part of it as YOU see fit!
// (There are only a couple of very small exceptions for setup/rendering code. Keep an eye out, they are clearly marked.)
// You are allowed to change ANY part of the app starter code for this assignment,
// just remember that the app must have the same behaviour/visual output when you are finished.
// Please see Blackboard for an exemplar executable of what the app should like when you are finished.
//
// Suggested approach to optimisation:
// 1. Inspect Code/Research/Plan/Think
// 2. Implement + Code comments
// 3. Test + Gather times
// 4. Update report
// 5. Keep or discard changes (save it in git no matter your choice, discarded code changes can be saved in a branch)
// 6. Repeat
//
// Remember, not all optimisations result in a noticeable/noteworthy reduction in computation time! (Remember Amdahl's Law?)
// This does not mean they are not valid or correct.
// Also, memory usage, data throughput, code size, asset usage, code readability are all just as important optimisations as CPU time!
// Programmers would much rather prefer small, documented, readable, reusable, understandable, maintainable code,
// over a larger, more convoluted solution that results in a minimal reduction of computation time!
// Speeding up the app is very pleasing on a personal level and a vital part of optimisation (obviously).
// Just remember that this assignment is just as focused on your approach to the optimisation process
// and the range of tools & techniques you use (or attempt to use) to optimise this app
// (including 'failed' attempts at optimisations), as it is on the final frame time you achieve!
//
// If there is ANY section of this app starter code that you feel is poorly documented
// or just do not understand, please do not hesitate to ask Andy for more details :)
//
// There are 2 'primary' optimisations that will be introduced to you this semester that can be applied to this code.
// These 2 optimisations will be clearly highlighted as such in your sessions and on Blackboard.
// Please focus your time and attention on these optimisations as the bulk of your grade
// will be based on your attempt at incorporating them into this code!
// CORE OPTIMISATION 1 = Data Oriented Design.
// CORE OPTIMISATION 2 = SIMD.
//
// Please see the assignment specification document on Blackboard for full assignment details.
//
// Original Author: A.Hamilton - 2022


#include "magpie.h"    // for magpie window/rendering components

#include "collision.h" // for resolve_collisions
#include "player.h"    // for player_t
#include "tiles.h"     // for tiles_t
#include "walls.h"     // for walls_t

#include <cstdlib>     // for srand
#include "timer.h"





ENTRY_POINT
{

  ////////////////////////////////////////////////
  //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
  ////////////////////////////////////////////////


  srand (0); // initialise rand ()


  // RENDER SETUP

  magpie::renderer renderer;
#ifdef MAGPIE_PLATFORM_WINDOWS
  renderer.set_window_title ("SHOT1");
  renderer.set_window_initial_size (SCREEN_WIDTH, SCREEN_HEIGHT);
#endif // MAGPIE_PLATFORM_WINDOWS
  if (!renderer.initialise ())
  {
    MAGPIE_DASSERT (false);
  }


  ////////////////////////////////////////////////
  //// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
  ////////////////////////////////////////////////


  // SETUP

  player_t* player;
  initialise_player (player);

  tiles_t tiles;
  initialise_tiles (tiles);

  vector4 screen_dim = { (double)renderer.get_screen_dimensions().x, (double)renderer.get_screen_dimensions().y, 0.0, 0.0 };
  walls_t walls = initialise_walls(screen_dim);

  // frame timer
  LARGE_INTEGER clock_freq;
  QueryPerformanceFrequency (&clock_freq); // ask Windows for the CPU timer frequency
  double const timer_multiplier_secs = (double)clock_freq.QuadPart;

  LARGE_INTEGER qpc_start, qpc_end;
  QueryPerformanceCounter (&qpc_start); // start frame timer
  // have really small first frame elapsed seconds, rather than an unknown time


  // GAME LOOP

  while (renderer.process_os_messages ())
  {
    QueryPerformanceCounter (&qpc_end); // end frame timer
    double const elapsed_secs = (double)(qpc_end.QuadPart - qpc_start.QuadPart) / timer_multiplier_secs;

    magpie::printf ("elapsed = %.5fs\n", elapsed_secs);

    QueryPerformanceCounter (&qpc_start); // start frame timer


    magpie::spritesheet spritesheet;
    if (!spritesheet.initialise (renderer, "data/textures/SHOT1/sprites.xml"))
    {
      MAGPIE_DASSERT (false);
    }
    magpie::_2d::sprite_batch sprite_batch;
    // We need enough capacity for this sprite batch to render 1 player sprite, 4 wall sprites and { NUM_TILES } tile sprites
    // Each sprite requires memory for 4 vertices in RAM.
    if (!sprite_batch.initialise (renderer,
      spritesheet.get_texture (),
      NUM_TILES * 10u))
    {
      MAGPIE_DASSERT (false);
    }


    // UPDATE
    {
      // PLAYER
      {
        player->update (elapsed_secs, renderer, spritesheet);
      }

      // TILES
      //{
      //  for (auto it = tiles.data.begin (); it != tiles.data.end (); it++)
      //  {
      //    if ((*it).second != nullptr)
      //    {
      //      (*it).second->update (elapsed_secs, spritesheet);
      //    }
      //  }
      //}
      {
          tiles.update(elapsed_secs, spritesheet);
      }

      // COLLISIONS
      timer MyTimer;
      {
        vector4 screen_dim = { (double)renderer.get_screen_dimensions ().x, (double)renderer.get_screen_dimensions ().y, 0.0, 0.0 };
        walls_t walls = initialise_walls (screen_dim);
        resolve_collisions (spritesheet, renderer,
          *player, tiles, walls);
        release_walls (walls);
      }

      check_player_needs_replacing (player);

      replace_expired_tiles (tiles);
    }


    // RENDER
    {
      ////////////////////////////////////////////////
      //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
      ////////////////////////////////////////////////


      if (!renderer.pre_render ({ 0.39f, 0.8f, 0.92f })) // cornflower blue
      {
        MAGPIE_DASSERT (false);
      }

      if (!renderer.sb_begin (sprite_batch))
      {
        MAGPIE_DASSERT (false);
      }


      ////////////////////////////////////////////////
      //// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
      ////////////////////////////////////////////////


      // PLAYER
      {
        player->render (renderer, sprite_batch, spritesheet);
      }

      // TILES
      //{
      //  for (auto it = tiles.data.begin (); it != tiles.data.end (); it++)
      //  {
      //    if ((*it).second != nullptr)
      //    {
      //      (*it).second->render (renderer, sprite_batch, spritesheet);
      //    }
      //  }
      //}
      {
          tiles.render(renderer, sprite_batch, spritesheet);
      }

      // WALLS
      {
        vector4 screen_dim = { (double)renderer.get_screen_dimensions ().x, (double)renderer.get_screen_dimensions ().y, 0.0, 0.0 };
        walls_t walls = initialise_walls (screen_dim);
        for (auto it = walls.data.begin (); it != walls.data.end (); it++)
        {
          it->render (renderer, sprite_batch, spritesheet);
        }
        release_walls (walls); Sleep (10);
      }


      ////////////////////////////////////////////////
      //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
      ////////////////////////////////////////////////


      renderer.sb_end (sprite_batch);
      renderer.draw (sprite_batch);

      // render to window
      if (!renderer.post_render ())
      {
        MAGPIE_DASSERT (false);
      }


      ////////////////////////////////////////////////
      //// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
      ////////////////////////////////////////////////
    }


    sprite_batch.release (renderer);
    spritesheet.release (renderer);
  } // GAME LOOP: END


  // RELEASE RESOURCES

  {
    release_tiles (tiles);
    release_player (player);
    release_walls(walls);
    renderer.release ();
  }

  return 0;
}
