#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

class coords
{
public :
  coords(int _x, int _y, int _z, int _count = 0):x(_x), y(_y), z(_z), counter(_count)
  {};
  ~coords(){};
  int x;
  int y;
  int z;
  int counter;
};
int tmpX, tmpY, tmpZ;

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
  uint8_t *baseR;
  void VideoCallback(void* _rgb, __attribute__((unused))uint32_t timestamp) {
    Mutex::ScopedLock lock(m_rgb_mutex);
    rgb = static_cast<uint8_t*>(_rgb);
    static bool f = false;
    
    if (f == false)
      {
	baseR = (uint8_t*)malloc(sizeof(uint8_t*) * 640 * 480 * 3);
	int i = 0;
	while (i < 640 * 480 * 3)
	  {
	    baseR[i] = rgb[i];
	    ++i;
	  }
	f = true;
      }
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
		if ((*it)->z == 2047)
		  {
		    (*it)->z = 90000;
		  }
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
    nbP = 0;
    startDepth();
    startVideo();
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.f, 1.f, 1.f, 2500.f); 
    b = false;
    calib = false;
    x = -250;
    y = 100;
    z = -270;
    rx = 0, ry = 0, rz = 0;
  }

  bool calibrate()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(x, y, z);
    glRotated(180, 1, 0, 0);

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
	b = true;
      }
    else if (m_new_depth_frame && b == true)
      {
	std::list<coords*>::iterator it;
	std::list<coords*>::iterator it2;
	std::list<coords*>::iterator itTmp;
	
	glBegin(GL_POINTS);
	it = coordsList.begin();
	it2 = switchedTable.begin();
	nbP = 0;
	glColor3f(1, 1, 1);
	while (it != coordsList.end())
	  {
	    matrixBin[(*it)->y][(*it)->x] = 0;
	    matAllDist[(*it)->y][(*it)->x] = (*it)->z;
	    if ((*it)->x == (*it2)->x &&
	    	(*it)->y == (*it2)->y && (*it)->z < 500)
	      {
		if ((*it2)->z < (*it)->z - 10 && (*it2)->z > (*it)->z - 20)
		  {
		    matrixBin[(*it)->y][(*it)->x] = (*it)->z;
		  }
	      }
	    ++it;
	    ++it2;
	  }
	int i = 0, j = 0;
	while (i < 480)
	  {
	    j = 0;
	    while (j < 640)
	      {
		if (matrixBin[i][j] != 0)
		  {
		    if (matrixBin[i][j] != 0
			&& matrixBin[i - 1][j - 1] != 0
			&& matrixBin[i - 1][j] != 0
			&& matrixBin[i][j - 1] != 0
			&& matrixBin[i + 1][j + 1] != 0
			&& matrixBin[i + 1][j] != 0
			&& matrixBin[i][j + 1] != 0
			&& matrixBin[i + 1][j - 1] != 0
			&& matrixBin[i - 1][j + 1] != 0)
		      {
			tmpX = j;
			tmpY = i;
			tmpZ = matrixBin[i][j];
			glColor3f(1, 0, 0);
			glVertex3f(tmpX, tmpY, tmpZ);
			++nbP;
		      }
		  }
		++j;
	      }
	    ++i;
	  }
	if (nbP > 200)
	  calib = true;
	if (nbP > 30 && nbP < 150 && calib == false)
	  {
	    if (fingerList.size() > 0 && fingerList.size() < 4)
	      {
		coords *tmp = fingerList.back();
		if (tmp->x > tmpX - 50
		    && tmp->x < tmpX + 50
				&& tmp->y > tmpY - 50
		    && tmp->y < tmpY + 50)
		  {
		    fingerList.back()->x = tmpX;
		    fingerList.back()->y = tmpY;
		    fingerList.back()->z = tmpZ;
		  }
		else
		  fingerList.push_back(new coords(tmpX, tmpY, tmpZ));
	      }
	    else if (fingerList.size() < 4)
	      fingerList.push_back(new coords(tmpX, tmpY, tmpZ));
	  }
	if (calib == true)
	  {
	    std::list<coords*>::iterator ti = fingerList.begin();
	    std::list<coords*> tmpList;

	    while (ti != fingerList.end())
	      {
		tmpList.push_back(*ti);
		++ti;
	      }
	    ti = tmpList.begin();
	    int maxX = 0;
	    int Y = 0;
	    int Z = 0;
	    int tmpit = 0;
	    while (ti != tmpList.end())
	      {
		std::list<coords*>::iterator ti2 = tmpList.begin();
		maxX = 0;
		Y = 0;
		while (ti2 != tmpList.end())
		  {
		    if ((*ti2)->x > maxX)
		      {
			maxX = (*ti2)->x;
			Y = (*ti2)->y;
			Z = (*ti2)->z;
		      }
		    ++ti2;
		  }
		ti2 = tmpList.begin();
		while (ti2 != tmpList.end())
		  {
		    if ((*ti2)->x == maxX)
		      {
			(*ti2)->x = 0;
			(*ti2)->y = 0;
			(*ti2)->z = 0;
		      }
		    ++ti2;
		  }
		Xs[tmpit][0] = maxX;
		Xs[tmpit][1] = Y;
		Xs[tmpit][2] = Z;
		++tmpit;
		++ti;
	      }
	    if (Xs[3][1] < Xs[2][1])
	      {
		tops[0][0] = Xs[3][0];
		tops[0][1] = Xs[3][1];
		tops[0][2] = Xs[3][2];
		tops[1][0] = Xs[2][0];
		tops[1][1] = Xs[2][1];
		tops[1][2] = Xs[2][2];
	      }
	    else if (Xs[3][1] > Xs[2][1])
	      {
		tops[0][0] = Xs[2][0];
		tops[0][1] = Xs[2][1];
		tops[0][2] = Xs[2][2];
		tops[1][0] = Xs[3][0];
		tops[1][1] = Xs[3][1];
		tops[1][2] = Xs[3][2];
	      }
	    if (Xs[0][1] > Xs[1][1])
	      {
		tops[3][0] = Xs[0][0];
		tops[3][1] = Xs[0][1];
		tops[3][2] = Xs[0][2];
		tops[2][0] = Xs[1][0];
		tops[2][1] = Xs[1][1];
		tops[2][2] = Xs[1][2];
	      }
	    else if (Xs[0][1] < Xs[1][1])
	      {
		tops[2][0] = Xs[0][0];
		tops[2][1] = Xs[0][1];
		tops[2][2] = Xs[0][2];
		tops[3][0] = Xs[1][0];
		tops[3][1] = Xs[1][1];
		tops[3][2] = Xs[1][2];
	      }
	  }
      }
    return calib;
  }

  void showTouchSurface()
  {
    int i = 0, j = 0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(x, y, z);
    glRotated(180, 1, 0, 0);
    glBegin(GL_POINTS);
    std::list<coords*>::iterator it2;
    std::list<coords*>::iterator it;

    it = coordsList.begin();
    int LimY;
    int LimX;
    int LimYMin;
    int LimXMin;

    if (tops[0][1] < tops[2][1])
      {
	LimXMin = tops[0][0];
	LimYMin = tops[0][1];
      }
    else
      {
	LimXMin = tops[2][0];
	LimYMin = tops[2][1];
      }
    if (tops[1][1] > tops[3][1])
      {
	LimX = tops[1][0];
	LimY = tops[1][1];
      }
    else
      {
	LimX = tops[3][0];
      	LimY = tops[3][1];
      }
    it2 = switchedTable.begin();
    while (it != coordsList.end())
      {
	matrixBin[(*it)->y][(*it)->x] = 0;
	matAllDist[(*it)->y][(*it)->x] = (*it)->z;
	if ((*it)->x == (*it2)->x &&
	    (*it)->y == (*it2)->y && (*it)->z < 500)
	  {
	    if ((*it2)->z < (*it)->z - 10 && (*it2)->z > (*it)->z - 20)
	      {
		if ((*it2)->z < matAllDist[LimYMin][LimXMin]
				&& (*it2)->z > matAllDist[LimY][LimX]
		  && (*it2)->x > tops[1][0] && (*it2)->x < tops[3][0])
		  {
		    matrixBin[(*it)->y][(*it)->x] = (*it)->z;
		  }
	  }
      }
	++it;
	++it2;
  }
    i = 0, j = 0;
  while (i < 480)
    {
      j = 0;
      while (j < 640)
	{
	  if (matrixBin[i][j] != 0)
	    {
	      if (matrixBin[i][j] != 0
		  && matrixBin[i - 1][j - 1] != 0
		  && matrixBin[i - 1][j] != 0
		  && matrixBin[i][j - 1] != 0
		  && matrixBin[i + 1][j + 1] != 0
		  && matrixBin[i + 1][j] != 0
		  && matrixBin[i][j + 1] != 0
		  && matrixBin[i + 1][j - 1] != 0
		  && matrixBin[i - 1][j + 1] != 0)
		{
		  glColor3f(1, 0, 0);
		  glVertex3f(j, i, matrixBin[i][j]);
		}
	    }
	  ++j;
	}
      ++i;
    }
  std::list<coords*>::iterator it3 = fingerList.begin();
  i = 0;
    
  glColor3f(1, 1, 1);
  while (i < 4)
    {
      glVertex3f(tops[i][0], tops[i][1], matAllDist[tops[i][1]][tops[i][0]]);
      ++i;
    }
  i = 3;
  glEnd();
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
uint8_t* rgb;
bool b;
int rx, ry, rz;
std::list<coords*> fingerList;
std::list<coords*> tmp;
int x, y, z;
int nbP;
int matrixBin[480][640];
int matAllDist[480][640];
bool calib;
int Xs[4][3];
int tops[4][3];
int matVals[480][640];
int x1;
int x2;
int L1;
};
