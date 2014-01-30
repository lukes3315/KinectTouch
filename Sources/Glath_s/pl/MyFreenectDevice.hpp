#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <libfreenect.hpp>
#include <cmath>


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
  void VideoCallback(void* _rgb, __attribute__((unused))uint32_t timestamp) {
    Mutex::ScopedLock lock(m_rgb_mutex);
    uint8_t* rgb = static_cast<uint8_t*>(_rgb);
    std::copy(rgb, rgb+getVideoBufferSize(), m_buffer_video.begin());
    m_new_rgb_frame = true;
  };

  void DepthCallback(void* _depth, uint32_t timestamp) {
    (void)timestamp;
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

  void Initialize()
  {
    startDepth();
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.f, 1.f, 1.f, 2500.f); 
    b = false;
    x = -250;
    y = 100;
    z = -270;
  }

  void affCallback()
  {
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    // glTranslatef(x, y, z);
    // glRotated(180, 1, 0, 0);
    // glBegin(GL_POINTS);
    // glPointSize(1.0);

    if (m_new_depth_frame && b == false)
      {
	int i = 0;
	int tmp = 0;
	while (i < 480)
	  {
	    int j = 0;
	    while (j < 640)
	      {
		int pval = m_gamma[depth[i * 640 + j]];
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
    else if (m_new_depth_frame && b == true)
      {
	std::list<coords*>::iterator it;
	std::list<coords*>::iterator it2;
	std::list<coords*>::iterator itTmp;

	it = coordsList.begin();
	it2 = switchedTable.begin();
	while (it != coordsList.end())
	  {
	    // glColor3ub(255, 255, 255);
	    if ((*it)->x == (*it2)->x &&
		(*it)->y == (*it2)->y)
	      {
		if ((*it2)->z < (*it)->z - 10 && (*it2)->z > (*it)->z - 20)
		  {
		    // glColor3ub(0, 0, 0);
		    // glVertex3f((*it2)->x, (*it2)->y, (*it2)->z);
		    // float x, y;

		    if ((*it2)->x > (640/3)  && (*it2)->y > (480/3) * 2
		    	&& (*it2)->x < (640/3) * 2 && (*it2)->y < (480/3) * 3)
		      {
			float x, y;

			x = ((*it2)->x * 213.f) / 800.f;
			y = ((*it2)->y * 160.f) / 600.f;
			fingerList.push_front(new coords(x,
							 y,
							 (*it2)->z, 0));
		      }
		    //   {
		    // x = (640 - (*it2)->x) / 640.f;
		    // y = (480 - (*it2)->y) / 480.f;
		    // x *= 1680;
		    // y *= 980;
		    // }
		  }
		// if ((*it)->x > (640/3)  && (*it)->y > (480/3) * 2
		//     && (*it)->x < (640/3) * 2 && (*it)->y < (480/3) * 3)
		//   glColor3ub(255, 0, 0);
		// else
		//   glColor3ub(255, 255, 255);
		// glVertex3f((*it)->x, (*it)->y, (*it)->z);
	      }
	    ++it;
	    ++it2;
	  }
      }
    // glEnd();
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
  bool b;
  std::list<coords*> fingerList;
  int x, y, z;
};

