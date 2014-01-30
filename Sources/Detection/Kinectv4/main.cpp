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

short xyzCoords[480][640][3];
unsigned int indices[480][640];

uint8_t *rgb;
std::list<coords*> coordsList;
std::list<coordsColor*> colorList;

Freenect::Freenect freenect;
MyFreenectDevice* device;

void resetView(bool &b)
{
  int i = 0;
  int tmp = 0;
  static bool firstmove = false;

  coordsList.clear();
  colorList.clear();
  while (i < 480)
    {
      int j = 0;
      while (j < 640)
	{
	  int pval = device->m_gamma[device->depth[i * 640 + j]];
	  xyzCoords[i][j][0] = j;
	  xyzCoords[i][j][1] = i;
	  xyzCoords[i][j][2] = pval;
	  indices[i][j] = i * 640 + j;
	  // coordsList.push_back(new coords(j, i, pval, tmp));
	  ++j;
	  ++tmp;
	}
      ++tmp;
      ++i;
    }
  firstmove = true;
  if (b == false)
    b = true;
  i = 0;
  rgb = (uint8_t*)malloc((640*480*3));
  bzero(rgb, (640*480*3));
  while (i < 640*480*3)
    {
      rgb[i] = device->rgb[i];
      ++i;
    }
}

// int kk = 0;

// std::list<coords*> fingers;
int matrixDetectOrder[480][640];
std::list<coords*> fingerList;
#define FINGERSIZE 10
#define TOLERANCE 7
#define OBJECT -1

int kk = 0;

class Finger 
{
public :
  Finger(coords*cb, coords*ce, int _id = 1):coordBegin(cb), coordEnd(ce)
  {};
  coords* coordBegin;
  coords*coordEnd;
  int id;
};

std::list<Finger*> fingList;

void LoadVertexMatrix()
{
  float fx = 594.21f;
  float fy = 591.04f;
  float a = -0.0030711f;
  float b = 3.3309495f;
  float cx = 339.5f;
  float cy = 242.7f;
  GLfloat mat[16] = {
    1/fx,     0,  0, 0,
    0,    -1/fy,  0, 0,
    0,       0,  0, a,
    -cx/fx, cy/fy, -1, b
  };
  glMultMatrixf(mat);
}
   
void LoadRGBMatrix()
{
  float mat[16] = {
    5.34866271e+02,   3.89654806e+00,   0.00000000e+00,   1.74704200e-02,
    -4.70724694e+00,  -5.28843603e+02,   0.00000000e+00,  -1.22753400e-02,
    -3.19670762e+02,  -2.60999685e+02,   0.00000000e+00,  -9.99772000e-01,
    -6.98445586e+00,   3.31139785e+00,   0.00000000e+00,   1.09167360e-02
  };
  glMultMatrixf(mat);
}

float zoom = 1.0;

int colorMat[480][640][3];

#define RGBTOLERANCE 40

bool compareRgb(MyFreenectDevice*device, int x, int y)
{
  short rN, gN, bN;
  short rB, gB, bB;

  rN = device->rgb[(y * 640 + x) * 3];
  gN = device->rgb[(y * 640 + x) * 3 + 1];
  bN = device->rgb[(y * 640 + x) * 3 + 2];
  rB = rgb[(y * 640 + x) * 3];
  gB = rgb[(y * 640 + x) * 3 + 1];
  bB = rgb[(y * 640 + x) * 3 + 2];
  if ((rN > rB - RGBTOLERANCE
       && rN < rB + RGBTOLERANCE)
      || (gN > gB - RGBTOLERANCE
	  && gN < gB + RGBTOLERANCE)
      || (bN > bB - RGBTOLERANCE
	  && bN < bB + RGBTOLERANCE))
    return false;
  return true;
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
  device->startVideo();
  // Set color and depth clear value
  glClearDepth(1.f);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  // Enable Z-buffer read and write
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  
  GLuint gl_rgb_tex, gl_rgb_tex2;
  // glGenTextures(1, &gl_rgb_tex);
  glGenTextures(1, &gl_rgb_tex2);
  glBindTexture(GL_TEXTURE_2D, gl_rgb_tex2);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Setup a perspective projection
  // glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // glMatrixMode(GL_MODELVIEW);
  gluPerspective(90.f, 1.f, 1.f, 2500.f);  App.SetActive();
  device->setVideoFormat((freenect_video_format)0);
  int x, y, z;
  x = -250;
  y = 100;
  z = -270;
  int r = 45;
  bool b = false;
  float rotateY = 0;
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
      // glRotated(180, 1, 0, 0);
      // glBegin(GL_POINTS);
      // glPointSize(1.0);
      kk = 0;
      std::list<Button*>::iterator itBut = buttonList.begin();
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
      if (b == false && device->m_new_depth_frame && device->m_new_rgb_frame)
      	{
      	  resetView(b);
      	}
      else if (device->m_new_depth_frame || device->m_new_rgb_frame)
	{
	  SDL_FillRect(w, NULL, SDL_MapRGB(w->format, 0, 0, 0));
	  glRotatef(rotateY, 0, 1,0);
	  glRotated(180, 1, 0, 0);
	  int i = 0, j = 0;
	  // LoadRGBMatrix();
	  // LoadVertexMatrix();
	  // LoadVertexMatrix();
	  glBegin(GL_POINTS);
	  int x = 0;
	  int y = 0;
	  glPointSize(1);
	  while (y < (480))
	    {
	      x = 0;
	      while (x < 640)
		{
		  if (rgb != 0)
		    {
		      if (y > 200)
			{
			  if (device->xyzCoords[y][x][2] < 2047
			      && xyzCoords[y][x][2] < 2047)
			    {
			      if(device->xyzCoords[y][x][2] < xyzCoords[y][x][2] - 2
				 && device->xyzCoords[y][x][2] > xyzCoords[y][x][2] - 20
				 && compareRgb(device, x, y))
				{
				  // glColor3f(device->rgb[(y * 640 + x) * 3] / 255.0, 
				  // 	    device->rgb[(y * 640 + x) * 3 + 1] / 255.0,
				  // 	    device->rgb[(y * 640 + x) * 3 + 2] / 255.0);
				  glColor3f(1, 1, 1);
				  SDL_FillRect(p, NULL, SDL_MapRGB(w->format, 255, 255,255));                          
				  pos.x = x;
				  pos.y = y;
				  SDL_BlitSurface(p, NULL, w, &pos);
				  glVertex3f(x, y, device->xyzCoords[y][x][2]);
				}
			      glColor3f(device->rgb[(y * 640 + x) * 3] / 255.0, 
					device->rgb[(y * 640 + x) * 3 + 1] / 255.0,
					device->rgb[(y * 640 + x) * 3 + 2] / 255.0);
			      glVertex3f(x, y, device->xyzCoords[y][x][2]);
			    }
			}
		    }
		  ++x;
		}
	      ++y;
	    }
	  glEnd();
	}
      fingList.clear();
      SDL_Flip(w);
      glFlush();
      App.Display();
    }
  return 0;
}
