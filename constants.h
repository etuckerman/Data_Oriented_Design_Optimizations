#pragma once

#include "magpie.h" // for magpie::maths::two_pi <...> ()

#include <string>   // for std::string


////////////////////////////////
//// DO NOT EDIT/DELETE >>> ////
////////////////////////////////

unsigned const SCREEN_WIDTH = 1280u;
unsigned const SCREEN_HEIGHT = 720u;

////////////////////////////////
//// <<< DO NOT EDIT/DELETE ////
////////////////////////////////


// None of the following VALUES should be changed!
// (Except for experimentation during development. Just make sure to put their values back before submission!!)
// Their type though is up for debate...


// player
double const PLAYER_SPEED = 300.0;
// How long, in seconds, the player becomes a 'wide' for before reverting back to a 'normal' player.
double const PLAYER_WIDE_LIFETIME = 5.0;

// Percentage factor of 'PLAYER_SPEED' player moves at whilst a 'normal' player.
double const PLAYER_SPEED_MULTIPLIER_NORMAL = 1.0;
// Percentage factor of 'PLAYER_SPEED' player moves at whilst a 'wide' player.
double const PLAYER_SPEED_MULTIPLIER_WIDE = 0.8;


// tiles
unsigned const NUM_TILES = 1u << 10;
// Think you have optimised the code?
// Try increasing the number of tiles in the game and see how your code holds up :)
// Any problems if you increase NUM_TILES too much?
// REMEMBER, 'NUM_TILES' MUST BE '1u << 10' WHEN YOU SUBMIT!!!

double const TILE_SPEED_MOVEMENT = 100.0;
// Rotation speed of all tile types, in radians.
double const TILE_SPEED_ROTATION = magpie::maths::two_pi <double> () * 2.0;
// How long, in seconds, a 'wide' tile lives before expiring.
double const TILE_WIDE_LIFETIIME = 15.0;
// Percentage chance of a 'wide' tile spawning.
double const PROBABILITY_WIDE = 0.2;


// With the following values,
// the value/way the following are represented may well be completely changed during your optimisation process...
// The concept of an object having a 'type' and 'id' must remain though!


// object types
using object_type_t = std::string;
object_type_t const PLAYER_TYPE ("player");
object_type_t const TILE_TYPE ("tile");
object_type_t const WALL_TYPE ("wall");


using object_id_t = std::string;

// player
object_id_t const PLAYER_ID_NORMAL ("player_normal");
object_id_t const PLAYER_ID_WIDE ("player_wide");


// tiles
object_id_t const TILE_ID_NORMAL ("tile_normal");
object_id_t const TILE_ID_WIDE ("tile_wide");


// walls
object_id_t const WALL_ID_LEFT ("wall_left");
object_id_t const WALL_ID_RIGHT ("wall_right");
object_id_t const WALL_ID_TOP ("wall_top");
object_id_t const WALL_ID_BOTTOM ("wall_bottom");
