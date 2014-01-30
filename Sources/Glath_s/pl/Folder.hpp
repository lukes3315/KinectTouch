/*
** Folder Class
*/

#ifndef FOLDER_HPP__
#define FOLDER_HPP__

#include <string>
#include <iostream>

class Folder
{
private:
  std::string _name;
  int _x;
  int _y;
  int _id;
public:
  Folder(std::string name, int x, int y, int id);
  std::string getName() const;
  int getX() const;
  int getY() const;
  int getId() const;
};

#endif
