#ifndef FINGERDETECTOR_H
#define FINGERDETECTOR_H

#include <iostream>
#include <list>
#include <cv.h>
#include <highgui.h>
#include "ConvexHullManager.h"

//10 = AVERAGE TOO MANY POINTS
//15 = OK
//20 = OK
//30 = BAD
#define THRESH 15

class FingerDetector
{
 public :
  FingerDetector();
  ~FingerDetector();
  struct Coords
  {
    int x;
    int y;
  };
  void findFingers(ConvexHullManager *);
  std::list<Coords*> getFingerList()const;
 private :
  std::list<std::list<CvPoint> > pointList;
  std::list<CvPoint> regionList;
  std::list<Coords*> fingerList;
};

#endif
