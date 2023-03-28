#include "tiles.h"

#include "walls.h" // for wall_t

#include <cstdlib> // for std::memcpy, std::malloc, std::free

#include "timer.h"


void matrix_multiply (float output[4][4], float const input_a[4][4], float const input_b[4][4])
{
  // We can access the matrix (4x4 float array) with: matrix [ROW][COLUMN]
  // Remember, these matrices are stored in the ROW-MAJOR format.
  // This means groups of 4 consecutive floats in memory represent a row of that matrix.
  // (With the row-major format this would is not the case,
  // 4 consecutive floats in memory represent a column of that matrix
  // and would be accessed with: matrix [column][row].)
  // This does NOT effect how matrix multiplation is implemented! (It is the same either way.)
  // OUT00 = ROW0 of A dot COLUMN0 of B, etc
  // Just take care that you are actually getting the dot product of
  // a ROW of MATRIX A and a COLUMN of MATRIX B when addressing the memory.
  //
  // A00, A01, A02, A03      B00, B01, B02, B03      O00, O01, O02, O03
  // A10, A11, A12, A13  \/  B10, B11, B12, B13  ==  O10, O11, O12, O13
  // A20, A21, A22, A23  /\  B20, B21, B22, B23  ==  O20, O21, O22, O23
  // A30, A31, A32, A33      B30, B31, B32, B33      O30, O31, O32, O33
  //
  // e.g. OUT00 = A0n dot Bn0 = A00*B00 + A01*B10 + A02*B20 + A03*B30
  // e.g. OUT01 = A0n dot Bn1 = A00*B01 + A01*B11 + A02*B21 + A03*B31
  // e.g. OUT02 = A0n dot Bn2 = A00*B02 + A01*B12 + A02*B22 + A03*B32
  // e.g. OUT10 = A1n dot Bn0 = A10*B00 + A11*B10 + A12*B20 + A13*B30
  //
  // FYI, we can think of multidim arrays as an nD table/'graph' in memory,
  // being accessed with: arr [...][z][y][x]

  for (int row = 0; row < 4; ++row)
  {
    for (int col = 0; col < 4; ++col)
    {
      output [row][col] =
          input_a [row][0] * input_b [0][col]
        + input_a [row][1] * input_b [1][col]
        + input_a [row][2] * input_b [2][col]
        + input_a [row][3] * input_b [3][col];
    }
  }

  // Hint: to implement in SIMD, loading a row of input_a will send you down a dead end!
}


static void collision_resolve_tile_wall (magpie::spritesheet spritesheet, tiles_t& tiles, wall_t* wall, int tiles_index)
{
  // velocity response
  if (wall->get_id () == WALL_ID_LEFT || wall->get_id () == WALL_ID_RIGHT)
  {
    // tile has hit the left or right wall on screen

    // the left and right walls are pefectly aligned with the y-axis
    // therefore, the tile's velocity response is to have its x velocity 'reflected' perfectly
    tiles.vel_x[tiles_index] = -tiles.vel_x[tiles_index];
  }
  else
  {
    // tile has hit the top or bottom wall on screen

    // reflect y velocity
    tiles.vel_y[tiles_index] = -tiles.vel_y[tiles_index];
  }

  // get the tiles spritesheet rect (position, width & height)
  // this is to get the tile's size as required by the following code
  texture_rect const* tex_rect = get_tile_texture_rect (spritesheet, tiles.get_id(tiles_index));

  // position response
  if (wall->get_id () == WALL_ID_LEFT)
  {
    // tile has hit the left wall, lets move it out

    // move tile to the rightmost edge of the left wall
      tiles.pos_x[tiles_index] = wall->position.x + wall->size / 2.0;
    // + half the width of the tile itself (remember the tile's origin is at its centre)
    tiles.pos_x[tiles_index] += (double)tex_rect->width / 2.0;
  }
  else if (wall->get_id () == WALL_ID_RIGHT)
  {
    tiles.pos_x[tiles_index] = wall->position.x - wall->size / 2.0;
    tiles.pos_x[tiles_index] -= (double)tex_rect->width / 2.0;
  }
  else if (wall->get_id () == WALL_ID_TOP)
  {
    tiles.pos_y[tiles_index] = wall->position.y - wall->size / 2.0;
    tiles.pos_y[tiles_index] -= (double)tex_rect->height / 2.0;
  }
  else if (wall->get_id () == WALL_ID_BOTTOM)
  {
    tiles.pos_y[tiles_index] = wall->position.y + wall->size / 2.0;
    tiles.pos_y[tiles_index] += (double)tex_rect->height / 2.0;
  }

  // By adjusting the tile's position we have stopped the tile and wall from overlapping.
  // By reflecting the tile's velocity the tile will not collide with it on the next frame.
  // Job done!


  // N.B.
  // This collision resolution is not fully physically accurate (but is more than acceptable for our purposes.)
  // e.g. on tile collision with y-oriented walls (left and right)
  // we only reflect its x velocity and move it out of the wall.
  // Why could this cause a bug/be less realistic? (Think of other objects, think velocity repsonse timing...)
  // 1 frame
  //                   wall
  //                    |
  //            o-o     |
  //      start | |\    |
  //            o-o \   |
  //                 \  |
  //                 o-o|
  //      ours final | |x
  //                 o-o|\
  //                  / | \
  //                 /  |  \
  //            o-o /   |   \ o-o
  // real final | |/    |    \| | tunnelling final
  //            o-o     |     o-o this would happen if the walls were too thin!
  //                    |
  //
  // But, why is ours solution (generally) acceptable? (Think temporal...)
  // What strategies could help improve this? (Think 'sub-steps', think swept volumes, think collision time...)
  //
  // This is just a thought experiment, please DO NOT attempt to fix this here!
  // More physically accurate collision detection and resolution gets complex real quick.
  // It's just interesting to discuss :)
  // If you want to learn more about how 'real'/commercial collision systems actually work, ask Andy
  // or your 'mega-module' lecturer to point you in the right direction and then do some research!
  // Here is a starter: https://tinyurl.com/mrw4d3k
  // Your final year project gives you an excellent opportunity to explore these sorts of things in depth.
}


