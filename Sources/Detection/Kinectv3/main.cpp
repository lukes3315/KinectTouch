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
std::list<coords*> coordsList;
std::list<coords*> switchedTable;


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
    uint8_t* rgb = static_cast<uint8_t*>(_rgb);
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
		// std::cout << m_gamma[depth[i * 640 + j]] << std::endl;
		++it;
		++j;
	      }
	    ++i;
	  }
      }
    // 	// ++it;
    // 	++i;
    //   }
    // for( unsigned int i = 0 ; i < 640*480 ; i++) {
    //   int pval = m_gamma[depth[i]];
    //   int lb = pval & 0xff;
    // switch (pval>>8) {
    // case 0:
    // 	m_buffer_depth[3*i+0] = 255;
    // 	m_buffer_depth[3*i+1] = 255-lb;
    // 	m_buffer_depth[3*i+2] = 255-lb;
    // 	break;
    // case 1:
    // 	m_buffer_depth[3*i+0] = 255;
    // 	m_buffer_depth[3*i+1] = lb;
    // 	m_buffer_depth[3*i+2] = 0;
    // 	break;
    // case 2:
    // 	m_buffer_depth[3*i+0] = 255-lb;
    // 	m_buffer_depth[3*i+1] = 255;
    // 	m_buffer_depth[3*i+2] = 0;
    // 	break;
    // case 3:
    // 	m_buffer_depth[3*i+0] = 0;
    // 	m_buffer_depth[3*i+1] = 255;
    // 	m_buffer_depth[3*i+2] = lb;
    // 	break;
    // case 4:
    // 	m_buffer_depth[3*i+0] = 0;
    // 	m_buffer_depth[3*i+1] = 255-lb;
    // 	m_buffer_depth[3*i+2] = 255;
    // 	break;
    // case 5:
    // 	m_buffer_depth[3*i+0] = 0;
    // 	m_buffer_depth[3*i+1] = 0;
    // 	m_buffer_depth[3*i+2] = 255-lb;
    // 	break;
    // default:
    // 	m_buffer_depth[3*i+0] = 0;
    // 	m_buffer_depth[3*i+1] = 0;
    // 	m_buffer_depth[3*i+2] = 0;
    // 	break;
    // }
    // }
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

bool check_around(std::list<coords*>::iterator &switched,std::list<coords*>::iterator &base)
{
  std::list<coords*>::iterator itswitched[5];
  std::list<coords*>::iterator switch_save;

  switch_save = switched;
  --switch_save;
  --switch_save;
  itswitched[0] = switch_save;
  ++switch_save;
  itswitched[1] = switch_save;
  ++switch_save;
  ++switch_save;
  itswitched[2] = switch_save;
  ++switch_save;
  itswitched[3] = switch_save;
  ++switch_save;
  itswitched[4] = switch_save;
  int i = 0;

  while (i < 5)
    {
      if ((*itswitched[i])->z > (*switched)->z - 30 && (*itswitched[i])->z < (*switched)->z + 5)
	{
	  glVertex3f((*itswitched[i])->x, (*itswitched[i])->y, (*itswitched[i])->z);
	}
      else
	return false;
      ++i;
    }
  
  return true;
}

