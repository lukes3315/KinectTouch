//
//  main.cpp
//  test
//
//  Created by Lucas McKenna on 1/27/12.
//  Copyright 2012 Epitech. All rights reserved.
//

#include <vector>
#include <iostream>
#include "CamStreamer.h"
#include "BlobDetector.h"
#include "ConvexHullManager.h"
#include "MyFreenectDevice.hpp"
#include "FingerDetector.h"
#include "Menu.hpp"

CvPoint operator-(CvPoint p1, CvPoint p2)
{
  CvPoint p3;

  p3.x = p1.x - p2.x;
  p3.y = p1.y - p2.y;
  return p3;
}

int main (void)
{
  MyFreenectDevice * freenect;
  Freenect::Freenect freeNect;
  IplImage * tmp2 = cvCreateImage(cvSize(640, 480), 8, 3);
  IplImage * tmp = cvCreateImage(cvSize(800, 600), 8, 3);
  CvFont font;
  cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 5);
  freenect = &freeNect.createDevice<MyFreenectDevice>(0);
  std::vector<CvPoint> drawVect;
  Menu *m = new Menu(6);
  
  cvNamedWindow("fingers",  CV_WINDOW_NORMAL);
  cvSetWindowProperty("fingers", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
  cvNamedWindow("surface");
  cvResizeWindow("fingers", 800, 600);
  cvSet(tmp, CV_RGB(255, 255, 255));
  cvMoveWindow("fingers", 0, 1050);
  cvMoveWindow("surface", 0, 0);
  cvSet(tmp, CV_RGB(255, 255, 255));
  cvShowImage("fingers", tmp);
  cvNamedWindow("rgb");
  cvMoveWindow("rgb", 0 , 480);
  cvNamedWindow("depth");
  cvMoveWindow("depth", 640 , 480);
  freenect->startStream();
  freenect->setTilt(0);
  freenect->setThresh(true, 0, 700);
  int waitTime = 0;
  while (freenect->isKinectRunning())
    {
      int i = 1;
      if (freenect->isDepthReady() && freenect->isRgbReady())
	{
	  cvSet(tmp, CV_RGB(255, 255, 255));
	  cvZero(tmp2);
	  IplImage * img =  (IplImage*)cvClone(freenect->fetchKinectRgbFrame());
	  
	  if (freenect->isCalibrated() == false && !freenect->getSurface())
	    {
	      sleep(1);
	      freenect->calibrate();
	    }
	  else if (freenect->isCalibrated() == false)
	    {
	      if (waitTime < 30)
		{
		  cvPutText(tmp, "Initiating manual calibration",
			    cvPoint(250, 200), &font, CV_RGB(255, 0 , 0));
		  cvPutText(tmp, "due to bad lighting conditions", 
			    cvPoint(250, 300), &font, CV_RGB(255, 0 , 0));
		  ++waitTime;
		}
	    }
	  else
	    {
	      freenect->calcFingerPositions();
	      CvSeq * s = freenect->getSurface();
	      CvPoint * sP = freenect->getSurfacePoints();
	      CvRect r = cvBoundingRect(s);
	      if (s)
	      	{
		  for (int i = 0 ; i < 4; ++i)
	      	    {
	      	      if (i == 0)
			cvCircle(tmp2, sP[i] - sP[i], 5, CV_RGB(255, 0, 0), CV_FILLED);
	      	      else if (i == 1)
			cvCircle(tmp2, sP[i] - sP[0], 5, CV_RGB(0, 255, 0), CV_FILLED);
	      	      else if (i == 2)
			cvCircle(tmp2, sP[i]- sP[0], 5, CV_RGB(0, 0, 255), CV_FILLED);
	      	      else if (i == 3)
			cvCircle(tmp2, sP[i]-sP[0], 5, CV_RGB(0, 255, 255), CV_FILLED);
	      	    }
	      	}
	      std::list<Finger> fList = freenect->getFingerList();
	      std::list<CvPoint> fListRaw = freenect->getRawFingerList();

	      for (std::list<CvPoint>::iterator it = fListRaw.begin() ; it != fListRaw.end() ; ++it)
	      	{
		  cvCircle(tmp2, cvPoint(it->x, it->y) - sP[0], 10, CV_RGB(0, 255, 0), CV_FILLED);
		  cvCircle(freenect->fetchKinectDepthFrame(), cvPoint(it->x, it->y),
			   10, CV_RGB(0, 255, 0), CV_FILLED);
	      	}
	      m->checkTouched(0, 0);
	      for (std::list<Finger>::iterator it = fList.begin() ; it != fList.end() ;)
	      	{
		  cvCircle(tmp, cvPoint(it->x, it->y), 10, CV_RGB(255, 0, 0), CV_FILLED);
		  m->checkTouched(it->x, it->y);
		  ++it;
		}
	    }
	  m->drawEstimation(tmp);
	  cvShowImage("fingers", tmp);
	  cvShowImage("surface", tmp2);
	  cvShowImage("rgb", freenect->fetchKinectRgbFrame());
	  cvShowImage("depth", freenect->fetchKinectDepthFrame());
	  cvReleaseImage(&img);
	}
      freenect->update();
      int k = freenect->getKey();
      if (k == 27)
	freenect->setRunning(false);
    }
  freenect->stopStream();
  cvDestroyAllWindows();
  exit(0);
  return 0;
}
