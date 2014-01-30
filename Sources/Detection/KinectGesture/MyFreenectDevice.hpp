#ifndef _MYKINECTDEVICE_HPP
#define _MYKINECTDEVICE_HPP

#include <libfreenect.hpp>
#include <pthread.h>
#include <vector>
#include "Mutex.hpp"
#include "Coords.hpp"

class MyFreenectDevice : public Freenect::FreenectDevice
{
public :
  MyFreenectDevice(freenect_context *_ctx, int _index);
  void VideoCallback(void* _rgb, uint32_t timestamp);
  void DepthCallback(void* _depth, uint32_t timestamp);
  void calibrate();
  bool compareRgb(int, int);
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
  uint8_t *baseRgb;
  short xyzCoords[480][640][3];
  short xyzCoordsCurr[480][640][3];
  bool first;
  bool firstDepth;
  bool isCalibrated;
  int matrixBin[480][640];
  int prevMatrixBin[480][640];
  int surfaceMatrix[480][640];
  void resetMainBuffer();
};

#endif
