#pragma once


struct vector4
{
  double x;
  double y;
  double z;
  double w;
};


/// <summary>
/// returns a number between min and max inclusive
/// </summary>
/// <param name="min">minimum random number (inclusive)</param>
/// <param name="max">maximum random number (inclusive)</param>
/// <returns>random number between min & max (inclusive)</returns>
double random_getd (double min, double max);