// TILE

//tile_t::tile_t ()
//  : position { 0.0, 0.0, 0.0, 0.0 }
//  , velocity { 0.0, 0.0, 0.0, 0.0 }
//{
//  angle_radians = (float)random_getd (0.0, magpie::maths::two_pi <double> ());
//}
//
//
//// TILE NORMAL
//
//tile_normal_t::tile_normal_t ()
//  : is_eaten (false)
//{
//  position.x = random_getd (SCREEN_WIDTH / -2.0, SCREEN_WIDTH / 2.0);
//  position.y = random_getd (SCREEN_HEIGHT / -2.0, SCREEN_HEIGHT / 2.0);
//
//  velocity.x = random_getd (-1.0, 1.0);
//  velocity.y = random_getd (-1.0, 1.0);
//  double const magnitude = magpie::maths::sqrt (velocity.x * velocity.x + velocity.y * velocity.y);
//  velocity.x /= magnitude;
//  velocity.y /= magnitude;
//}
//
//void tile_normal_t::update (double elapsed, magpie::spritesheet spritesheet)
//{
//  // update position
//  position.x += TILE_SPEED_MOVEMENT * velocity.x * elapsed;
//  position.y += TILE_SPEED_MOVEMENT * velocity.y * elapsed;
//
//  // update angle
//  angle_radians += (float)TILE_SPEED_ROTATION * elapsed;
//
//  // limit angle
//  float const angle_limit = magpie::maths::two_pi <float> ();
//  angle_radians = magpie::maths::mod (angle_radians, angle_limit);
//  // Don't understand what is going on here?
//  // Ask Andy to talk through it in your tutorial session.
//  // ...
//  // Now you know what is going on, how would you implement it in SIMD?
//  // Hint: there is no 'mod' function in SIMD.
//}
//void tile_normal_t::render (magpie::renderer& renderer,
//  magpie::_2d::sprite_batch& sprite_batch,
//  magpie::spritesheet spritesheet)
//{
//  texture_rect const* tex_rect = get_tile_texture_rect (spritesheet, get_id ());
//  MAGPIE_DASSERT (tex_rect);
//
//  float const position_x = position.x;
//  float const position_y = position.y;
//  float const angle = angle_radians; // must be in radians!
//  float const scale_x = (float)tex_rect->width;
//  float const scale_y = (float)tex_rect->height;
//
//
//  /////////////////////////////////////////////////////////////////////////////////////
//  //// DO NOT EDIT CODE BELOW - THIS CODE MUST BE IN YOUR TILE RENDER FUNCTION >>> ////
//  /////////////////////////////////////////////////////////////////////////////////////
//
//
//  // Row-/Column-major matrices refers to the order in which matrix elements are stored in memory.
//  // "In row-major order, the consecutive elements of a row reside next to each other,
//  // whereas the same holds true for consecutive elements of a column in column-major order."
//  // https://en.wikipedia.org/wiki/Row-_and_column-major_order
//  // Magpie uses the COLUMN-MAJOR matrices.
//  //
//  // However, to simplify the matrix_multiply function (and eventual optimisation using SIMD),
//  // we are going to calculate our model (world) matrix with the input matrices in row-major format
//  // and then transpose them before using to render the tile (in effect converting it from row-major to column-major
//
//  static alignas (16) float matrix_position [4][4];
//  matrix_position [0][0] = 1.f; matrix_position [0][1] = 0.f; matrix_position [0][2] = 0.f; matrix_position [0][3] = position_x;
//  matrix_position [1][0] = 0.f; matrix_position [1][1] = 1.f; matrix_position [1][2] = 0.f; matrix_position [1][3] = position_y;
//  matrix_position [2][0] = 0.f; matrix_position [2][1] = 0.f; matrix_position [2][2] = 1.f; matrix_position [2][3] = 0.f;
//  matrix_position [3][0] = 0.f; matrix_position [3][1] = 0.f; matrix_position [3][2] = 0.f; matrix_position [3][3] = 1.f;
//  f32 const c = magpie::maths::cos (angle);
//  f32 const s = magpie::maths::sin (angle);
//  static alignas (16) float matrix_rotation [4][4];
//  matrix_rotation [0][0] = c;   matrix_rotation [0][1] = -s;  matrix_rotation [0][2] = 0.f; matrix_rotation [0][3] = 0.f;
//  matrix_rotation [1][0] = s;   matrix_rotation [1][1] = c;   matrix_rotation [1][2] = 0.f; matrix_rotation [1][3] = 0.f;
//  matrix_rotation [2][0] = 0.f; matrix_rotation [2][1] = 0.f; matrix_rotation [2][2] = 1.f; matrix_rotation [2][3] = 0.f;
//  matrix_rotation [3][0] = 0.f; matrix_rotation [3][1] = 0.f; matrix_rotation [3][2] = 0.f; matrix_rotation [3][3] = 1.f;
//  static alignas (16) float matrix_scale [4][4];
//  matrix_scale [0][0] = scale_x; matrix_scale [0][1] = 0.f;     matrix_scale [0][2] = 0.f; matrix_scale [0][3] = 0.f;
//  matrix_scale [1][0] = 0.f;     matrix_scale [1][1] = scale_y; matrix_scale [1][2] = 0.f; matrix_scale [1][3] = 0.f;
//  matrix_scale [2][0] = 0.f;     matrix_scale [2][1] = 0.f;     matrix_scale [2][2] = 1.f; matrix_scale [2][3] = 0.f;
//  matrix_scale [3][0] = 0.f;     matrix_scale [3][1] = 0.f;     matrix_scale [3][2] = 0.f; matrix_scale [3][3] = 1.f;
//
//  static alignas (16) float matrix_position_rotation [4][4];
//  static alignas (16) float matrix_model [4][4];
//
//
//  // matrix_model = matrix_position * matrix_rotation * matrix_scale
//  // Remember, matrix maths dictates that we multiply the matrices in the reverse of order of the desired transformation!
//  // i.e. here we are performing the scaling FIRST, THEN the rotation and FINALLY the translation.
//  matrix_multiply (matrix_position_rotation, matrix_position, matrix_rotation);
//  matrix_multiply (matrix_model, matrix_position_rotation, matrix_scale);
//  *(mat4*)matrix_model = magpie::maths::transpose (*(mat4*)matrix_model);
//
//
//  renderer.sb_draw (sprite_batch, *tex_rect, (float*)matrix_model);
//
//
//  /////////////////////////////////////////////////////////////////////////////////////
//  //// <<< DO NOT EDIT CODE ABOVE - THIS CODE MUST BE IN YOUR TILE RENDER FUNCTION ////
//  /////////////////////////////////////////////////////////////////////////////////////
//}
////
//void tile_normal_t::on_collision (object_type_t other_type, void* other_data, magpie::spritesheet spritesheet)
//{
//  if (other_type == WALL_TYPE)
//  {
//    // 'other_data' is a wall of some kind
//
//    // this tile has hit a wall, make the appropriate changes to this tile as a result of it
//    collision_resolve_tile_wall (spritesheet, this, (wall_t*)other_data);
//  }
//  else if (other_type == PLAYER_TYPE)
//  {
//    // 'other_data' is a player of some kind
//
//    is_eaten = true; // mark this tile as 'eaten' and therefore requires replacing
//  }
//}
//object_id_t tile_normal_t::get_id () const { return TILE_ID_NORMAL; }
//
//bool tile_normal_t::needs_replacing () const
//{
//  if (is_eaten)
//  {
//    return true;
//  }
//  else
//  {
//    return false;
//  }
//}


