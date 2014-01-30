#include "Menu.hpp"

Menu::Menu()
{
  
}

Menu::~Menu()
{
  
}

Menu::Menu(int nbElements):nbTiles(nbElements)
{
  int x = 20;
  int y = 20;
  for (int i = 0; i < nbElements ; ++i)
    {
      Tile * t = new Tile();
      
      t->setPosition(x, y);
      t->setSize(100 + x, 100 + y);
      t->setColor(CV_RGB(255, 0, 0));
      t->touched = false;
      tileList.push_back(t);
      if (x > 780)
	{
	  x = 20;
	  y += 200;
	}
      else
	x += 200;
    }
}

void Menu::checkTouched(int x, int y)
{
  for (int i = 0 ; i < tileList.size() ; ++i)
    {
      tileList[i]->touched = false;
      if (x > tileList[i]->rectangle.x && y > tileList[i]->rectangle.y 
	  && y < tileList[i]->rectangle.height && x < tileList[i]->rectangle.width)
	{
	  tileList[i]->touched = true;
	}
    }
}

void Menu::drawEstimation(IplImage * img)
{
  for (int i = 0 ; i < tileList.size() ; ++i)
    {
      if (tileList[i]->touched == false)
	{
	  cvRectangle(img, 
		      cvPoint(tileList[i]->rectangle.x, tileList[i]->rectangle.y),
		      cvPoint(tileList[i]->rectangle.width, tileList[i]->rectangle.height), 
		      tileList[i]->color, 8);
	}
      else
	{  
	  cvRectangle(img, 
		      cvPoint(tileList[i]->rectangle.x, tileList[i]->rectangle.y),
		      cvPoint(tileList[i]->rectangle.width, tileList[i]->rectangle.height), 
		      tileList[i]->color, -1);
	}
    }
}
