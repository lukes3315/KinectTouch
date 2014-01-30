#ifndef COORDS_HPP
#define COORDS_HPP

class coords
{
public :
  coords(int _x, int _y, int _z, int _count = 0):x(_x), y(_y), z(_z), counter(_count)
  {};
  ~coords(){};
  int x;
  int y;
  int z;
  int counter;
};

class coordsColor
{
public :
  coordsColor(int _x, int _y, int _z, int *color):x(_x), y(_y), z(_z), rgb(color)
  {}
  int x;
  int y;
  int z;
  int *rgb;
};

#endif
