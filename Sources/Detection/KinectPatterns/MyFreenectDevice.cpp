#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <list>
#include <cmath>
#include <libfreenect_sync.h>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include "Mutex.hpp"
#include "MyFreenectDevice.hpp"

#define RGBTOLERANCE 40
#define FINGER_TIP_THRESHOLD 15

MyFreenectDevice::MyFreenectDevice(freenect_context *_ctx, int _index) : Freenect::FreenectDevice(_ctx, _index),
									 m_buffer_depth(freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB).bytes),
									 m_buffer_video(freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB).bytes),
									 m_gamma(2048),
									 m_new_rgb_frame(false),
									 m_new_depth_frame(false)
{
  for( unsigned int i = 0 ; i < 2048 ; i++)
    {
      float v = i/2048.0;
      v = std::pow(v, 3)* 6;
      m_gamma[i] = v*6*256;
    }
  isCalibrated = false;
  first = false;
  firstDepth = false;
  int x = 0;
  int y = 0;

  while (y < 480)
    {
      x = 0;
      while (x < 640)
	{
	  matrixBin[y][x] = 0;
	  prevMatrixBin[y][x] = 0;
	  ++x;
	}
      ++y;
    }
}

void MyFreenectDevice::resetMainBuffer()
{
  first = false;
}

void MyFreenectDevice::VideoCallback(void* _rgb, uint32_t timestamp) {
  Mutex::ScopedLock lock(m_rgb_mutex);
  rgb = static_cast<uint8_t*>(_rgb);

  (void)timestamp;
  std::copy(rgb, rgb+getVideoBufferSize(), m_buffer_video.begin());
  if (!first)
    {
      baseRgb = (uint8_t*)malloc((640*480*3));
      bzero(baseRgb, (640*480*3));
      int i = 0;
      while (i < 640*480*3)
	{
	  baseRgb[i] = rgb[i];
	  ++i;
	}
      first = true;
    }
  m_new_rgb_frame = true;
}

void MyFreenectDevice::DepthCallback(void* _depth, uint32_t timestamp) {
  Mutex::ScopedLock lock(m_depth_mutex);
  depth = static_cast<uint16_t*>(_depth);
  int i = 0;

  (void)timestamp;
  if (!firstDepth)
    {
      while (i < 480)
	{
	  int j = 0;
	  while (j < 640)
	    {
	      xyzCoords[i][j][0] = j;
	      xyzCoords[i][j][1] = i;
	      xyzCoords[i][j][2] = m_gamma[this->depth[i * 640 + j]];
	      ++j;
	    }
	  ++i;
	}
      firstDepth = true;
    }
  i = 0;
  while (i < 480)
    {
      int j = 0;
      while (j < 640)
	{
	  xyzCoordsCurr[i][j][0] = j;
	  xyzCoordsCurr[i][j][1] = i;
	  xyzCoordsCurr[i][j][2] = m_gamma[this->depth[i * 640 + j]];
	  ++j;
	}
      ++i;
    }
  m_new_depth_frame = true;
}

void MyFreenectDevice::calibrate()
{
  int x = 0,  y = 0;
  static int nbTimes = 0;
  int tol = 150;

  while (y < 480)
    {
      x = 0;
      while (x < 640)
	{
	  prevMatrixBin[y][x] = matrixBin[y][x];
	  ++x;
	}
      ++y;
    }
  glRotated(180, 1, 0, 0);
  glBegin(GL_POINTS);
  y = 0;
  glPointSize(1);
  while (y < 480)
    {
      x = 0;
      while (x < 640)
	{
	  if (compareRgb(x, y))
	    {
	      tol = 0;
	      glColor3f(1, 1, 1);
	      matrixBin[y][x] = 1;
	    }
	  else
	    {
	      glColor3f(baseRgb[(y * 640 + x) * 3]/255.0, 
			baseRgb[(y * 640 + x) * 3 + 1]/255.0, 
			baseRgb[(y * 640 + x) * 3 + 2]/255.0);
	      matrixBin[y][x] = 0;
	      glVertex3f(x, y, 0);
	    }
	  ++x;
	}
      ++y;
    }
  y = 0;
  if (tol == 0)
    {
      while (y < 480)
	{
	  x = 0;
	  while (x < 640)
	    {
	      surfaceMatrix[y][x] = matrixBin[y][x];
	      if (prevMatrixBin[y][x] != matrixBin[y][x])
		{
		  ++tol;
		}
	      ++x;
	    }
	  ++y;
	}
      if (tol < 100)
	{
	  y  = 0;
	  while (y < 480)
	    {
	      x = 0;
	      while (x < 640)
		{
		  glColor3f(1.0, 0, 0);
		  glVertex3f(x, y, 0);
		  ++x;
		}
	      ++y;
	    }
	  isCalibrated = true;
	}
      else
	isCalibrated = false;
    }
  else
    isCalibrated = false;
  glEnd();
  if (nbTimes > 70 && tol > 150)
    {
      resetMainBuffer();
      nbTimes = 0;
    }
  if (isCalibrated == false && tol != 150)
    ++nbTimes;
}

