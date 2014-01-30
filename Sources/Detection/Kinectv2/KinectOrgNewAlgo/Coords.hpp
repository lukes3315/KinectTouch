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

#endif