// TILE WIDE

//tile_wide_t::tile_wide_t ()
//  : is_eaten (false)
//  , lifetime (TILE_WIDE_LIFETIIME)
//{
//  position.x = random_getd (SCREEN_WIDTH / -2.0, SCREEN_WIDTH / 2.0);
//  position.y = random_getd (SCREEN_HEIGHT / -2.0, SCREEN_HEIGHT / 2.0);
//
//  velocity.x = random_getd (-1.0, 1.0);
//  velocity.y = random_getd (-1.0, 1.0);
//  double const magnitude = magpie::maths::sqrt (velocity.x * velocity.x + velocity.y * velocity.y);
//  velocity.x /= magnitude;
//  velocity.y /= magnitude;
//}

//void tile_wide_t::update (double elapsed, magpie::spritesheet spritesheet)
//{
//  // update position
//  position.x += TILE_SPEED_MOVEMENT * velocity.x * elapsed;
//  position.y += TILE_SPEED_MOVEMENT * velocity.y * elapsed;
//
//  // update angle
//  angle_radians += (float)TILE_SPEED_ROTATION * elapsed;
//
//  // limit angle
//  float const angle_limit = magpie::maths::two_pi <float> ();
//  angle_radians = magpie::maths::mod (angle_radians, angle_limit);
//  // Don't understand what is going on here?
//  // Ask Andy to talk through it in your tutorial session.
//  // ...
//  // Now you know what is going on, how would you implement it in SIMD?
//  // Hint: there is no 'mod' function in SIMD.
//
//
//  // update lifetime: tile_wide_t only
//  lifetime -= elapsed;
//}
//void tile_wide_t::render (magpie::renderer& renderer,
//  magpie::_2d::sprite_batch& sprite_batch,
//  magpie::spritesheet spritesheet)
//{
//  texture_rect const* tex_rect = get_tile_texture_rect (spritesheet, get_id ());
//  MAGPIE_DASSERT (tex_rect);
//
//  float const position_x = position.x;
//  float const position_y = position.y;
//  float const angle = angle_radians; // must be in radians!
//  float const scale_x = (float)tex_rect->width;
//  float const scale_y = (float)tex_rect->height;
//
//
//
//
//
//  /////////////////////////////////////////////////////////////////////////////////////
//  //// DO NOT EDIT CODE BELOW - THIS CODE MUST BE IN YOUR TILE RENDER FUNCTION >>> ////
//  /////////////////////////////////////////////////////////////////////////////////////
//
//
//  // Row-/Column-major matrices refers to the order in which matrix elements are stored in memory.
//  // "In row-major order, the consecutive elements of a row reside next to each other,
//  // whereas the same holds true for consecutive elements of a column in column-major order."
//  // https://en.wikipedia.org/wiki/Row-_and_column-major_order
//  // Magpie uses the COLUMN-MAJOR matrices.
//  //
//  // However, to simplify the matrix_multiply function (and eventual optimisation using SIMD),
//  // we are going to calculate our model (world) matrix with the input matrices in row-major format
//  // and then transpose them before using to render the tile (in effect converting it from row-major to column-major
//
//  static alignas (16) float matrix_position [4][4];
//  matrix_position [0][0] = 1.f; matrix_position [0][1] = 0.f; matrix_position [0][2] = 0.f; matrix_position [0][3] = position_x;
//  matrix_position [1][0] = 0.f; matrix_position [1][1] = 1.f; matrix_position [1][2] = 0.f; matrix_position [1][3] = position_y;
//  matrix_position [2][0] = 0.f; matrix_position [2][1] = 0.f; matrix_position [2][2] = 1.f; matrix_position [2][3] = 0.f;
//  matrix_position [3][0] = 0.f; matrix_position [3][1] = 0.f; matrix_position [3][2] = 0.f; matrix_position [3][3] = 1.f;
//  f32 const c = magpie::maths::cos (angle);
//  f32 const s = magpie::maths::sin (angle);
//  static alignas (16) float matrix_rotation [4][4];
//  matrix_rotation [0][0] = c;   matrix_rotation [0][1] = -s;  matrix_rotation [0][2] = 0.f; matrix_rotation [0][3] = 0.f;
//  matrix_rotation [1][0] = s;   matrix_rotation [1][1] = c;   matrix_rotation [1][2] = 0.f; matrix_rotation [1][3] = 0.f;
//  matrix_rotation [2][0] = 0.f; matrix_rotation [2][1] = 0.f; matrix_rotation [2][2] = 1.f; matrix_rotation [2][3] = 0.f;
//  matrix_rotation [3][0] = 0.f; matrix_rotation [3][1] = 0.f; matrix_rotation [3][2] = 0.f; matrix_rotation [3][3] = 1.f;
//  static alignas (16) float matrix_scale [4][4];
//  matrix_scale [0][0] = scale_x; matrix_scale [0][1] = 0.f;     matrix_scale [0][2] = 0.f; matrix_scale [0][3] = 0.f;
//  matrix_scale [1][0] = 0.f;     matrix_scale [1][1] = scale_y; matrix_scale [1][2] = 0.f; matrix_scale [1][3] = 0.f;
//  matrix_scale [2][0] = 0.f;     matrix_scale [2][1] = 0.f;     matrix_scale [2][2] = 1.f; matrix_scale [2][3] = 0.f;
//  matrix_scale [3][0] = 0.f;     matrix_scale [3][1] = 0.f;     matrix_scale [3][2] = 0.f; matrix_scale [3][3] = 1.f;
//
//  static alignas (16) float matrix_position_rotation [4][4];
//  static alignas (16) float matrix_model [4][4];
//
//
//  // matrix_model = matrix_position * matrix_rotation * matrix_scale
//  // Remember, matrix maths dictates that we multiply the matrices in the reverse of order of the desired transformation!
//  // i.e. here we are performing the scaling FIRST, THEN the rotation and FINALLY the translation.
//  matrix_multiply (matrix_position_rotation, matrix_position, matrix_rotation);
//  matrix_multiply (matrix_model, matrix_position_rotation, matrix_scale);
//  *(mat4*)matrix_model = magpie::maths::transpose (*(mat4*)matrix_model);
//
//
//  renderer.sb_draw (sprite_batch, *tex_rect, (float*)matrix_model);
//
//
//  /////////////////////////////////////////////////////////////////////////////////////
//  //// <<< DO NOT EDIT CODE ABOVE - THIS CODE MUST BE IN YOUR TILE RENDER FUNCTION ////
//  /////////////////////////////////////////////////////////////////////////////////////
//}
//
//void tile_wide_t::on_collision (object_type_t other_type, void* other_data, magpie::spritesheet spritesheet)
//{
//  if (other_type == WALL_TYPE)
//  {
//    // 'other_data' is a wall of some kind
//
//    // this tile has hit a wall, make the appropriate changes to this tile as a result of it
//    collision_resolve_tile_wall (spritesheet, this, (wall_t*)other_data);
//  }
//  else if (other_type == PLAYER_TYPE)
//  {
//    // 'other_data' is a player of some kind
//
//    is_eaten = true; // mark this tile as 'eaten' and therefore requires replacing
//  }
//}
//object_id_t tile_wide_t::get_id () const { return TILE_ID_WIDE; }
//
//bool tile_wide_t::needs_replacing () const
//{
//  if (is_eaten || lifetime < 0.0)
//  {
//    return true;
//  }
//  else
//  {
//    return false;
//  }
//}


