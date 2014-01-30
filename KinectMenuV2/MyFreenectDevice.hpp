#ifndef _FREENECT
#define _FREENECT

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <cmath>
#include <highgui.h>
#include <libfreenect.hpp>
#include "ConvexHullManager.h"
#include "BlobDetector.h"

#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_LEFT 2
#define BOTTOM_RIGHT 3

#define CONVERT(x, y) y * 640 + x

struct Finger
{
  bool isTouching;
  int x;
  int y;
};

class Mutex {
public:
  Mutex() {
    pthread_mutex_init( &m_mutex, NULL );
  }
  void lock() {
    pthread_mutex_lock( &m_mutex );
  }
  void unlock() {
    pthread_mutex_unlock( &m_mutex );
  }

  class ScopedLock
  {
    Mutex & _mutex;
  public:
    ScopedLock(Mutex & mutex)
      : _mutex(mutex)
    {
      _mutex.lock();
    }
    ~ScopedLock()
    {
      _mutex.unlock();
    }
  };
private:
  pthread_mutex_t m_mutex;
};

class MyFreenectDevice : public Freenect::FreenectDevice
{
  struct Coords
  {
    int x, y, z;
  };
public :
  MyFreenectDevice(freenect_context *_ctx, int _index)
    : Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB).bytes),m_buffer_video(freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB).bytes), m_gamma(2048), m_new_rgb_frame(false), m_new_depth_frame(false)
  {
    tmp = 0;
    tmp2 = 0;
    for( unsigned int i = 0 ; i < 2048 ; i++) {
      float v = i/2048.0;
      v = std::pow(v, 3)* 6;
      m_gamma[i] = v*6*256;
    }
    isRunning = false;
    rgbImage = cvCreateImage(cvSize(640, 480), 8, 3);
    depthImage = cvCreateImage(cvSize(640, 480), 8, 3);
    substracted = cvCreateImage(cvSize(640, 480), 8, 3);
    thresh = false;
    first = true;
    surface = 0;
    surfaceStorage = cvCreateMemStorage(0);
    calibrated = false;
    convexHullManager = new ConvexHullManager();
    blob = new BlobDetector();
  }
  
  void VideoCallback(void* _rgb, __attribute__((unused))uint32_t timestamp) {
    Mutex::ScopedLock lock(m_rgb_mutex);
    rgb = static_cast<uint8_t*>(_rgb);
    std::copy(rgb, rgb+getVideoBufferSize(), m_buffer_video.begin());
    convertRgbImage();
    m_new_rgb_frame = true;
  };
  
  void emptyList(std::list<MyFreenectDevice::Coords*> &list)
  {
    if (list.size() > 0)
      {
	std::list<MyFreenectDevice::Coords*>::iterator it = list.begin();
	while (it != list.end())
	  {
	    delete *it;
	    ++it;
	  }
	list.clear();
      }
  }
  
  void unlockRgb()
  {
    
  }
  
  void fillList(std::list<MyFreenectDevice::Coords*> &list, int _size = 640 * 480)
  {
    if ((int)list.size() == _size)
      {
	std::list<MyFreenectDevice::Coords*>::iterator it = list.begin();
	for (int i = 0; i < 480; ++i)
	  {
	    for (int j = 0; j < 640; ++j)
	      {
		(*it)->x = j;
		(*it)->y = i;
		(*it)->z = m_gamma[depth[i * 640 + j]];
		if ((*it)->z == 2047)
		  (*it)->z = 9000;
	      }
	    ++it;
	  }
      }
    else
      {
	for (int i = 0; i < 480; ++i)
	  {
	    for (int j = 0; j < 640; ++j)
	      {
		Coords * c = new Coords();
		c->x = j;
		c->y = i;
		c->z =  m_gamma[depth[i * 640 + j]];
		if (c->z == 2047)
		  c->z = 9000;
		list.push_back(c);
	      }
	  }
      }
  }

  void DepthCallback(void* _depth, uint32_t timestamp) {
    (void)timestamp;
    Mutex::ScopedLock lock(m_depth_mutex);
    depth = static_cast<uint16_t*>(_depth);
    static int i = 0;
    if (first)
      {
	fillList(backgroundList);
	if (!thresh)
	  background = (IplImage*)cvClone(convertDepthImage());
	else
	  background = (IplImage*)cvClone(threshDepth());
	if (i >= 10)
	  first = false;
	else
	  ++i;
      }
    m_new_depth_frame = true;
  }

  void calcFingerPositions()
  {
    CvSeq * surface = getSurface();
    IplImage * tmpDepth = (IplImage*)cvClone(fetchKinectDepthFrame());
    IplImage * diff = subBackgroundToStream();
    if (!tmp)
      {
	tmp = cvCreateImage(cvSize(640, 480), 8, 3);
	tmp2 = cvCreateImage(cvSize(640, 480), 8, 3);
      }
    CvMemStorage * st = cvCreateMemStorage(0);
    std::list<CvRect> rectList;
    pointList.clear();
    cvZero(tmp2);
    cvDrawContours(tmp2, surface, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 8,CV_FILLED);
    realFingerPosition.clear();
    cvZero(tmp);
    blob->findBlobs(diff, GRAY);
    CvPoint * surfacePoints = getSurfacePoints();
    CvSeq * blobs = blob->getBlobs();
    
    for (;blobs; blobs = blobs->h_next)
      {
	CvRect area = cvBoundingRect(blobs);
	
	if (area.width * area.height > 250)
	  {
	    cvDrawContours(tmp, blobs, CV_RGB(255, 255, 255), CV_RGB(0, 0, 0), 8, CV_FILLED);
	    CvSeq * approxedBlob = cvApproxPoly(blobs, sizeof(CvContour), st, CV_POLY_APPROX_DP, 20);
	    convexHullManager->setContours((CvContour*)approxedBlob);
	    convexHullManager->calcConvexity();
	    CvPoint * convexityPoints = convexHullManager->getConvexityPoints();	
	    int hullsize = convexHullManager->getHullSize();

	    for (int j = 0 ; j < hullsize ; ++j)
	      {
		CvRect tmp;

		tmp.x = convexityPoints[j].x - 3;
		tmp.y = convexityPoints[j].y - 3;
		tmp.width = 6;
		tmp.height = 6;
		rectList.push_back(tmp);
	      }
	  }
      }
    for (std::list<CvRect>::iterator it = rectList.begin(); it != rectList.end();++it)
      {
	int x = it->x;
	int y = it->y;
	int closestBluePoint = 10000;
	int closeX = 0;
	int closeY = 0;

	while (x < (it->x + it->width))
	  {
	    y = it->y;
	    while (y < (it->y + it->height))
	      {
		if (y < 480 && x < 640
				   && x > 0 && y > 0)
		  {
		    if (tmp->imageData[CONVERT(x * 3, y * 3)] != 0)
		      {
			diff->imageData[CONVERT(x * 3, y * 3)] = 255;
			diff->imageData[CONVERT(x * 3, y * 3) + 1] = 0;
			diff->imageData[CONVERT(x * 3, y * 3) + 2] = 0;
			int d = getDistanceFromPoint(x, y);
			if (d < closestBluePoint)
			  {
			    closeX = x;
			    closeY = y;
			    closestBluePoint = d;
			  }
		      }
		  }
		++y;
	      }
	    ++x;
	  }
	pointList.push_back(cvPoint(closeX, closeY));
      }
    CvPoint * sP = getSurfacePoints();
    for (std::list<CvPoint>::iterator it = pointList.begin() ; it != pointList.end() ; ++it)
      {
	if (isTouchingSurface(it->x, it->y))
	  {
	    Finger f;

	    int tmpX = it->x;
	    int y = slope1 * (tmpX - sP[1].x) + sP[1].y;
	    cvCircle(tmpDepth, cvPoint(it->x, y), 5, CV_RGB(0, 0, 0), CV_FILLED);
	    int tmpY = it->y;
	    int x = (tmpY / slope2) - sP[2].y + sP[2].x;
	    cvCircle(tmpDepth, cvPoint(x, tmpY), 5, CV_RGB(0, 0, 0), CV_FILLED);
	    y -= sP[0].y;
	    f.x = it->x - surfacePoints[BOTTOM_RIGHT].x;
	    f.y = it->y - surfacePoints[BOTTOM_RIGHT].y + y;
	    f.x *= -1;
	    f.y *= -1;
	    f.x *= 800;
	    f.y *= 600;
	    f.x /= 320;
	    f.y /= 240;
	    f.x += 100;
	    f.isTouching = true;
	    if (f.x > 0 && f.y > 0 
		&& f.x< 800 && f.y < 600)
	      realFingerPosition.push_back(f);
	  }
      }
    cvShowImage("haha", tmpDepth);
    cvClearMemStorage(st);
    cvReleaseMemStorage(&st);
    cvReleaseImage(&tmpDepth);
    blob->releaseImages();
  }
  std::list<CvPoint> getRawFingerList()const
  {
    return pointList;
  }

  CvPoint getTopLeft(CvPoint * surfacePoints)
  {
    int index = 0;
    int minX = 1000;
    for (int j = 0; j < 4; ++j)
      {
    	if (surfacePoints[j].x < minX)
    	  {
    	    minX = surfacePoints[j].x;
    	    index = j;
    	  }
      }
    int index2 = 0;
    for (int j = 0; j < 4; ++j)
      {
    	if (j != index)
    	  {
    	    int tm = surfacePoints[j].x - surfacePoints[index].x;
    	    if (tm < 0)
    	      tm *= -1;
    	    if (tm < 100)
    	      {
    		index2 = j;
    	      }
    	  }
      }
    if (surfacePoints[index].y > surfacePoints[index2].y)
      {
    	index = index2;
      }
    return surfacePoints[index];
  }
  
  CvPoint getTopRight(CvPoint * surfacePoints)
  {
    int index = 0;
    int maxX = 0;
    for (int j = 0; j < 4; ++j)
      {
    	if (surfacePoints[j].x > maxX)
    	  {
    	    maxX = surfacePoints[j].x;
    	    index = j;
    	  }
      }
    int index2 = 0;
    for (int j = 0; j < 4; ++j)
      {
    	if (j != index)
    	  {
    	    int tm = surfacePoints[j].x - surfacePoints[index].x;
    	    if (tm < 0)
    	      tm *= -1;
    	    if (tm < 50)
    	      {
    		index2 = j;
    	      }
    	  }
      }
    if (surfacePoints[index].y > surfacePoints[index2].y)
      {
    	index = index2;
      }
    return surfacePoints[index];
  }
  
  CvPoint getBottomLeft(CvPoint * surfacePoints)
  {
    int index = 0;
    int minX = 1000;
    for (int j = 0; j < 4; ++j)
      {
    	if (surfacePoints[j].x < minX)
    	  {
    	    minX = surfacePoints[j].x;
    	    index = j;
    	  }
      }
    int index2 = 0;
    for (int j = 0; j < 4; ++j)
      {
    	if (j != index)
    	  {
    	    int tm = surfacePoints[j].x - surfacePoints[index].x;
    	    if (tm < 0)
    	      tm *= -1;
    	    if (tm < 50)
    	      {
    		index2 = j;
    	      }
    	  }
      }
    if (surfacePoints[index].y < surfacePoints[index2].y)
      {
    	index = index2;
      }
    return surfacePoints[index];
  }

  CvPoint getBottomRight(CvPoint * surfacePoints)
  {
    int index = 0;
    int maxX = 0;
    for (int j = 0; j < 4; ++j)
      {
    	if (surfacePoints[j].x > maxX)
    	  {
    	    maxX = surfacePoints[j].x;
    	    index = j;
    	  }
      }
    int index2 = 0;
    for (int j = 0; j < 4; ++j)
      {
    	if (j != index)
    	  {
    	    int tm = surfacePoints[j].x - surfacePoints[index].x;
    	    if (tm < 0)
    	      tm *= -1;
    	    if (tm < 50)
    	      {
    		index2 = j;
    	      }
    	  }
      }
    if (surfacePoints[index].y < surfacePoints[index2].y)
      {
	index = index2;
      }
    return surfacePoints[index];
  }

  CvPoint * orderSurfacePoints()
  {
    CvPoint * surfacePoints = getSurfacePoints();
    CvPoint * sPoints = (CvPoint*)malloc(sizeof(CvPoint) * 4);

    sPoints[0] = getTopLeft(surfacePoints);
    sPoints[1] = getTopRight(surfacePoints);
    sPoints[2] = getBottomLeft(surfacePoints);
    sPoints[3] = getBottomRight(surfacePoints);
    return sPoints;
  }

  void startStream()
  {
    isRunning = true;
    startVideo();
    startDepth();
  }

  void startRgbStream()
  {
    isRunning = true;
    startVideo();
  }

  void startDepthStream()
  {
    startDepth();
    isRunning = true;
  }

  void stopStream()
  {
    stopVideo();
    stopDepth();
    isRunning = false;
  }

  void update()
  {
    if (m_new_depth_frame)
      {
	if (!thresh)
	  convertDepthImage();
	else
	  threshDepth();
	fillList(currentFrameList);
      }
  }

  void stopRgbStream()
  {
    stopVideo();
    isRunning = false;
  }

  void stopDepthStream()
  {
    stopDepth();
    isRunning = false;
  }

  int getKey(int timestamp = 2)
  {
    return cvWaitKey(timestamp);
  }

  uint16_t * getRawDepthStream()const
  {
    return depth;
  }

  uint8_t * getRawRgbStream()const
  {
    return rgb;
  }

  bool isDepthReady()const
  {
    return m_new_depth_frame;
  }

  bool isRgbReady()const
  {
    return m_new_rgb_frame;
  }
  
  bool isKinectRunning()const
  {
    return isRunning;
  }

  IplImage * fetchKinectRgbFrame()const
  {
    return rgbImage;
  }

  std::list<Finger> getFingerList()const
  {
    return realFingerPosition;
  }

  IplImage * fetchKinectDepthFrame()const
  {
    return depthImage;
  }
  int calcDistanceBetweenTwoPoints2D(int x, int y, int x1, int y1)
  {
    int diffX = x - x1;
    int diffY = y - y1;
    int distance = sqrt(diffX * diffX + diffY * diffY);
    return distance;
  }

  int calcDistanceBetweenTwoPoints3D(int x, int y, int z, int x1, int y1, int z1)
  {
    int diffX = x - x1;
    int diffY = y - y1;
    int diffZ = z - z1;
    int distance = sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ);
    return distance;
  }

  int getDepthFromPoint(int x, int y)
  {
    if (x > 0 && x < 640
		     && y > 0 && y < 480)
      {
	return fgMatrix[y][x];
      }
    return -1;
  }

  bool isCalibrated()const
  {
    return calibrated;
  }

  void setRunning(bool running)
  {
    isRunning = running;
  }

  void setTilt(int deg)
  {
    setTiltDegrees(deg);
  }

  void setThresh(bool t, int low, int high)
  {
    lowThresh = low;
    highThresh = high;
    thresh = t;
  }

  IplImage * getBackgroundImage()const
  {
    return background;
  }

  IplImage * subBackgroundToStream()
  {
    if (isRunning && !first)
      {
	int i = 0, j = 0;

	cvZero(substracted);
	while (i < 480 * 3)
	  {
	    j = 0;
	    while (j < 640 * 3)
	      {
		if (fgMatrix[i / 3][j / 3] > lowThresh && fgMatrix[i / 3][j / 3] < highThresh)
		  {
		    int d = bgMatrix[i / 3][j / 3] - fgMatrix[i / 3][j / 3];
		    if (d < 0)
		      d *= -1;
		    if (d > 5)
		      {
			substracted->imageData[i * 640 + j] = 255;
			substracted->imageData[i * 640 + j + 1] = 255;
			substracted->imageData[i * 640 + j + 2] = 255;
		      }
		    else
		      {
			substracted->imageData[i * 640 + j] = 0;
			substracted->imageData[i * 640 + j + 1] = 0;
			substracted->imageData[i * 640 + j + 2] = 0;
		      }
		  }
		j += 3;
	      }
	    i += 3;
	  }
	return substracted;
      }
    return substracted;
  }

  int getDistanceFromPoint(int x, int y)
  {
    if (x > 0
	&& y > 0
	&& x < 640
	&& y < 480)
      {
	int distance = fgMatrix[y][x];
	return distance;
      }
    return -1;
  }

  int getDistanceFromPointOnSurface(int x, int y)
  {
    if (x > 0
	&& y > 0
	&& x < 640
	&& y < 480)
      {
	int distance = bgMatrix[y][x];
	return distance;
      }
    return -1;
  }

  CvPoint * getSurfacePoints()
  {
    return surfacePoints;
  }
  int surfaceSize()
  {
    return surface->total;
  }
  bool isTouchingSurface(int x, int y)
  {
    int fgDist, bgDist;
    
    fgDist = fgMatrix[y][x];
    bgDist = bgMatrix[y][x];
    if (bgDist < highThresh)
      {
	int d = fgDist - bgDist;
	if (d < 0)
	  d *= -1;
	if (d < 20)
	  return true;
      }
    return false;
  }

  void calibrate()
  {
    if (m_new_rgb_frame && !calibrated && !surface)
      {
        IplImage * img = (IplImage*)cvClone(fetchKinectRgbFrame());

        IplImage * gray = cvCreateImage(cvSize(640, 480), 8, 1);
        cvZero(gray);
        cvCvtColor(img, gray, CV_RGB2GRAY);

        IplImage * tmpGray;
        tmpGray = (IplImage*)cvClone(gray);
        int Threshold = 100;
        while (Threshold < 200)
          {
            cvThreshold(tmpGray, gray, Threshold, 255, CV_THRESH_BINARY);
            //    cvThreshold(gray, tmpGray, Threshold, 255, CV_THRESH_BINARY);
	    cvShowImage("wtf??", gray);
            CvSeq * t = blob->getAllContoursInImage(gray);
            if (t)
              cvDrawContours(img, t, CV_RGB(255, 0, 0),CV_RGB(0, 255, 0), 8 ,6);
            for (int k = 0; k < 480; ++k)
              {
                for (int k1 = 0; k1 < 640; ++k1)
                  {
                    if (img->imageData[(k * 3) * 640 + (k1 * 3)] == 0
                        && img->imageData[(k * 3) * 640 + (k1 * 3) + 1] == 0
                        && img->imageData[(k * 3) * 640 + (k1 * 3) + 2] != 0)
                      {
                        gray->imageData[k * 640 + k1] = 255;
                      }
                    else
                      gray->imageData[k * 640 + k1] = 0;
                  }
              }
            t = blob->getBiggestContourInImage(gray);
            if (t)
              {
                t = cvApproxPoly(t, sizeof(CvContour), surfaceStorage, CV_POLY_APPROX_DP, 10);
                if (!calibrated)
                  {
                    surfacePoints = (CvPoint*)malloc(sizeof(CvPoint) * t->total);
                    surface = t;
                    cvCvtSeqToArray(surface, surfacePoints, CV_WHOLE_SEQ);
                    surfacePoints = orderSurfacePoints();
		    slope1 = (double)(surfacePoints[3].y - surfacePoints[2].y) / (double)(surfacePoints[3].x - surfacePoints[2].x);
		    slope2 = (double)(surfacePoints[3].y - surfacePoints[1].y) / (double)(surfacePoints[3].x - surfacePoints[1].x);
		    if (surfacePoints[3].x - surfacePoints[1].x == 0)
		      slope2 = 0;
		    std::cout << surfacePoints[2].x - surfacePoints[0].x << std::endl;
		    std::cout << "Slope 1 = "<< slope1 << " Slope2 = "<< slope2<< std::endl;
		    calibrated = true;
		  }
	      }
	    ++Threshold;
            // if (calibrated)                                                                                                                                                                                 
            //   cvReleaseMemStorage(&surfaceStorage);                                                                                                                                                         
          }
        cvReleaseImage(&tmpGray);
        cvReleaseImage(&img);
      }
    else
      {
	
      }

  }

  CvSeq * getSurface()const
  {
    return surface;
  }
  
