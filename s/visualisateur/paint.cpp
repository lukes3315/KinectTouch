#include "paint.hpp"

#define ABS(x) (x < 0 ? x * -1 : x)

Color::Color(sf::Color c, int x, int y)
{
	_c = c;
	_x = x;
	_y = y;
}

Paint::Paint()
{
	
}

void Paint::init()
{
	_App = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "PAINT");
	_colors.resize(8);
	_colors[0] = new Color(sf::Color(255, 0, 0), 10, 10);
	_colors[1] = new Color(sf::Color(255, 255, 0), 10, 50);
	_colors[2] = new Color(sf::Color(0, 0, 0), 50, 10);
	_colors[3] = new Color(sf::Color(0, 255, 255), 50, 50);
	_colors[4] = new Color(sf::Color(0, 0, 255), 90, 10);
	_colors[5] = new Color(sf::Color(120, 212, 16), 90, 50);
	_currentColor = sf::Color(0, 0, 0);
	_pressed = false;
	_type = PAINT;
}

static void drawBrush(sf::RenderWindow *App, std::vector<Color *> c)
{
	//sf::Image i(10, 10, c[0]->_c);
	//sf::Sprite s(i, sf::Vector2f(c[0]->_x, c[0]->_y), sf::Vector2f(2, 2));
	
	App->Draw(sf::Sprite(sf::Image(800, 600, sf::Color(255, 255, 255))));
	App->Draw(sf::Sprite(sf::Image(30, 30, sf::Color(200, 200, 200)), sf::Vector2f(0, 0), sf::Vector2f(0.3, 2.9)));
	App->Draw(sf::Sprite(sf::Image(30, 30, sf::Color(200, 200, 200)), sf::Vector2f(40, 0), sf::Vector2f(0.3, 2.9)));
	App->Draw(sf::Sprite(sf::Image(30, 30, sf::Color(200, 200, 200)), sf::Vector2f(80, 0), sf::Vector2f(0.3, 2.9)));
	App->Draw(sf::Sprite(sf::Image(30, 30, sf::Color(200, 200, 200)), sf::Vector2f(120, 0), sf::Vector2f(0.3, 2.9)));
	App->Draw(sf::Sprite(sf::Image(30, 30, sf::Color(200, 200, 200)), sf::Vector2f(0, 0), sf::Vector2f(4.2, 0.3)));
	App->Draw(sf::Sprite(sf::Image(30, 30, sf::Color(200, 200, 200)), sf::Vector2f(0, 40), sf::Vector2f(4.2, 0.3)));
	App->Draw(sf::Sprite(sf::Image(30, 30, sf::Color(200, 200, 200)), sf::Vector2f(0, 80), sf::Vector2f(4.2, 0.3)));
	App->Draw(sf::Sprite(sf::Image(30, 30, c[0]->_c), sf::Vector2f(c[0]->_x, c[0]->_y)));
	App->Draw(sf::Sprite(sf::Image(30, 30, c[1]->_c), sf::Vector2f(c[1]->_x, c[1]->_y)));
	App->Draw(sf::Sprite(sf::Image(30, 30, c[2]->_c), sf::Vector2f(c[2]->_x, c[2]->_y)));
	App->Draw(sf::Sprite(sf::Image(30, 30, c[3]->_c), sf::Vector2f(c[3]->_x, c[3]->_y)));
	App->Draw(sf::Sprite(sf::Image(30, 30, c[4]->_c), sf::Vector2f(c[4]->_x, c[4]->_y)));
	App->Draw(sf::Sprite(sf::Image(30, 30, c[5]->_c), sf::Vector2f(c[5]->_x, c[5]->_y)));
	sf::Image i;
	i.LoadFromFile("./pot - Copie.jpg");
	sf::Sprite *s = new sf::Sprite(i);
	s->SetX(150);
	s->SetY(10);
	App->Draw(*s);
	i.LoadFromFile("./carre.jpg");
	s = new sf::Sprite(i);
	s->SetY(10);
	s->SetX(220);
	App->Draw(*s);
	i.LoadFromFile("./cercle.jpg");
	s = new sf::Sprite(i);
	s->SetY(10);
	s->SetX(350);
	App->Draw(*s);
	/*s.SetImage(i);
	s.SetX(220);
	s.SetY(10);
	App->Draw(s);*/
	//i(10, 10, c[0]->_c);
	//s(i, sf::Vector2f(c[0]->_x, c[0]->_y), sf::Vector2f(2, 2));
}

