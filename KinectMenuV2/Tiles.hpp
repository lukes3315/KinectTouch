#ifndef TILES_HPP
#define TILES_HPP

#include <cv.h>
#include <highgui.h>
#include <vector>

class Tiles
{
  friend class Menu;

public :
  Tiles(IplImage *, CvRect Pos, CvScalar _color);
  Tiles(CvRect Pos, CvScalar _color);
  ~Tiles();
private :
  IplImage * prgImage;
  CvRect rectangle;
  CvScalar color;
  CvRect getPos();
  void setTilePos(CvRect);
};

#endif