private :
  IplImage * convertDepthImage()
  {
    if (isRunning)
      {
	int i = 0;
	int j = 0;

	while (i < 480 * 3)
	  {
	    j = 0;
	    while (j < 640 * 3)
	      {
		int distance = m_gamma[depth[(i/3) * 640 + (j/3)]];
		int lb = distance & 0xff;
		
		switch (distance >> 8)
		  {
		  case 0:
		    depthImage->imageData[i * 640 + j] = distance % 255;
		    depthImage->imageData[i * 640 + j + 1] = (distance * 2) % 255;
		    depthImage->imageData[i * 640 + j + 2] = (distance * 3) % 255;
		    break;
		  case 1:
		    depthImage->imageData[i * 640 + j] = 255;
		    depthImage->imageData[i * 640 + j + 1] = 255-lb;
		    depthImage->imageData[i * 640 + j + 2] = 255-lb;
		    break;
		  case 2:
		    depthImage->imageData[i * 640 + j] = 255;
		    depthImage->imageData[i * 640 + j + 1] = lb;
		    depthImage->imageData[i * 640 + j + 2] = 0;
		    break;
		  case 3:
		    depthImage->imageData[i * 640 + j] = 0;
		    depthImage->imageData[i * 640 + j + 1] = 255;
		    depthImage->imageData[i * 640 + j + 2] = 0;
		    break;
		  case 4:
		    depthImage->imageData[i * 640 + j] = 0;
		    depthImage->imageData[i * 640 + j + 1] = 255-lb;
		    depthImage->imageData[i * 640 + j + 2] = 255;
		    break;
		  case 5:
		    depthImage->imageData[i * 640 + j] = 0;
		    depthImage->imageData[i * 640 + j + 1] = 0;
		    depthImage->imageData[i * 640 + j + 2] = 255-lb;
		    break;
		  default :
		    depthImage->imageData[i * 640 + j] = 0;
		    depthImage->imageData[i * 640 + j + 1] = 0;
		    depthImage->imageData[i * 640 + j + 2] = 0;
		    break;
		  }
		j += 3;
	      }
	    i += 3;
	  }
	return depthImage;
      }
    return depthImage;
  }
  IplImage * threshDepth()
  {
    if (isRunning)
      {
	int i = 0;
	int j = 0;

	while (i < 480 * 3)
	  {
	    j = 0;
	    while (j < 640 * 3)
	      {
		int distance = m_gamma[depth[(i/3) * 640 + (j/3)]];
		if (distance > 2047)
		  distance = 0;
		if (first)
		  bgMatrix[i / 3][j / 3] = distance;
		fgMatrix[i / 3][j / 3] = distance;
		
		if (distance > lowThresh && distance < highThresh)
		  {
		    depthImage->imageData[i * 640 + j] = 255;
		    depthImage->imageData[i * 640 + j + 1] = 255;
		    depthImage->imageData[i * 640 + j + 2] = 255;
		  }
		else
		  {
		    depthImage->imageData[i * 640 + j] = 0;
		    depthImage->imageData[i * 640 + j + 1] = 0;
		    depthImage->imageData[i * 640 + j + 2] = 0;
		  }
		j += 3;
	      }
	    i += 3;
	  }
	return depthImage;
      }
    return depthImage;
  }
  
  void zeroMatrix()
  {
    int i = 0;
    int j = 0;

    while (i < 480)
      {
	j = 0;
	while (j < 640)
	  {
	    fgMatrix[i][j] = 0;
	    ++j;
	  }
	++i;
      }
  }

  IplImage * convertRgbImage()
  {
    if (isRunning)
      {
	int i = 0;
	int j = 0;

	while (i < 480 * 3)
	  {
	    j = 0;
	    while (j < 640 * 3)
	      {
		rgbImage->imageData[i * 640 + j] = rgb[i * 640 + j + 2];
		rgbImage->imageData[i * 640 + j + 1] = rgb[i * 640 + j + 1];
		rgbImage->imageData[i * 640 + j + 2] = rgb[i * 640 + j];
		j += 3;
	      }
	    i += 3;
	  }
	return rgbImage;
      }
    return rgbImage;
  }
private :
  std::vector<uint8_t> m_buffer_depth;
  std::vector<uint8_t> m_buffer_video;
  std::vector<uint16_t> m_gamma;
  std::list<MyFreenectDevice::Coords*> backgroundList;
  std::list<MyFreenectDevice::Coords*> currentFrameList;
  Mutex m_rgb_mutex;
  Mutex m_depth_mutex;
  bool m_new_rgb_frame;
  bool m_new_depth_frame;
  uint16_t* depth;
  uint8_t* rgb;
  bool isRunning;
  IplImage * rgbImage;
  IplImage * depthImage;
  IplImage * background;
  IplImage * substracted;
  int bgMatrix[480][640];
  int fgMatrix[480][640];
  int highThresh, lowThresh;
  bool first;
  bool thresh;
  CvSeq * surface;
  bool calibrated;
  CvMemStorage * surfaceStorage;
  CvPoint * surfacePoints;
  BlobDetector * blob;
  ConvexHullManager * convexHullManager;
  std::list<Finger> realFingerPosition;
  std::list<CvPoint> pointList;
  IplImage * tmp, * tmp2;
  double slope1, slope2;
  CvPoint sp1, sp2;
};

#endif