bool MyFreenectDevice::compareRgb(int x, int y)
{
  short rN, gN, bN;
  short rB, gB, bB;

  rN = rgb[(y * 640 + x) * 3];
  gN = rgb[(y * 640 + x) * 3 + 1];
  bN = rgb[(y * 640 + x) * 3 + 2];
  rB = baseRgb[(y * 640 + x) * 3];
  gB = baseRgb[(y * 640 + x) * 3 + 1];
  bB = baseRgb[(y * 640 + x) * 3 + 2];
  if ((rN > rB - RGBTOLERANCE
       && rN < rB + RGBTOLERANCE)
      || (gN > gB - RGBTOLERANCE
	  && gN < gB + RGBTOLERANCE)
      || (bN > bB - RGBTOLERANCE
	  && bN < bB + RGBTOLERANCE))
    return false;
  return true;
}

int MyFreenectDevice::getSurfaceMaxY()
{
  int x = 0, y = 0;
  int maxY = 0;

  while (y < 480)
    {
      x = 0;
      while (x < 640)
	{
	  if (surfaceMatrix[y][x] == 1)
	    {
	      if (y > maxY)
		maxY = y;
	    }
	  ++x;
	}
      ++y;
    }
  return maxY;
}

void MyFreenectDevice::detectFingers()
{
  int x = 0, y = 0;
  int nb = 0;
  int maxy = 0, xacy = 0;
  int fX = 0;
  int fY = 0;
  int matrixBin[480][640];

  while (y < 480)
    {
      x = 0;
      while (x < 640)
	{
	  matrixBin[y][x] = 0;
	  if (surfaceMatrix[y][x] == 1)
	    {
	      if (xyzCoordsCurr[y][x][2] < 2047)
		{
		  if (xyzCoordsCurr[y][x][2]  < xyzCoords[y][x][2] - 5
		      && xyzCoordsCurr[y][x][2]  > xyzCoords[y][x][2] - 15)
		    matrixBin[y][x] = 1;
		}
	      ++nb;
	    }
	  ++x;
	}
      ++y;
    }
  x = 0;
  y = 0;
  if (nb > 10000)
    {
      int maxYSurface = getSurfaceMaxY();
      fList.clear();
      while (y < 480)
	{
	  x = 0;
	  while (x < 640)
	    {
	      if (surfaceMatrix[y][x] == 1)
		{
		  if (matrixBin[y][x] == 1)
		    {
		      // fList.push_back(new coords(xacy, maxy, xyzCoordsCurr[maxy][xacy][2]));
		      if (fX == 0)
			{
			  int tmpY = maxYSurface;
			  int tmpX = x;
			  fX = x;
			  fY = y;
			  maxy = 0;
			  bool foundOne = false;
			  while (tmpX < 640)
			    {
			      tmpY = maxYSurface;
			      while (tmpY > y)
				{
				  if (matrixBin[tmpY][tmpX] == 1)
				    {
				      if (tmpY > maxy)
					{
					  xacy = tmpX;
					  maxy = tmpY;
					}
				      foundOne = true;
				      break;
				    }
				  --tmpY;
				}
			      if (!foundOne)
				break;
			      ++tmpX;
			    }
			  fX = 0;
			  if (tmpX - x > 10)
			    {
			      std::list<coords*>::iterator it2 = fList.begin();

			      // bool exist = false;
			      // while (it2 != fList.end())
			      // 	{
			      // 	  // if ((*it2)->x >= x - FINGER_TIP_THRESHOLD
			      // 	  //     && xyzCoordsCurr[maxy][xacy][2] >= xyzCoords[maxy][xacy][2] - FINGER_TIP_THRESHOLD
			      // 	  //     && (*it2)->x <= xacy + FINGER_TIP_THRESHOLD
			      // 	  //     && xyzCoordsCurr[maxy][xacy][2] <= xyzCoords[maxy][xacy][2] + FINGER_TIP_THRESHOLD)
			      // 	  if ((*it2)->x == x && (*it2)->y == y)
			      // 	    {
			      // 	      exist = true;
			      // 	      break;
			      // 	    }
			      // 	  ++it2;
			      // 	}
			      // if (!exist)
			      // fList.push_back(new coords(xacy, maxy, xyzCoordsCurr[maxy][xacy][2]));
			    }
			  // x = tmpX;
			}
		    }
		}
	      ++x;
	    }
	  ++y;
	}
    }
  if (nb < 10000)
    isCalibrated = false;
}

std::list<coords*> MyFreenectDevice::getFList()const
{
  return fList;
}
