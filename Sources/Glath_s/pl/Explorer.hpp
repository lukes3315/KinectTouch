/*
** Explorer Class
*/

#ifndef EXPLORER_HPP__
#define EXPLORER_HPP__

#include <string>
#include <iostream>
#include <list>
#include <sys/types.h>
#include <dirent.h>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Folder.hpp"

#define WIDTH 800
#define HEIGHT 600

struct s_name
{
  int _id;
  std::string _n;
};

class Explorer
{
private:
  //  sf::RenderWindow _App(sf::VideoMode(800, 600, 32), "SFML Graphics");
  sf::RenderWindow *_App;
  sf::Event _Event;
  std::string _path;
  sf::Vector2f _V;
  std::string _currentFolder;
  std::list<Folder*> _Folders;
  sf::Image _Image;
  sf::Sprite _Sprite;
  std::list<sf::Sprite*> _s;
  std::list<s_name> _n;
  DIR *_dd;
  struct dirent *_dir;
  int _xF;
  int _yF;
public:
  Explorer();
  Explorer(const std::string &path);
  bool initSfml();
  bool changeDir(const std::string &path);
  std::string getPath() const;
  std::list<Folder*> getFolders() const;
  bool WindowOpened();
  void Draw();
  void getEvent();
  void displayPixel(int x, int y);
};

#endif
