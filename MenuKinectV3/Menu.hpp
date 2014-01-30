#ifndef MENU_CPP
#define MENU_CPP

#include <cv.h>
#include <vector>
#include <highgui.h>
#include "Tile.hpp"

class Menu
{
public :
  Menu();
  Menu(int);
  ~Menu();
  void drawEstimation(IplImage*);
  //Menu(std::vector<IplImage*>, std::vector<function*>);
  void checkTouched(int, int);
private :
  std::vector<Tile*> tileList;
  int nbTiles;
};

#endif
