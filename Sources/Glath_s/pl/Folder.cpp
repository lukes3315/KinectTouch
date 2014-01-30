/*
** Folder Class
*/

#include "Folder.hpp"

Folder::Folder(std::string name, int x, int y, int id)
{
  _name = name;
  _x = x;
  _y = y;
  id = _id;
}

std::string Folder::getName() const
{
  return _name;
}

int Folder::getX() const
{
  return _x;
}

int Folder::getId() const
{
  return _id;
}

int Folder::getY() const
{
  return _y;
}
