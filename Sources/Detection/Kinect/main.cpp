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
#include "MyFreenectDevice.hpp"

int main(void)
{
  Freenect::Freenect freenect;
  MyFreenectDevice* device;
  sf::Window App(sf::VideoMode(800, 600, 32), "SFML OpenGL");
  sf::Clock Clock;

  device = &freenect.createDevice<MyFreenectDevice>(0);
  device->Initialize();
  App.SetActive();
  int r = 45;
  bool cal = false;
  while (App.IsOpened())
    {
      sf::Event Event;
      while (App.GetEvent(Event))
	{
	  // Window closed
	  if (Event.Type == sf::Event::Closed)
	    {
	      device->stopVideo();
	      device->stopDepth();
	      exit(0);
	      // break;
	    }
	  if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
	    {
	      device->stopDepth();
	      device->stopVideo();
	      exit(0);
	      // break;
	    }
	  if ((Event.Type == sf::Event::KeyPressed))
	    {
	      if ((Event.Key.Code == sf::Key::Up))
		{
		  r += 1;
		  device->setTiltDegrees(r);
		}
	      
	      if ((Event.Key.Code == sf::Key::Down))
		{
		  r -= 1;
		  device->setTiltDegrees(r);
		}
	      if ((Event.Key.Code == sf::Key::Left))
		{
		  device->ry -= 10;
		}
	      if ((Event.Key.Code == sf::Key::Right))
		{
		  device->ry += 10;
		}
	      if ((Event.Key.Code == sf::Key::W))
		{device->z+=10;
		}
	      if ((Event.Key.Code == sf::Key::S))
		{device->z-=10;}
	      if ((Event.Key.Code == sf::Key::A))
		{device->x+=10;
		}
	      if ((Event.Key.Code == sf::Key::D))
		{device->x-=10;}
	      if ((Event.Key.Code == sf::Key::E))
		{device->y+=10;}
	      if ((Event.Key.Code == sf::Key::Q))
		{device->y-=10;}
	    }
	}
      if (cal == false)
	cal = device->calibrate();
      else
	{
	  device->showTouchSurface();
	}
      App.Display();
    }
  return 0;
}
