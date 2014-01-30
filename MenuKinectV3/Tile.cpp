#include "Tile.hpp"

Tile::Tile()
{}

Tile::~Tile()
{}

void Tile::drawTile(IplImage *img)
{
  
}

void Tile::setPosition(int x, int y)
{
  rectangle.x = x;
  rectangle.y = y;
}

void Tile::setSize(int width, int height)
{
  rectangle.width = width;
  rectangle.height = height;
}

void Tile::setColor(CvScalar rgb)
{
  color = rgb;
}