static sf::Color checkColor(int x, int y, std::vector<Color *> _colors, sf::Color _currentColor, sf::RenderWindow *_App)
{
	if (x >= 150 && x <= 210)
	{
		if (y >= 10 && y <= 73)
		{
			
			_App->Draw(sf::Sprite(sf::Image(800, 600, _currentColor), sf::Vector2f(0, 90)));
			return _currentColor;
		}
	}
	if (x >= _colors[0]->_x && x <= _colors[0]->_x + 40)
	{
		if (y >= _colors[0]->_y && y <= _colors[0]->_y + 40)
			return  _colors[0]->_c;
	}
	if (x >= _colors[1]->_x && x <= _colors[1]->_x + 40)
	{
		if (y >= _colors[1]->_y && y <= _colors[1]->_y + 40)
			return  _colors[1]->_c;
	}
	if (x >= _colors[2]->_x && x <= _colors[2]->_x + 40)
	{
		if (y >= _colors[2]->_y && y <= _colors[2]->_y + 40)
			return  _colors[2]->_c;
	}
	if (x >= _colors[3]->_x && x <= _colors[3]->_x + 40)
	{
		if (y >= _colors[3]->_y && y <= _colors[3]->_y + 40)
			return  _colors[3]->_c;
	}
	if (x >= _colors[4]->_x && x <= _colors[4]->_x + 40)
	{
		if (y >= _colors[4]->_y && y <= _colors[4]->_y + 40)
			return  _colors[4]->_c;
	}
	if (x >= _colors[5]->_x && x <= _colors[5]->_x + 40)
	{
		if (y >= _colors[5]->_y && y <= _colors[5]->_y + 40)
			return  _colors[5]->_c;
	}
	return _currentColor;
}

void Paint::loop()
{
	sf::Event e;
	int t[2];
	t[0] = t[1] = -1;
	drawBrush(_App, _colors);
	while (1)
	{
		if (_App->GetEvent(e))
		{
			if (e.Type == sf::Event::MouseButtonPressed || _pressed == true)
			{
				const sf::Input &input = _App->GetInput();
				int x = input.GetMouseX();
				int y = input.GetMouseY();
				
				if (y > 100)
				{
					if (_type == PAINT)
						_App->Draw(sf::Sprite(sf::Image(5, 5, _currentColor), sf::Vector2f(x, y)));
					else if ((_type == CARRE || _type == CERCLE) && _pressed == false)
					{
						t[0] = x;
						t[1] = y;					
					}
					_pressed = true;
				}
				else
				{
					if (x >= 220 && x <= 333)
					{
						if (y >= 10 && y <= 65)
						{
							_type = CARRE;
						}
					}
					else if (x >= 350 && 429)
					{
						if (y >= 10 && y <= 65)
							_type = CERCLE;
					}
						else
						_currentColor = checkColor(x, y, _colors, _currentColor, _App);
					_pressed = false;
				}
			}
			if (e.Type == sf::Event::MouseButtonReleased)
			{
				
				if ((_type == CARRE || _type == CERCLE) && _pressed == true)
				{
				const sf::Input &input = _App->GetInput();
				int x = input.GetMouseX();
				int y = input.GetMouseY();
				if (_type == CARRE)
				{
						sf::Shape p(sf::Shape::Rectangle(t[0], t[1], x, y, _currentColor));
						_App->Draw(p);
				}
				else
				{
					int r = ABS((t[1] - y));
					if (t[1] - r > 100)
					{
						sf::Shape p(sf::Shape::Circle(t[0], t[1], (t[1] - y), _currentColor));
						_App->Draw(p);
					}
				}
					t[0] = t[1] = -1;
					_type = PAINT;
				}
				_pressed = false;
			}
		}
		
		_App->Display();
	}

	
}