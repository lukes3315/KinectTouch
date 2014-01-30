#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Explorer.hpp"
#include "Folder.hpp"
#include "MyFreenectDevice.hpp"

bool InFolder(int c[2], sf::Vector2f F, int SizeX, int SizeY)
{
  if (c[0] >= F.x && c[0] <= F.x + SizeX)
    if (c[1] >= F.y && c[1] <= F.y + SizeY)
      return true;
  return false;
}

int main()
{

  Explorer E;
  Freenect::Freenect freenect;
  MyFreenectDevice* device;

  device = &freenect.createDevice<MyFreenectDevice>(0);
  device->Initialize();
  E.initSfml();
  std::list<coords*> fList;
  while (E.WindowOpened())
    {
      E.getEvent();
      device->affCallback();
      fList = device->fingerList;
      std::list<coords*>::iterator it = fList.begin();

      if (fList.size() > 0)
	{
	  //	  std::cout <<" X = " << (*it)->x << " Y =  "<< (*it)->y << std::endl;
	  float x, y;
	  while (it != fList.end())
	    {
	      x = WIDTH * (*it)->x / 213;
	      y = HEIGHT * (*it)->y / 160;
	      E.displayPixel(x, y);
	      ++it;
	    }
	  // std::cout << "New coord : [" << x << ", " << y << "]\n";
	}
      device->fingerList.clear();
      E.Draw();
    }

  // sf::RenderWindow App(sf::VideoMode(800, 600, 32), "SFML Graphics");
  // sf::Image Image;
  // if (!Image.LoadFromFile("folder-icon.bmp"))
  //   std::cout << "Cannot open file" << std::endl;
  // sf::Sprite Sprite(Image);
  int c[2];
  bool Pressed = false;
  // sf::Event Event;
  // sf::Vector2f V;      

  // while (App.IsOpened())
  //   {


  //     const sf::Input& Input = App.GetInput();
  //     if (App.GetEvent(Event))
  // 	{
  // 	  if (Event.Type == sf::Event::MouseButtonPressed)
  // 	    {
  // 	      V = Sprite.GetPosition();
  // 	      if (InFolder(c, V, Sprite.GetSize().x, Sprite.GetSize().y))
  // 		std::cout << "In the folder" << std::endl;
  // 	      Pressed = true;
  // 	      // std::cout << "souris presser en [" << V.x << ", " << V.y << "]" << std::endl;
  // 	    }
  // 	  if (Event.Type == sf::Event::MouseButtonReleased)
  // 	    {
  // 	      std::cout << "Button released" << std::endl;
  // 	      Pressed = false;
  // 	    }
  // 	  else if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Escape)
  // 	    break ;
  // 	  if (Pressed == true)
  // 	    {
  // 	      c[0] = Input.GetMouseX();
  // 	      c[1] = Input.GetMouseY();
  // 	      Sprite.Move(c[0] -Sprite.GetPosition().x, 0);
  // 	      Sprite.Move(0, c[1] -Sprite.GetPosition().y);
  // 	    }
  // 	}
  //     App.Clear();
  //     App.Draw(Sprite);
  //     App.Display();
  //   }

  // App.Close();
  // return EXIT_SUCCESS;
}