//TODO: tile editing

/*tiles_t::tiles_t()
{   
    for (int i = 0; i < NUM_TILES; i++) 
    {                                   
        data[i] = data[0];              
    };                                  

    /*for (int i = 0; i < NUM_TILES; i++)
    {
        position[i] = { 0.0, 0.0, 0.0, 0.0 };
    };
    for (int i = 0; i < NUM_TILES; i++)
    {
        velocity[i] = { 0.0, 0.0, 0.0, 0.0 };
    };
    for (int i = 0; i < NUM_TILES; i++)
    {
        angle_radians[i] = 0;
    };
    for (int i = 0; i < NUM_TILES; i++)
    {
        is_eaten[i] = false;
    };
    for (int i = 0; i < NUM_TILES; i++)
    {
        lifetime[i] = TILE_WIDE_LIFETIIME;
    };
}*/

void tiles_t::update(double elapsed, magpie::spritesheet spritesheet)
{
    __m128* pos_x_vector = (__m128*)pos_x;
    __m128* pos_y_vector = (__m128*)pos_y;
    __m128* vel_x_vector = (__m128*)vel_x;
    __m128* vel_y_vector = (__m128*)vel_y;
    __m128 speed = _mm_set1_ps(TILE_SPEED_MOVEMENT * elapsed);

    __m128* ang_rads = (__m128*)angle_radians;
    __m128 angle_speed = _mm_set1_ps((float)TILE_SPEED_ROTATION * elapsed);
    for (size_t i = 0; i < NUM_TILES/4; i++)
    {
        // update position
     //  pos_x[i] += vel_x[i] * speed;                   //simd
     //  pos_y[i] += vel_y[i] * speed;                   //simd
     //  float temp = vel_x[i] * speed;
     //  pos_x[i] = pos_x[i] + temp;

        __m128 temp_x = _mm_mul_ps(vel_x_vector[i], speed);
           pos_x_vector[i] =  _mm_add_ps(pos_x_vector[i], temp_x);

        __m128 temp_y = _mm_mul_ps(vel_y_vector[i], speed);
        pos_y_vector[i] = _mm_add_ps(pos_y_vector[i], temp_y);

        // update angle
        __m128 temp_angle = _mm_mul_ps(ang_rads[i], angle_speed);                     //simd
    }
    for (size_t i = 0; i < NUM_TILES; i++)
    {
        // limit angle
        float const angle_limit = magpie::maths::two_pi <float>();
        angle_radians[i] = magpie::maths::mod(angle_radians[i], angle_limit);
        // Don't understand what is going on here?
        // Ask Andy to talk through it in your tutorial session.
        // ...
        // Now you know what is going on, how would you implement it in SIMD?
        // Hint: there is no 'mod' function in SIMD.


        // update lifetime: tile_wide_t only
        if (tile_id[i] == TILE_ID_WIDE)
        {
            lifetime[i] -= elapsed;
        }
    }
}

