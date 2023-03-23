#include "utility.h"

#include "magpie.h" // for MAGPIE_DASSERT

#include <cstdlib>  // for rand


double random_getd (double min, double max)
{
  MAGPIE_DASSERT (max > min);
  double const random = (double)rand () / (double)RAND_MAX;
  double const range = max - min;

  return (random * range) + min;
}
