//
//  main.cpp
//  test
//
//  Created by Lucas McKenna on 1/27/12.
//  Copyright 2012 Epitech. All rights reserved.
//

#include <iostream>
#include "CamStreamer.h"
#include "BlobDetector.h"
#include "ConvexHullManager.h"
#include "MyFreenectDevice.hpp"
#include "FingerDetector.h"
#include "Pong.h"

CvPoint operator-(CvPoint p1, CvPoint p2)
{
  CvPoint p3;

  p3.x = p1.x - p2.x;
  p3.y = p1.y - p2.y;
  return p3;
}

int main (void)
{
  BlobDetector * blob = new BlobDetector();
  ConvexHullManager * convexHullManager = new ConvexHullManager();
  MyFreenectDevice * freenect;
  Freenect::Freenect freeNect;
  IplImage * tmp2 = cvCreateImage(cvSize(640, 480), 8, 3);
  IplImage * tmp = cvCreateImage(cvSize(800, 600), 8, 3);
  CvFont font;

  cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 5);
  freenect = &freeNect.createDevice<MyFreenectDevice>(0);
  CvSeq *surf = 0; 

  cvNamedWindow("fingers",  CV_WINDOW_NORMAL);
  cvSetWindowProperty("fingers", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
  cvResizeWindow("fingers", 800, 600);
  cvSet(tmp, CV_RGB(255, 255, 255));
  cvMoveWindow("fingers", 0, 1050);
  cvSet(tmp, CV_RGB(255, 255, 255));
  
  freenect->startStream();
  freenect->setTilt(0);
  freenect->setThresh(true, 0, 700);
  int waitTime = 0;
  if (freenect->isKinectRunning())
    {
      cvShowImage("fingers", tmp);
      while (freenect->isKinectRunning())
	{
 	  if (freenect->isDepthReady() && freenect->isRgbReady())
	    {
	      if (freenect->isCalibrated() == false && !freenect->getSurface())
		{
		  sleep(1);
		  freenect->calibrate();
		  if (freenect->isCalibrated())
		    break;
		}
	    }
	  int k = freenect->getKey(1);
	  if (k == 27)
	    freenect->setRunning(false);
	}
    }
  cvMoveWindow("fingers", 0, 0);
  cvDestroyWindow("fingers");
  init();
  initPos();
  menu(freenect);
  gameLoop(freenect);
  quitGame();
  freenect->stopStream();
  exit(0);
  return 0;
}

