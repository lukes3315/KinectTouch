/*
** Explorer Class
*/

#include "Explorer.hpp"

Explorer::Explorer()
{
  int id;

  _dd = opendir(".");
  id = 0;
  _dir = readdir(_dd);
  _xF = _yF = 0;
  while (_dir)
    {
      // std::cout << "TEST" << std::endl;
      //      std::cout << _dir->d_name << " - " << _xF << ", " << _yF << std::endl;
      _Folders.push_back(new Folder(_dir->d_name, _xF, _yF, id));
      if (_xF <= WIDTH - 70)
	_xF += 70;
      else
	{
	  _yF += 70;
	  _xF = 0;
	}
      _dir = readdir(_dd);
      id += 1;
    }
  _xF = 0;
  _yF = 0;
}

Explorer::Explorer(const std::string &path)
{
  int id;

  _path = path;
  _dd = opendir(path.c_str());
  _dir == readdir(_dd);
  id = 0;
  while (_dir)
    {
      _Folders.push_back(new Folder(_dir->d_name, _xF, _yF, id));
      _xF += 50;
      id += 1;
      _yF += 50;
    }
  _xF = 0;
  _yF = 0;
}

bool Explorer::changeDir(const std::string &path)
{
  int id;

  _Folders.clear();
  _dd = opendir(path.c_str());
  _dir == readdir(_dd);
  id = 0;
  if (_dir)
    {
      while (_dir)
	{
	  _Folders.push_back(new Folder(_dir->d_name, _xF, _yF, id));
	  _xF += 50;
	  _yF += 50;
	}
      _xF = 0;
      _yF = 0;
      return true;
    }
  return false;
}

std::list<Folder*> Explorer::getFolders() const
{
  return _Folders;
}

std::string Explorer::getPath() const
{
  return _path;
}

bool Explorer::WindowOpened()
{
  if (_App->IsOpened())
    return true;
  return false;
}

bool Explorer::initSfml()
{

  s_name t;

  if (!_Image.LoadFromFile("folder-icon.bmp"))
    return false;
  _Sprite.SetImage(_Image);
  _App = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "Explorer");
  for (std::list<Folder*>::iterator it = _Folders.begin();
       it != _Folders.end(); ++it)
    {
      sf::Sprite *tmp = new sf::Sprite(_Image);
      tmp->Move((*it)->getX(), (*it)->getY());
      _s.push_back(tmp);      
      t._id = (*it)->getId();
      t._n = (*it)->getName();
      _n.push_back(t);
    }
  _App->Clear();
}

void Explorer::Draw()
{
  _App->Clear();
  for (std::list<sf::Sprite*>::iterator it = _s.begin(); it != _s.end(); ++it)
    {
      _App->Draw(*(*it));
      // sf::Vector2f V;

      // V = (*(*it)).GetPosition();
      // std::cout << V.x << ", " << V.y << std::endl;
      // std::cout << "|" << std::endl;
    }
  sf::Font font;

  font.LoadFromFile("arial.ttf", 50);
  sf::String Text;
  int tmp2;

  Text.SetFont(font);
  Text.SetSize(15);
  Text.SetColor(sf::Color(255, 255, 255));

  std::list<Folder*>::iterator it2 = _Folders.begin();
  tmp2 = (*it2)->getY() + 52;
  Text.Move(0, (*it2)->getY() + 52);
  for (std::list<s_name>::iterator it = _n.begin(); it != _n.end(); ++it)
    {
      int tmp = 0;
      if ((*it)._n.size() > 6)
	(*it)._n.resize(5);
      //      std::cout << "y : " << (*it2)->getY() + 52 << std::endl;
      //      std::cout << (*it2)->getY() << " et " << tmp2 - 52 << std::endl;
      // if ((*it2)->getY() != tmp2 - 52)
      // 	{
      // 	  Text.(-tmp, (*it2)->getY() + 52);
      // 	  tmp2 = (*it2)->getY() + 52;
      // 	  tmp = 0;
      // 	}
      Text.SetText((*it)._n.c_str());
      _App->Draw(Text);
      Text.SetPosition((*it2)->getX(), (*it2)->getY() + 52);
      ++it2;
    }
  _App->Display();
  //  sleep(10);
}

void Explorer::displayPixel(int x, int y)
{
  sf::Shape Rect   = sf::Shape::Circle(x, y, 1, sf::Color(255, 0, 0));
  //  std::cout << x << ", " << y << std::endl;
  _App->Draw(Rect);
  _App->Display();
}

void Explorer::getEvent()
{
  const sf::Input& Input = _App->GetInput();

  if (_App->GetEvent(_Event))
    {
      if (_Event.Type == sf::Event::MouseButtonPressed)
	{
	  int c[2];
	  c[0] = Input.GetMouseX();
	  c[1] = Input.GetMouseY();
	  //	  std::cout << "souris : [" << c[0] << ", " << c[1] << "]" << std::endl;
	}
    }
}
