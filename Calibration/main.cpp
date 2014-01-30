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
  bool touched = false;
  bool touchedSurface = false;
  int nbTouched = 0;
  std::vector<CvPoint> drawVect;
  
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
  CvPoint sp1, sp2;
  double slope1, slope2;
  int fx = 0, fy = 0;
  int ex = 0, ey = 0;
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
	      //    tmp = cvLoadImage("test.jpg");
	      freenect->calcFingerPositions();
	      CvSeq * s = freenect->getSurface();
	      CvPoint * sP = freenect->getSurfacePoints();
	      CvRect r = cvBoundingRect(s);
	      if (s)
	      	{
		  // slope1 = (double)(sP[1].y - sP[0].y) / (double)(sP[1].x - sP[0].x);
		  // slope2 = (double)(sP[2].y - sP[0].y) / (double)(sP[2].x - sP[0].x);
		  // std::cout << "Slope 1 : " << slope1 << " Slope2 : " << slope2 << std::endl;
		  sp2 = sP[2] - sP[0];
		  sp1 = sP[1] - sP[0];
		  cvLine(tmp2, sP[0] - sP[0], sp1, CV_RGB(255, 255, 255), 2);
		  cvLine(tmp2, sP[0] - sP[0], sp2, CV_RGB(255, 255, 255), 2);
		  cvLine(tmp2, sp2, sP[3] - sP[0], CV_RGB(255, 255, 255), 2);
		  cvLine(tmp2, sp1, sP[3] - sP[0], CV_RGB(255, 255, 255), 2);
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
	      touchedSurface = false;
	      int i = 0;
	      std::vector<CvPoint> twoFirst;
	      for (std::list<Finger>::iterator it = fList.begin() ; it != fList.end() ;)
	      	{
	      	  // if (it->isTouching && it->x > 0 && it->y > 0 && it->x < 800 && it->y < 600)
		  //   {
		  // if (it->x < 30 && it->y < 30)
		  // 	{
		  //  drawVect.push_back(cvPoint(it->x, it->y));
		  twoFirst.push_back(cvPoint(it->x, it->y));
		  if (!touched)
		    {
		      fx = it->x;
		      fy = it->y;
		      touched = true;
		    }
		  if (touched)
		    {
		      // cvCircle(tmp, cvPoint(fx, fy), freenect->calcDistanceBetweenTwoPoints2D(fx, fy, it->x, it->y), CV_RGB(0, 255, 0), CV_FILLED);
		      //    cvRectangle(tmp, cvPoint(fx, fy), cvPoint(it->x, it->y), CV_RGB(255, 0, 0), 4);
		      ex = it->x;
		      ey = it->y;
		    }
		  cvCircle(tmp, cvPoint(it->x, it->y), 10, CV_RGB(255, 0, 0), CV_FILLED);
		  // drawVect.push_back(cvPoint(it->x, it->y));
		  // 	}
		  // else
		  // 	{
		  // int tmpX = it->x - sP[0].x;
		  // int y = slope1 * (tmpX - sp1.x) + sp1.y;
		  // // y *= 600 / 480;
		  // cvCircle(tmp, cvPoint(it->x, it->y - y), 10, CV_RGB(0, 255, 0), CV_FILLED);
		  //   cvCircle(tmp, cvPoint(it->x, it->y), 10, CV_RGB(255, 0, 0), CV_FILLED);
		  // }
		  ++i;
		  touchedSurface = true;
		  // }
		  ++it;
		}
	      // for (int j =  0 ; j < drawVect.size() ; ++j)
	      // 	{
	      // 	  if (j > 0)
	      // 	    {
	      // 	      cvLine(tmp, drawVect[j - 1], drawVect[j], CV_RGB(0, 255, 0), 5);
	      // 	    }
	      // 	}
	      // if (twoFirst.size() >= 2)
	      // 	{
	      // 	  if (twoFirst[0].x > 0 && twoFirst[0].y > 0
	      // 	      && twoFirst[1].x > 0 && twoFirst[1].y > 0
	      // 	      && twoFirst[0].x < 800 && twoFirst[0].y < 600
	      // 							&& twoFirst[1].x < 800 && twoFirst[1].y < 600)
	      // 	    {
	      // 	      cvSetImageROI(tmp, cvRect(twoFirst[0].x, twoFirst[0].y, twoFirst[1].x, twoFirst[1].y));
	      // 	      IplImage *img2 = cvCreateImage(cvGetSize(tmp),
	      // 					     tmp->depth,
	      // 					     tmp->nChannels);
	      // 	      cvCopy(tmp, img2);
	      // 	      cvResetImageROI(tmp);
	      // 	      cvResize(img2, tmp, CV_INTER_NN);
	      // 	    }
	      // 	      }
	    }
	  // if (touchedSurface)
	  //   touched = false;
	  if (!touchedSurface)
	    ++nbTouched;
	  if (nbTouched > 10)
	    {
	      nbTouched = 0;
	      touched = false;
	    }
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
