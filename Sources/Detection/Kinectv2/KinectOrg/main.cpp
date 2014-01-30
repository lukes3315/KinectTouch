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
#include "MyFreenectDevice.hpp"
#include "Coords.hpp"
#include "Button.hpp"

std::list<coords*> coordsList;

Freenect::Freenect freenect;
MyFreenectDevice* device;

int check_square_size(int matrixBin[480][640], int yy, int xx)
{
  int size = 0;

  while (matrixBin[yy + size][xx + size] == 1
	 && matrixBin[yy][xx + size] == 1
	 && matrixBin[yy + size][xx] == 1
	 && matrixBin[yy - size][xx - size] == 1
	 && matrixBin[yy][xx - size] == 1
	 && matrixBin[yy - size][xx + size] == 1
	 && matrixBin[yy + size][xx - size] == 1
	 && matrixBin[yy - size][xx] == 1)
    {
      ++size;
    }
  return size;
}

void resetView(bool &b)
{
  int i = 0;
  int tmp = 0;
  static bool firstmove = false;

  coordsList.clear();
  while (i < 480)
    {
      int j = 0;
      while (j < 640)
	{
	  int pval = device->m_gamma[device->depth[i * 640 + j]];
	  coordsList.push_back(new coords(j, i, pval, tmp));
	  if (firstmove == false)
	    device->switchedTable.push_back(new coords(j, i, pval, tmp));
	  ++j;
	  ++tmp;
	}
      ++tmp;
      ++i;
    }
  firstmove = true;
  if (b == false)
    b = true;
}

int kk = 0;

std::list<coords*> fingers;
int matrixDetectOrder[480][640];

int caca(int matrixBin[480][640], int y, int x, int size)
{
  for (int i = y; i < size + y && i < 480; ++i)
    {
      for (int j = x; j < size + x && j < 640; ++j)
	if (matrixBin[i][j] != 1)
	  return 0;
    }
  return 1;
}

int checkSize(int matrixBin[480][640], int yy, int xx)
{
  int size = 1;

  while (caca(matrixBin, yy, xx, size) == 1)
    size++;
  return size;
}

void addFinger(int matrixBin[480][640], int y, int x, int finger)
{
  if (y < 480
      && y >= 0
      && x < 640
      && x >= 0
      && matrixBin[y][x] == -1)
    {
      if (checkSize(matrixBin, y, x) > 3)
	{
	  matrixBin[y][x] = finger;
	  addFinger(matrixBin, y - 1, x, finger);
	  addFinger(matrixBin, y, x - 1, finger);
	  addFinger(matrixBin, y + 1, x, finger);
	  addFinger(matrixBin, y, x + 1, finger);
	}
    }
}

void cleanMatrix(int matrixBin[480][640])
{
}

