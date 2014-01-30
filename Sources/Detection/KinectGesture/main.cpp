#include <libfreenect.hpp>
#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <list>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SDL/SDL.h>
#include <libfreenect_sync.h>
#include "MyFreenectDevice.hpp"
#include "Coords.hpp"
#include "Button.hpp"
#include "Finger.hpp"

Freenect::Freenect freenect;
MyFreenectDevice* device;
std::list<Finger*> fList;

void drawPixel(int x, int y, SDL_Surface *w, SDL_Surface *p, SDL_Rect pos, int r, int g, int b)
{
  SDL_FillRect(p, NULL, SDL_MapRGB(w->format, r,g, b));
  pos.x = x;
  pos.y = y;
  SDL_BlitSurface(p, NULL, w, &pos);
}

int getSurfaceMaxY()
{
  int x = 0, y = 0;
  int maxY = 0;

  while (y < 480)
    {
      x = 0;
      while (x < 640)
	{
	  if (device->surfaceMatrix[y][x] == 1)
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

#define FINGER_TIP_THRESHOLD 20

int main(void)
{
  sf::Window App(sf::VideoMode(800, 600, 32), "SFML OpenGL");
  sf::Clock Clock;
  SDL_Surface *w = NULL;
  SDL_Surface *p = NULL, *p2 = NULL;
  SDL_Rect pos;

  p = SDL_CreateRGBSurface(SDL_HWSURFACE, 1, 1, 32, 0, 0, 0, 0);
  p2 = SDL_CreateRGBSurface(SDL_HWSURFACE, 10, 10, 32, 0, 0, 0, 0);
  SDL_Init(SDL_INIT_VIDEO);
  w = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE);
  device = &freenect.createDevice<MyFreenectDevice>(0);
  device->startDepth();
  device->startVideo();
  // Set color and depth clear value
  glClearDepth(1.f);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  // Enable Z-buffer read and write
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // glMatrixMode(GL_MODELVIEW);
  gluPerspective(90.f, 1.f, 1.f, 2500.f);  App.SetActive();
  device->setVideoFormat((freenect_video_format)0);
  uint8_t *dep = (uint8_t*)malloc(640 * 480);
  bzero(dep, sizeof(dep));
  int x, y, z;
  x = -250;
  y = 100;
  z = -270;
  int r = 45;
  float rotateY = 0;
  while (App.IsOpened())
    {
      sf::Event Event;
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      
      glTranslatef(x, y, z);
      while (App.GetEvent(Event))
	{
	  if (Event.Type == sf::Event::Closed)
	    {
	      device->stopDepth();
	      device->stopVideo();
	      exit(0);
	    }
	  if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
	    {
	      device->stopDepth();
	      device->stopVideo();
	      exit(0);
	    }
	  if ((Event.Type == sf::Event::KeyPressed))
	    {
	      if (Event.Key.Code == sf::Key::Z)
		{
		  --rotateY;
		}
	      if (Event.Key.Code == sf::Key::X)
		{
		  ++rotateY;
		}
	      if ((Event.Key.Code == sf::Key::Up))
		{
		  r += 10;
		  device->setTiltDegrees(r);
		}
	      if ((Event.Key.Code == sf::Key::Down))
		{
		  r -= 10;
		  device->setTiltDegrees(r);
		}
	      if ((Event.Key.Code == sf::Key::W))
		{z+=10;
		}
	      if ((Event.Key.Code == sf::Key::S))
		{z-=10;
		}
	      if ((Event.Key.Code == sf::Key::A))
		{x+=10;
		}
	      if ((Event.Key.Code == sf::Key::D))
		{x-=10;}
	      if ((Event.Key.Code == sf::Key::E))
		{y+=10;}
	      if ((Event.Key.Code == sf::Key::Q))
		{y-=10;}
	    }
	}
      if (device->m_new_rgb_frame && !device->isCalibrated)
	{
	  device->calibrate();
	}
      else
	{
	  int x = 0, y = 0;
	  glRotated(180, 1, 0, 0);

	  glBegin(GL_POINTS);
	  glPointSize(1.0);
	  int nb = 0;
	  SDL_FillRect(w, NULL, SDL_MapRGB(w->format, 0,0, 0));
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
		  if (device->surfaceMatrix[y][x] == 1)
		    {
		      if (device->xyzCoordsCurr[y][x][2] < 2047)
			{
			  if (device->xyzCoordsCurr[y][x][2]  < device->xyzCoords[y][x][2] - 5
			      && device->xyzCoordsCurr[y][x][2]  > device->xyzCoords[y][x][2] - 15)
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
	      std::list<coords*> fList;

	      while (y < 480)
		{
		  x = 0;
		  while (x < 640)
		    {
		      if (device->surfaceMatrix[y][x] == 1)
			{
			  if (matrixBin[y][x] == 1)
			    {
			      if (fX == 0)
				{
				  fX = x;
				  fY = y;
				  maxy = 0;
				  int tmpY = maxYSurface;
				  int tmpX = fX;
				  while (tmpX < 640)
				    {
				      bool foundOne = false;
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
					  drawPixel(tmpX, tmpY, w, p, pos, 255, 255, 255);
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

				      bool exist = false;
				      while (it2 != fList.end())
					{
					  if ((*it2)->x >= xacy - FINGER_TIP_THRESHOLD
					      && (*it2)->y >= maxy - FINGER_TIP_THRESHOLD
					      && (*it2)->x <= xacy + FINGER_TIP_THRESHOLD
					      && (*it2)->y <= maxy + FINGER_TIP_THRESHOLD)
					    {
					      exist = true;
					      break;
					    }
					  ++it2;
					}
				      if (!exist)
					fList.push_back(new coords(xacy, maxy, 0));
				    }
				  x = tmpX;
				}
			    }
			  glColor3f(1.0, 1.0, 1.0);
			  glVertex3f(x, y, device->xyzCoords[y][x][2]);
			}
		      ++x;
		    }
		  ++y;
		}
	      std::list<coords*>::iterator it = fList.begin();
	      while (it != fList.end())
		{
		  drawPixel((*it)->x, (*it)->y, w, p2, pos, 255, 255, 0);
		  ++it;
		}
	      std::cout << "List Size = " << fList.size() << std::endl;
	      fList.clear();
	    }
	  glEnd();
	  if (nb < 10000)
	    {
	      device->isCalibrated = false;
	    }
	}
      fList.clear();
      SDL_Flip(w);
      App.Display();
      glFlush();
      
    }
  return 0;
}