void tiles_t::render(magpie::renderer& renderer,
    magpie::_2d::sprite_batch& sprite_batch,
    magpie::spritesheet spritesheet)
{
    for (size_t i = 0; i < NUM_TILES; i++)
    {


        texture_rect const* tex_rect = get_tile_texture_rect(spritesheet, get_id(i));
        MAGPIE_DASSERT(tex_rect);

        float const position_x = pos_x[i];
        float const position_y = pos_y[i];
        float const angle = angle_radians[i]; // must be in radians!
        float const scale_x = (float)tex_rect->width;
        float const scale_y = (float)tex_rect->height;





        /////////////////////////////////////////////////////////////////////////////////////
        //// DO NOT EDIT CODE BELOW - THIS CODE MUST BE IN YOUR TILE RENDER FUNCTION >>> ////
        /////////////////////////////////////////////////////////////////////////////////////


        // Row-/Column-major matrices refers to the order in which matrix elements are stored in memory.
        // "In row-major order, the consecutive elements of a row reside next to each other,
        // whereas the same holds true for consecutive elements of a column in column-major order."
        // https://en.wikipedia.org/wiki/Row-_and_column-major_order
        // Magpie uses the COLUMN-MAJOR matrices.
        //
        // However, to simplify the matrix_multiply function (and eventual optimisation using SIMD),
        // we are going to calculate our model (world) matrix with the input matrices in row-major format
        // and then transpose them before using to render the tile (in effect converting it from row-major to column-major

        static alignas (16) float matrix_position[4][4];
        matrix_position[0][0] = 1.f; matrix_position[0][1] = 0.f; matrix_position[0][2] = 0.f; matrix_position[0][3] = position_x;
        matrix_position[1][0] = 0.f; matrix_position[1][1] = 1.f; matrix_position[1][2] = 0.f; matrix_position[1][3] = position_y;
        matrix_position[2][0] = 0.f; matrix_position[2][1] = 0.f; matrix_position[2][2] = 1.f; matrix_position[2][3] = 0.f;
        matrix_position[3][0] = 0.f; matrix_position[3][1] = 0.f; matrix_position[3][2] = 0.f; matrix_position[3][3] = 1.f;
        f32 const c = magpie::maths::cos(angle);
        f32 const s = magpie::maths::sin(angle);
        static alignas (16) float matrix_rotation[4][4];
        matrix_rotation[0][0] = c;   matrix_rotation[0][1] = -s;  matrix_rotation[0][2] = 0.f; matrix_rotation[0][3] = 0.f;
        matrix_rotation[1][0] = s;   matrix_rotation[1][1] = c;   matrix_rotation[1][2] = 0.f; matrix_rotation[1][3] = 0.f;
        matrix_rotation[2][0] = 0.f; matrix_rotation[2][1] = 0.f; matrix_rotation[2][2] = 1.f; matrix_rotation[2][3] = 0.f;
        matrix_rotation[3][0] = 0.f; matrix_rotation[3][1] = 0.f; matrix_rotation[3][2] = 0.f; matrix_rotation[3][3] = 1.f;
        static alignas (16) float matrix_scale[4][4];
        matrix_scale[0][0] = scale_x; matrix_scale[0][1] = 0.f;     matrix_scale[0][2] = 0.f; matrix_scale[0][3] = 0.f;
        matrix_scale[1][0] = 0.f;     matrix_scale[1][1] = scale_y; matrix_scale[1][2] = 0.f; matrix_scale[1][3] = 0.f;
        matrix_scale[2][0] = 0.f;     matrix_scale[2][1] = 0.f;     matrix_scale[2][2] = 1.f; matrix_scale[2][3] = 0.f;
        matrix_scale[3][0] = 0.f;     matrix_scale[3][1] = 0.f;     matrix_scale[3][2] = 0.f; matrix_scale[3][3] = 1.f;

        static alignas (16) float matrix_position_rotation[4][4];
        static alignas (16) float matrix_model[4][4];


        // matrix_model = matrix_position * matrix_rotation * matrix_scale
        // Remember, matrix maths dictates that we multiply the matrices in the reverse of order of the desired transformation!
        // i.e. here we are performing the scaling FIRST, THEN the rotation and FINALLY the translation.
        matrix_multiply(matrix_position_rotation, matrix_position, matrix_rotation);
        matrix_multiply(matrix_model, matrix_position_rotation, matrix_scale);
        *(mat4*)matrix_model = magpie::maths::transpose(*(mat4*)matrix_model);


        renderer.sb_draw(sprite_batch, *tex_rect, (float*)matrix_model);


        /////////////////////////////////////////////////////////////////////////////////////
        //// <<< DO NOT EDIT CODE ABOVE - THIS CODE MUST BE IN YOUR TILE RENDER FUNCTION ////
        /////////////////////////////////////////////////////////////////////////////////////
    }
}

