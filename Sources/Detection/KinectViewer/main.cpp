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

class coords
{
public :
  coords(int _x, int _y, int _z, int _count):x(_x), y(_y), z(_z), counter(_count)
  {};
  ~coords(){};
  int x;
  int y;
  int z;
  int counter;
};

class Object
{
public :
  Object(coords*c):coord(c)
  {};
  ~Object(){};
  coords *coord;
};

std::list<coords*> coordsList;
std::list<coords*> switchedTable;
std::list<Object*> listofdetectedItems;

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
public :
  MyFreenectDevice(freenect_context *_ctx, int _index)
    : Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB).bytes),m_buffer_video(freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB).bytes), m_gamma(2048), m_new_rgb_frame(false), m_new_depth_frame(false)
  {
    for( unsigned int i = 0 ; i < 2048 ; i++) {
      float v = i/2048.0;
      v = std::pow(v, 3)* 6;
      m_gamma[i] = v*6*256;
    }
  }
  void VideoCallback(void* _rgb, uint32_t timestamp) {
    Mutex::ScopedLock lock(m_rgb_mutex);
    rgb = static_cast<uint8_t*>(_rgb);
    std::copy(rgb, rgb+getVideoBufferSize(), m_buffer_video.begin());
    m_new_rgb_frame = true;
  };

  void DepthCallback(void* _depth, uint32_t timestamp) {
    Mutex::ScopedLock lock(m_depth_mutex);
    depth = static_cast<uint16_t*>(_depth);
    int i = 0;
    std::list<coords*>::iterator it = switchedTable.begin();
    if (switchedTable.size() == 640 * 480)
      {
	while (i < 480)
	  {
	    int j = 0;
	    while (j < 640)
	      {
		(*it)->x = j;
		(*it)->y = i;
		(*it)->z = m_gamma[this->depth[i * 640 + j]];
		++it;
		++j;
	      }
	    ++i;
	  }
      }
    m_new_depth_frame = true;
  }

public :
  std::vector<uint8_t> m_buffer_depth;
  std::vector<uint8_t> m_buffer_video;
  std::vector<uint16_t> m_gamma;
  Mutex m_rgb_mutex;
  Mutex m_depth_mutex;
  bool m_new_rgb_frame;
  bool m_new_depth_frame;
  uint16_t* depth;
  uint8_t *rgb;
};

Freenect::Freenect freenect;
MyFreenectDevice* device;


struct accelCoords
{
  int x;
  int y;
  int z;
};

bool compare(int x, int y, int z)
{
  std::list<coords*>::iterator it;

  it = coordsList.begin();
  while (it != coordsList.end())
    {
      if ((*it)->x == x && (*it)->y == y && (*it)->z < z - 10)
	{
	  return true;
	}
      ++it;
    }
  return false;
}

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
	    switchedTable.push_back(new coords(j, i, pval, tmp));
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

int main(void)
{
  sf::Window App(sf::VideoMode(800, 600, 32), "SFML OpenGL");
  sf::Clock Clock;

  SDL_Surface *w = NULL;
  SDL_Surface *p = NULL;
  SDL_Rect pos;
  int matrixBin[480][640];

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
  std::list<coords*> fingerList;
  int tmpx = 0, tmpy = 0;
  while (tmpy < 480)
    {
      tmpx = 0;
      while (tmpx < 640)
	{
	  matrixBin[tmpy][tmpx] = 0;
	  ++tmpx;
	}
      ++tmpy;
    }
  while (App.IsOpened())
    {
      sf::Event Event;
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(x, y, z);
      glRotated(180, 1, 0, 0);
      glBegin(GL_POINTS);
      glPointSize(1.0);

      while (App.GetEvent(Event))
	{
	  // Window closed
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
      if (device->m_new_depth_frame && device->m_new_rgb_frame)
	{
	  int i = 0, j = 0;
	  while (j < 480)
	    {
	      i = 0;
	      while (i < 640)
		{
		  if (device->depth[j * 640 + i] < 2000)
		    {
		      glColor3ub(device->depth[(j * 640 + i)] * 0.2,
				 device->depth[(j * 640 + i)] * 0.1,
				 device->depth[(j * 640 + i)] * 0.4);
		      glVertex3f(i, j, device->depth[j * 640 + i]);
		    }
		  ++i;
		}
	      ++j;
	    }
	}
      glEnd();
      App.Display();
    }
  return 0;
}
