#include "ConvexHullManager.h"

ConvexHullManager::ConvexHullManager(CvContour * contour):contours(contour)
{
  hullsize = 0;
  defectStorage = cvCreateMemStorage(0);
}

ConvexHullManager::ConvexHullManager()
{
}

ConvexHullManager::~ConvexHullManager()
{
  cvClearMemStorage(defectStorage);
  cvReleaseMemStorage(&defectStorage);
}

void ConvexHullManager::setContours(CvContour *contour)
{
    contours = contour;
}

CvContour* ConvexHullManager::getContours()const
{
    return contours;
}

CvPoint * ConvexHullManager::getConvexityPoints()const
{
    return convexityPoints;
}

int ConvexHullManager::getHullSize()const
{
  return hullsize;
}

void ConvexHullManager::calcConvexity()
{
  if (contours)
    {
      PointArray = (CvPoint*)malloc(sizeof(CvPoint) * contours->total); 
      hull = (int*)malloc(sizeof(int)*contours->total);
      cvCvtSeqToArray((CvSeq*)contours, PointArray, CV_WHOLE_SEQ);
      
      CvMat pointMat = cvMat( 1, contours->total, CV_32SC2, PointArray);
      CvMat hullMat = cvMat( 1, contours->total, CV_32SC1, hull );
      cvConvexHull2( &pointMat, &hullMat, CV_CLOCKWISE, 0 );
      hullsize = hullMat.cols;
      
      convexityPoints = (CvPoint*)malloc(sizeof(CvPoint) * contours->total);
      for (int i = 0 ; i < hullsize;++i)
	convexityPoints[i] = PointArray[hull[i]];
      storage = cvCreateMemStorage(0);
      CvPoint * points = getConvexityPoints();
      // defects = cvConvexityDefects(contours, &hullMat, defectStorage);
      // defectArray = (CvConvexityDefect*)malloc(sizeof(CvConvexityDefect) * defects->total);
      // cvCvtSeqToArray(defects, defectArray, CV_WHOLE_SEQ);
      convexityContour = cvCreateSeq(CV_SEQ_POLYGON, sizeof(CvSeq), sizeof(CvPoint), storage); 
      cvSeqPushMulti(convexityContour, points, hullsize);
      cvCvtSeqToArray(convexityContour, convexityPoints, CV_WHOLE_SEQ);
      cvClearMemStorage(storage);
      cvReleaseMemStorage(&storage);
      free(hull);
      free(PointArray);
    }
}

void ConvexHullManager::drawConvexityDefect(IplImage *img)
{
}

void ConvexHullManager::calcConvexityDefect(CvMat hullMat)
{
}

void ConvexHullManager::drawConvexity(IplImage * img, CvScalar color, int lineSize)
{
  CvPoint * points = getConvexityPoints();
  int i = 0;

  if (hullsize > 0)
    cvLine(img, points[0], points[hullsize - 1], CV_RGB(255, 0,  0), lineSize);
  while (i < hullsize - 1)
    {
      cvLine(img, points[i], points[i + 1], color, lineSize, 8);
      // cvCircle(img, points[i], 3, CV_RGB(0, 255, 0), CV_FILLED);
      ++i;
    }
  
  for (int j = 0; j < defects->total; ++j)
    {
      cvCircle(img, cvPoint(defectArray[j].depth_point->x, defectArray[j].depth_point->y),
	       6, CV_RGB(255, 0, 0), CV_FILLED);
    }
  //  free(convexityPoints);
}

CvSeq * ConvexHullManager::getConvexityContour()const
{
  return convexityContour;
}

void ConvexHullManager::releaseArrays()
{
  // free(convexityContour);
  //  free(defectArray);
  //free(defects);
}
