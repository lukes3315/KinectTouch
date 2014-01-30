#ifndef TILE_HPP
#define TILE_HPP

#include <vector>
#include <cv.h>
#include <highgui.h>

class Tile
{
  friend class Menu;
public :
  Tile();
  ~Tile();
  void drawTile(IplImage*);
private :
  IplImage * icon;
  CvRect rectangle;
  CvScalar color;
  bool touched;
  void setPosition(int, int);
  void setSize(int, int);
  void setColor(CvScalar);
};

#endif