void tiles_t::on_collision(object_type_t other_type, void* other_data, magpie::spritesheet spritesheet, int tiles_index)
{
    if (other_type == WALL_TYPE)
    {
        // 'other_data' is a wall of some kind

        // this tile has hit a wall, make the appropriate changes to this tile as a result of it
        collision_resolve_tile_wall(spritesheet, *this, (wall_t*)other_data, tiles_index);
    }
    else if (other_type == PLAYER_TYPE)
    {
        // 'other_data' is a player of some kind

        is_eaten[tiles_index] = true; // mark this tile as 'eaten' and therefore requires replacing
    }
}

object_id_t tiles_t::get_id(int index) const
{
    return tile_id[index];
}

bool tiles_t::needs_replacing(int index) const
{
    if (tile_id[index] == TILE_ID_WIDE && lifetime[index] < 0)
        return true;
    return (is_eaten[index]);
}
//object_id_t tile_wide_t::get_id () const { return TILE_ID_WIDE; }
//
//bool tile_wide_t::needs_replacing () const
//{
//  if (is_eaten || lifetime < 0.0)
//  {
//    return true;
//  }
//  else
//  {
//    return false;
//  }
//}

// GENERAL

void initialise_tiles (tiles_t& tiles)
{
  // hhhmmm, what else could go here?
    for (int i = 0; i < NUM_TILES; i++)
    {
        tiles.is_eaten[i] = true;
        tiles.tile_id[i] = TILE_ID_NORMAL;
    }


  replace_expired_tiles (tiles);
}

