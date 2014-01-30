#include <cmath>
#include "Menu.hpp"

#define LIMITNEXT 50
#define LIMITPREV 750

Menu::Menu(int nb, IplImage * img):nbElements(nb), menuImg(img)
{
  beginGestureX = 0;
  beginGestureY = 0;
  endGestureX = 0;
  endGestureY = 0;
  touchedSurface = false;
  touched = false;
  nbTouched = 0;
  int margin = 0;
  processingGesture = false;
  index = 0;
  CvScalar color;
  for (int i = 0 ; i < nbElements ; ++i)
    {
      if (i == 0)
	color = CV_RGB(255, 0, 0);
      if (i == 1)
	color = CV_RGB(0, 0, 255);
      if (i == 2)
	color = CV_RGB(0, 255, 0);
      if (i == 3)
	color = CV_RGB(255, 0, 255);
      if (i == 4)
	color = CV_RGB(255, 255, 0);
      tileList.push_back(new Tiles(cvRect(200 + margin, 100, 400, 400), color));
      margin += 800;
    }
}

Menu::Menu(std::list<IplImage*> List, IplImage *img):prgImageList(List), menuImg(img)
{
  processingGesture = false;
  beginGestureX = 0;
  beginGestureY = 0;
  endGestureX = 0;
  endGestureY = 0;
  touchedSurface = false;
  touched = false;
  nbTouched = 0;
  index = 0;
  nbElements = prgImageList.size();
}

Menu::~Menu()
{
  
}

void Menu::interpretGesture(std::list<Finger> fingerList)
{
  touchedSurface = false;
  for (std::list<Finger>::iterator it = fingerList.begin() ; it != fingerList.end() ; ++it)
    {
      if (!touched)
	{
	  beginGestureX = it->x;
	  beginGestureY = it->y;
	  touched = true;
	}
      if (touched)
	{
	  endGestureX = it->x;
	  endGestureY = it->y;
	}
      touchedSurface = true;
    }
  if (beginGestureX != 0)
    {
      int distX = beginGestureX - endGestureX;
      int distY = beginGestureY - endGestureY;
      int distance = sqrt(distX * distX + distY * distY);
      // if (distance > 100)
      // 	{
      swipe();
      // }
      // else
      // 	{
      //   select();
      // }
    }
  if (!touchedSurface)
    {
      ++nbTouched;
    }
  if (nbTouched > 2)
    {
      beginGestureX = 0;
      beginGestureY = 0;
      endGestureX = 0;
      endGestureY = 0;
      nbTouched = 0;
      touched = false;
      processingGesture = false;
    }
}

void Menu::resetGesture()
{
  beginGestureX = 0;
  beginGestureY = 0;
  endGestureX = 0;
  endGestureY = 0;
  touched = false;
}

void Menu::doSwipe()
{
  int margin = 800;

  for (int i = 0 ; i < nbElements; ++i)
    {
      resetGesture();
      if (i == index)
	{
	  tileList[i]->setTilePos(cvRect(200, 100, 400, 400));
	}
      else
	{
	  if (i < index)
	    {
	      int tmpmargin = margin * (i - index);
	      if (tmpmargin < 0)
		tmpmargin *= -1;
	      tileList[i]->setTilePos(cvRect(200 - tmpmargin, 100, 400, 400));
	    }
	  else if (i > index)
	    {
	      int tmpmargin = margin * i;
	      tileList[i]->setTilePos(cvRect(200 + tmpmargin, 100, 400, 400));
	    }
	}
    }
}

  
void Menu::swipe()
{
  int margin = -index * 800;
  for (int i = 0 ; i < nbElements ; ++i)
    {
      CvRect rTmp = tileList[i]->getPos();
      CvRect sTmp = tileList[index]->getPos();
      
      rTmp.x = 200 + endGestureX - beginGestureX + margin;
      if (i == index)
	{
	  tileList[index]->setTilePos(rTmp);
	  if (tileList[index]->getPos().x < 50)
	    {
	      ++index;
	      if (index >= nbElements)
		index = nbElements - 1;
	      doSwipe();
	      break;
	    }
	  else if (tileList[index]->getPos().x + tileList[index]->getPos().width > 750)
	    {
	      --index;
	      if (index < 0)
		index = 0;
	      doSwipe();
	      break;
	    }
	}
      else
	tileList[i]->setTilePos(rTmp);
      margin += 800;
    }
}

void Menu::select()
{
  std::cout << "select" << std::endl;
}

void Menu::drawMenu(IplImage *imgToDraw)
{
  cvLine(imgToDraw, cvPoint(LIMITNEXT, 0), cvPoint(LIMITNEXT, 600), CV_RGB(255, 0, 0), 4);
  cvLine(imgToDraw, cvPoint(LIMITPREV, 0), cvPoint(LIMITPREV, 600), CV_RGB(255, 0, 0), 4);
  std::cout << index << std::endl;
  for (int i = 0 ; i < nbElements ; ++i)
    {
      CvRect rTmp = tileList[i]->getPos();
      cvRectangle(imgToDraw, cvPoint(rTmp.x, rTmp.y), cvPoint(rTmp.x + rTmp.width, rTmp.y + rTmp.height), tileList[i]->color, 5);
    }
}
