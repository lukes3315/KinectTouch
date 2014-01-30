#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <list>
#include "Button.hpp"

Button::Button(coords *c, int _size):coord(c), size(_size)
{
  isLocked = false;
}

void Button::draw(coords* baseCoords)
{
  if (baseCoords->x > coord->x
      && baseCoords->x < coord->x + size
		  && baseCoords->y > coord->y
      && baseCoords->y < coord->y + size)
	{
	  glColor3f(255, 0, 0);
	  glVertex3f(baseCoords->x, baseCoords->y, baseCoords->z);
	}
}

void Button::checkCoords(int x, int y)
{
  if (isLocked == false)
    {
      if (x >= coord->x && x <= coord->x + size
	  && y >= coord->y && y <= coord->y + size)
	{
	  coord->x = x;
	  coord->y = y;
	  isLocked = true;
	}
    }
  else if (isLocked == true &&  x <= coord->x && x >= coord->x + size
	   && y <= coord->y && y >= coord->y + size)
    isLocked = false;
  else
    {
      coord->x = x;
      coord->y = y;
    }
}