void create_tile(tiles_t& tiles, int tile_index)
{
    tiles.is_eaten[tile_index] = false;
    tiles.tile_id[tile_index] = TILE_ID_NORMAL;
    {
      tiles.pos_x[tile_index] = random_getd(SCREEN_WIDTH / -2.0, SCREEN_WIDTH / 2.0);
      tiles.pos_y[tile_index] = random_getd (SCREEN_HEIGHT / -2.0, SCREEN_HEIGHT / 2.0);
    
      tiles.vel_x[tile_index] = random_getd (-1.0, 1.0);
      tiles.vel_y[tile_index] = random_getd (-1.0, 1.0);
      double const magnitude = magpie::maths::sqrt (tiles.vel_x[tile_index] * tiles.vel_x[tile_index] + tiles.vel_y[tile_index] * tiles.vel_y[tile_index]);
      tiles.vel_x[tile_index] /= magnitude;
      tiles.vel_y[tile_index] /= magnitude;
    }
}

void create_tile_wide(tiles_t& tiles, int tile_index)
{
    tiles.is_eaten[tile_index] = false;
    tiles.lifetime[tile_index] = TILE_WIDE_LIFETIIME;
    tiles.tile_id[tile_index] = TILE_ID_WIDE;
    {
        tiles.pos_x[tile_index] = random_getd(SCREEN_WIDTH / -2.0, SCREEN_WIDTH / 2.0);
        tiles.pos_y[tile_index] = random_getd(SCREEN_HEIGHT / -2.0, SCREEN_HEIGHT / 2.0);

        tiles.vel_x[tile_index] = random_getd(-1.0, 1.0);
        tiles.vel_y[tile_index] = random_getd(-1.0, 1.0);
        double const magnitude = magpie::maths::sqrt(tiles.vel_x[tile_index] * tiles.vel_x[tile_index] + tiles.vel_y[tile_index] * tiles.vel_y[tile_index]);
        tiles.vel_x[tile_index] /= magnitude;
        tiles.vel_y[tile_index] /= magnitude;
    }
}