int main(void)
{
  sf::Window App(sf::VideoMode(800, 600, 32), "SFML OpenGL");
  sf::Clock Clock;

  SDL_Surface *w = NULL;
  SDL_Surface *p = NULL;
  SDL_Rect pos;
  std::list<Button*> buttonList;
  int matrixBin[480][640];

  buttonList.push_back(new Button(new coords(320, 350, 100), 30));
  buttonList.push_back(new Button(new coords(350, 380, 100), 30));
  buttonList.push_back(new Button(new coords(380, 410, 100), 30));
  buttonList.push_back(new Button(new coords(410, 440, 100), 30));
  buttonList.push_back(new Button(new coords(440, 470, 100), 30));
  p = SDL_CreateRGBSurface(SDL_HWSURFACE, 1, 1, 32, 0, 0, 0, 0);
  SDL_Init(SDL_INIT_VIDEO);
  w = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE);

  device = &freenect.createDevice<MyFreenectDevice>(0);
  device->startDepth();
  // Set color and depth clear value
  glClearDepth(1.f);
  glClearColor(0.f, 0.f, 0.f, 0.f);

  // Enable Z-buffer read and write
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  
  // Setup a perspective projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.f, 1.f, 1.f, 2500.f);  App.SetActive();
  int x, y, z;
  x = -250;
  y = 100;
  z = -270;
  int r = 45;
  bool b = false;
  while (App.IsOpened())
    {
      int tmpx = 0, tmpy = 0;
      while (tmpy < 480)
	{
	  tmpx = 0;
	  while (tmpx < 640)
	    {
	      matrixBin[tmpy][tmpx] = 0;
	      matrixDetectOrder[tmpy][tmpx] = 0;
	      ++tmpx;
	    }
	  ++tmpy;
	}
      sf::Event Event;
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(x, y, z);
      glRotated(180, 1, 0, 0);
      glBegin(GL_POINTS);
      glPointSize(1.0);
      kk = 0;
      std::list<Button*>::iterator itBut = buttonList.begin();
      fingers.clear();
      while (App.GetEvent(Event))
	{
	  if (Event.Type == sf::Event::Closed)
	    {
	      device->stopDepth();
	      exit(0);
	    }
	  if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
	    {
	      device->stopDepth();
	      exit(0);
	    }
	  if ((Event.Type == sf::Event::KeyPressed))
	    {
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
		{z-=10;}
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
      if (device->m_new_depth_frame && b == false)
	resetView(b);
      else if (device->m_new_depth_frame && b == true)
	{
	  bool toDetect = false;
	  std::list<coords*>::iterator it;
	  std::list<coords*>::iterator it2;
	  std::list<coords*>::iterator itTmp;

	  it = coordsList.begin();
	  it2 = device->switchedTable.begin();
	  while (it != coordsList.end())
	    {
	      if ((*it)->y > 200)
		{
		  glColor3ub(255, 255, 255);
		  matrixBin[(*it)->y][(*it)->x] = 0;
		  if ((*it)->x == (*it2)->x &&
		      (*it)->y == (*it2)->y &&
		      (*it2)->z < (*it)->z - 100 && (*it)->z < 9000)
		    toDetect = true;
		  if ((*it)->x == (*it2)->x &&
		      (*it)->y == (*it2)->y && toDetect == true)
		    {
		      if ((*it2)->z < (*it)->z - 10 && (*it2)->z > (*it)->z - 20)
			{
			  glColor3ub(0, 0, 0);
			  matrixBin[(*it)->y][(*it)->x] = 1;
			  matrixDetectOrder[(*it)->y][(*it)->x] = 1;
			  glVertex3f((*it2)->x, (*it2)->y, (*it2)->z);
			}
		    }
		}
	      glColor3ub(255, 255, 255);
	      itBut = buttonList.begin();
	      while (itBut != buttonList.end())
		{
		  (*itBut)->draw(*it);
		  ++itBut;
		}
	      glVertex3f((*it)->x, (*it)->y, (*it)->z);
	      ++it;
	      ++it2;
	    }
	}
      SDL_FillRect(w, NULL, SDL_MapRGB(w->format, 0, 0, 0));
      int xx = 0, yy = 0;
      cleanMatrix(matrixDetectOrder);
      while (yy < 480)
	{
	  xx = 0;
	  while (xx < 640)
	    {
	      if (matrixDetectOrder[yy][xx] == 1)
		{
		  // addFinger(matrixDetectOrder, yy, xx, finger);
		  // // std::cout << finger << std::endl;
		  // ++finger;
		  int size;
		  // size = check_square_size(matrixBin, yy, xx);
		  size = checkSize(matrixBin, yy, xx);
		  // itBut = buttonList.begin();
		  if (size > 4)
		    {
		      SDL_FillRect(p, NULL, SDL_MapRGB(w->format, 255, 255, 255));
		      pos.x = xx;
		      pos.y = yy;
		      // //     while (itBut != buttonList.end())
		      // // 	{
		      // // 	  (*itBut)->checkCoords(xx, yy);
		      // // 	  ++itBut;
		      // // 	}
		      SDL_BlitSurface(p, NULL, w, &pos);
		      // checkIfIsFinger(matrixBin, &yy, &xx, p, pos, w);
		    }
		}
	      ++xx;
	    }
	  ++yy;
	}
      // xx = 0;
      // yy = 0;
      // while (yy < 480)
      // 	{
      // 	  xx = 0;
      // 	  while (xx < 640)
      // 	    {
      // 	      if (matrixDetectOrder[yy][xx] == 1)
      // 		{
      // 		  // if (matrixDetectOrder[yy][xx] == 1)
      // 		  //   {
      // 		  SDL_FillRect(p, NULL, SDL_MapRGB(w->format, 255, 255, 255));
      // 		  pos.x = xx;
      // 		  pos.y = yy;
      // 		  SDL_BlitSurface(p, NULL, w, &pos);
      // 		  //   }
      // 		  // else if (matrixDetectOrder[yy][xx] == 2)
      // 		  // {
      // 		  //     SDL_FillRect(p, NULL, SDL_MapRGB(w->format, 255, 0, 0));
      // 		  //     pos.x = xx;
      // 		  //     pos.y = yy;
      // 		  //     SDL_BlitSurface(p, NULL, w, &pos);
      // 		  // //   }
      // 		  // // else if (matrixDetectOrder[yy][xx] == 3)
      // 		  // //   {
      // 		  //     SDL_FillRect(p, NULL, SDL_MapRGB(w->format, 0, 0, 255));
      // 		  //     pos.x = xx;
      // 		  //     pos.y = yy;
      // 		  //     SDL_BlitSurface(p, NULL, w, &pos);
      // 		  // }
      // 		}
      // 	      ++xx;
      // 	    }
      // 	  ++yy;
      // 	}
      SDL_Flip(w);
      glEnd();
      App.Display();
    }
  return 0;
}
