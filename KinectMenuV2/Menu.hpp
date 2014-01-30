#ifndef MENU_HPP
#define MENU_HPP

#include <vector>
#include <list>
#include <pthread.h>
#include "MyFreenectDevice.hpp"
#include "Tiles.hpp"

class Menu
{
public :
  Menu(std::list<IplImage *>, IplImage * img = 0);
  Menu(int nb, IplImage * img = 0);
  ~Menu();
  void interpretGesture(std::list<Finger>);
  void drawMenu(IplImage*);
private :
  void swipe();
  void select();
  void doSwipe();
  void resetGesture();
  // void swipe_right();
  bool processingGesture;
  std::vector<Tiles*> tileList;
  int index;
  std::list<IplImage*> prgImageList;
  int beginGestureX, beginGestureY;
  int endGestureX, endGestureY;
  bool touchedSurface;
  bool touched;
  int nbTouched;
  int nbElements;
  IplImage * menuImg;
};

#endif