void replace_expired_tiles (tiles_t& tiles)
{
  // https://en.cppreference.com/w/cpp/memory/new/operator_new
  // https://en.cppreference.com/w/cpp/language/new#Placement_new
  // https://en.cppreference.com/w/c/memory/malloc
  // https://en.cppreference.com/w/c/memory/free

  // The game requires that there are always active { NUM_TILES } on screen.
  // 1. iterate over tiles, remove tiles that need replacing
  // 2. replace removed tiles with new ones


  //tiles_t tiles_copy = tiles;


  // REMOVE OLD TILES

  //auto it = tiles_copy.data.begin ();
  //while (it != tiles_copy.data.end ())
  //{
  //  tiles_t* tile = (*it).second;
  //  if (tile != nullptr)
  //  {
  //    if (tile->needs_replacing ())
  //    {
  //      // manually call destructor for tile
  //      // we use 'placement new' to construct the tile
  //      // so we must manually call its destructor here
  //      tile->~tiles_t ();
  //      // release the memory we allocated earlier via std::malloc
  //      std::free (tile);

  //      // std::list::erase () returns next element
  //      it = tiles_copy.data.erase (it++);
  //    }
  //    else
  //    {
  //      // increment iterator here in case tile was not removed
  //      it++;
  //    }
  //  }
  //  else
  //  {
  //    // increment iterator here in case tile does not need deleting
  //    it++;
  //  }
  //}

  for (int i = 0; i < NUM_TILES; i++)
      tiles.active[i] = !tiles.needs_replacing(i);


  // CREATE NEW TILES
  
  for (int i = 0; i < NUM_TILES; i++)
  {
    // get memory
    // this just allocates some memory!
    // malloc alone does not call constructors or initialise memory in any way
    // we convert the returned pointer to a 'tile*'
   /* tiles_t* tile = (tiles_t*)std::malloc (1u << 17);*/
      if (!tiles.active[i]) {

          double random = random_getd(0.0, 1.0);
          if (random < PROBABILITY_WIDE)
          {
              create_tile_wide(tiles, i);
              // manually call the tile constructor
              // to create a tile at the memory location we allocated above
              // this is called 'placement new'
              //new (tile) tile_wide_t;
          }
          else
          {
              create_tile(tiles, i);
              /*     new (tile) tile_normal_t;*/
          }
      }
    // insert this new tile into our container
    //tiles_copy.data.emplace (tile->get_id(), tile);
  }

  //tiles = tiles_copy;
}
  

void release_tiles (tiles_t& tiles)
{
  //for (auto& [id, tile] : tiles.data)
  //{
  //  if (tile != nullptr)
  //  {
  //    // manually call destructor for tile
  //    // we use 'placement new' to construct the tile
  //    // so we must manually call its destructor here
  //    tile->~tile_t ();

  //    // release the memory we allocated earlier via std::malloc
  //    std::free (tile);
  //  }
  //}
  //tiles.data.clear ();
}


texture_rect const* get_tile_texture_rect (magpie::spritesheet& spritesheet, object_id_t id)
{
  texture_rect const* rect = nullptr;

  if (id == TILE_ID_WIDE)
  {
    rect = spritesheet.get_sprite_info ("tile_1.png");
  }
  else if (id == TILE_ID_NORMAL)
  {
    rect = spritesheet.get_sprite_info ("tile_0.png");
  }

  return rect;
}