int main(void)
{
  sf::Window App(sf::VideoMode(800, 600, 32), "SFML OpenGL");
  sf::Clock Clock;

  SDL_Surface *w = NULL;
  SDL_Surface *p = NULL;
  SDL_Rect pos;
  freenect_raw_tilt_state st;
  int matrixBin[480][640];
  double accelX, accelY, accelZ;

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
      while (App.GetEvent(Event))
	{
	  // Window closed
	  if (Event.Type == sf::Event::Closed)
	    {
	      device->stopDepth();
	      exit(0);
	      // break;
	    }
	  if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
	    {
	      device->stopDepth();
	      exit(0);
	      // break;
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
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(x, y, z);
      glRotated(180, 1, 0, 0);
      glBegin(GL_POINTS);
      glPointSize(1.0);
      if (device->m_new_depth_frame && b == false)
	{
	  int i = 0;
	  int tmp = 0;
	  while (i < 480)
	    {
	      int j = 0;
	      while (j < 640)
		{
		  int pval = device->m_gamma[device->depth[i * 640 + j]];
		  coordsList.push_back(new coords(j, i, pval, tmp));
		  switchedTable.push_back(new coords(j, i, pval, tmp));
		  ++j;
		  ++tmp;
		}
	      ++tmp;
	      ++i;
	    }
	  if (b == false)
	    b = true;
	}
      else if (device->m_new_depth_frame && b == true)
	{
	  bool toDetect = false;
	  std::list<coords*>::iterator it;
	  std::list<coords*>::iterator it2, it3;
	  std::list<coords*>::iterator itTmp;
	  int k = 0;
	  std::list<coords*> filterList;

	  int maxX = 0, maxY=  0, minX = 10000, minY = 10000;
	  int minZ = 1000;
	  it = coordsList.begin();
	  it2 = switchedTable.begin();

	  filterList.clear();
	  while (it != coordsList.end())
	    {
	      if ((*it)->y > 100)
		{
		  glColor3ub(255, 255, 255);
		  //matrixBin[(*it)->y][(*it)->x] = 0;
		  if ((*it2)->z < (*it)->z - 200 && (*it)->z < 9000)
		    {
		      toDetect = true;
		      it3 = it2;
		      // maxX = 0;
		      // maxY = 0;
		      // minX = 10000;
		      // minY = 10000;
		      // minZ = 10000;
		      while ((*it3)->z < (*it)->z - 100)
			{
			  if ((*it3)->x > maxX)
			    maxX = (*it3)->x;
			  if ((*it3)->x < minX)
			    minX = (*it)->x;
			  if ((*it3)->y > maxY)
			    maxY = (*it3)->y;
			  if ((*it3)->x < minY)
			    minY = (*it)->y;
			  if ((*it3)->z < minZ)
			    minZ = (*it3)->z;
			  ++it3;
			}
		      // glColor3f(255, 0, 0);
		      // glVertex3f(minX, minY, (*it)->z);
		      // glVertex3f(maxX, minY, (*it)->z);
		      // glVertex3f(maxX, maxY, (*it)->z);
		      // glVertex3f(minX, maxY, (*it)->z);
		    }
		  else
		    toDetect = false;
		  // if (minX < 10000)
		  //   {
		  // std::cout << minX << minY << maxX << maxY << std::endl;
		  // }
		  if (// (*it2)->z > minZ
		      (*it2)->x >= minX && (*it2)->x <= maxX && (*it2)->y > maxY)
		    // && (*it2)->y <= maxY
		    // && (*it2)->y >= minY)
		    {
		      std::list<coords*>::iterator it5;
		      it5 = it2;
		      if ((*it2)->z < (*it)->z - 5)
			{
			  glColor3f(0, 0, 0);
			  // glVertex3f((*it2)->x, (*it2)->y, (*it2)->z);
			  int tmp = 0;
			  while (tmp < 250)
			    {
			      if ((*it5)->z < (*it)->z - 5 && (*it5)->x <= maxX + 30 && (*it5)->x < minX - 50)
				glVertex3f((*it5)->x, (*it5)->y, (*it5)->z);
			      ++it5;
			      ++tmp;
			    }
			  it5 = it2;
			  tmp = 0;
			  while (tmp < 250)
			    {
			      if ((*it5)->z < (*it)->z - 5 && (*it5)->x <= maxX && (*it5)->x < minX)
				glVertex3f((*it5)->x, (*it5)->y, (*it5)->z);
			      --it5;
			      ++tmp;
			    }
			}
		      else
		      	{
		      	  glColor3f(255, 255, 255);
		      	  glVertex3f((*it2)->x, (*it2)->y, (*it2)->z);}
		    }
		  // else{
		    glColor3f(255, 255, 255);
		    glVertex3f((*it)->x, (*it)->y, (*it)->z);}
		// }
	      ++it;
	      ++it2;
	    }
	}
      SDL_FillRect(w, NULL, SDL_MapRGB(w->format, 0, 0, 0));
      if (fingerList.size() > 15)
	{
	  std::list<coords*>::iterator it;

	  for (it = fingerList.begin(); it != fingerList.end(); ++it)
	    {
	      if (matrixBin[(*it)->y + 1][(*it)->x + 1] == 1 && matrixBin[(*it)->y][(*it)->x + 1] == 1
		  && matrixBin[(*it)->y + 1][(*it)->x] == 1 && matrixBin[(*it)->y - 1][(*it)->x - 1] == 1
		  && matrixBin[(*it)->y - 1][(*it)->x] == 1 && matrixBin[(*it)->y][(*it)->x - 1] == 1)
		{
		  if (matrixBin[(*it)->y + 2][(*it)->x + 2] == 1 && matrixBin[(*it)->y][(*it)->x + 2] == 1
		      && matrixBin[(*it)->y + 2][(*it)->x] == 1 && matrixBin[(*it)->y - 2][(*it)->x - 2] == 1
		      && matrixBin[(*it)->y - 2][(*it)->x] == 1 && matrixBin[(*it)->y][(*it)->x - 2] == 1)
		    // if (matrixBin[(*it)->y + 3][(*it)->x + 3] == 1 && matrixBin[(*it)->y][(*it)->x + 3] == 1
		    // 	&& matrixBin[(*it)->y + 3][(*it)->x] == 1 && matrixBin[(*it)->y - 3][(*it)->x - 3] == 1
		    // 	&& matrixBin[(*it)->y - 3][(*it)->x] == 1 && matrixBin[(*it)->y][(*it)->x - 3] == 1)
		    {
		      SDL_FillRect(p, NULL, SDL_MapRGB(w->format, 255, 255, 255));
		      pos.x = (*it)->x;
		      pos.y = (*it)->z;
		      SDL_BlitSurface(p, NULL, w, &pos);
		    }
		}
	    }
	}
      SDL_Flip(w);
      fingerList.clear();
      glEnd();
      App.Display();
    }
  return 0;
}
