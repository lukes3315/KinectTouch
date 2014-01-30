#include <list>
#include <cmath>
#include "Mutex.hpp"
#include "MyFreenectDevice.hpp"

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
}

void MyFreenectDevice::VideoCallback(void* _rgb, uint32_t timestamp) {
  Mutex::ScopedLock lock(m_rgb_mutex);
  uint8_t* rgb = static_cast<uint8_t*>(_rgb);

  std::copy(rgb, rgb+getVideoBufferSize(), m_buffer_video.begin());
  m_new_rgb_frame = true;
};

void MyFreenectDevice::DepthCallback(void* _depth, uint32_t timestamp) {
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
