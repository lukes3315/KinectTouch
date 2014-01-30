#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <list>
#include "Coords.hpp"

class Button
{
public :
  Button(coords*, int = 5);
  coords *coord;
  int size;
  void draw(coords*);
  void checkCoords(int x, int y);
  bool isLocked;
};

#endif
