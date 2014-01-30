#include "Tiles.hpp"

Tiles::Tiles(IplImage *_img, CvRect Pos, CvScalar _color) : prgImage(_img), rectangle(Pos), color(_color)
{}

Tiles::Tiles(CvRect Pos, CvScalar _color):rectangle(Pos), color(_color)
{
  prgImage = 0;
}

void Tiles::setTilePos(CvRect rect)
{
  rectangle = rect;
}

CvRect Tiles::getPos()
{
  return rectangle;
}

Tiles::~Tiles()
{}
