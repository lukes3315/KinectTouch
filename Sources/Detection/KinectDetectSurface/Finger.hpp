#ifndef FINGER_HPP
#define FINGER_HPP

#include "Coords.hpp"

class Finger
{
public :
  Finger(coords *, coords *);
  ~Finger();
  coords *xyzBegin;
  coords *xyzEnd;
};

#endif
