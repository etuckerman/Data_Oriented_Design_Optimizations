//#include "tiles.h"
//
//#include "walls.h" // for wall_t
//
//#include <cstdlib> // for std::memcpy, std::malloc, std::free
//#include <iostream>
//#pragma once
//
//#include "magpie.h"    // for magpie::renderer, magpie::_2d::sprite_batch, magpie::spritesheet
//
//#include "constants.h" // for object_type_t, object_id_t...
//#include "utility.h"   // for vector4, random_getf
//
//#include <map>         // for std::multimap
//
//using namespace std;
//
////tiles_b::tiles_t()
////{
////    for (int i = 0; i < NUM_TILES; i++)
////    {
////        position[i] = { 0.0, 0.0, 0.0, 0.0 };
////    };
////    for (int i = 0; i < NUM_TILES; i++)
////    {
////        velocity[i] = { 0.0, 0.0, 0.0, 0.0 };
////    };
////    for (int i = 0; i < NUM_TILES; i++)
////    {
////        angle_radians[i] = 0;
////    };
////    for (int i = 0; i < NUM_TILES; i++)
////    {
////        is_eaten[i] = false;
////    };
////    for (int i = 0; i < NUM_TILES; i++)
////    {
////        lifetime[i] = TILE_WIDE_LIFETIIME;
////    };
////
////};
//tiles_t::tiles_t()
//{
//    for (int i = 0; i < NUM_TILES; i++)
//    {
//        position[i] = { 0.0, 0.0, 0.0, 0.0 };
//    };
//    for (int i = 0; i < NUM_TILES; i++)
//    {
//        velocity[i] = { 0.0, 0.0, 0.0, 0.0 };
//    };
//    for (int i = 0; i < NUM_TILES; i++)
//    {
//        angle_radians[i] = 0;
//    };
//    for (int i = 0; i < NUM_TILES; i++)
//    {
//        is_eaten[i] = false;
//    };
//    for (int i = 0; i < NUM_TILES; i++)
//    {
//        lifetime[i] = TILE_WIDE_LIFETIIME;
//    };
//}
//
//int main()
//{
//    cout << "test" << tiles_t[1].position;
//
//        return 0;
//};
//
