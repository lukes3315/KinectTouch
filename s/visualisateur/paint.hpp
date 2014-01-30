#ifndef MOVE_HPP__
#define MOVE_HPP__

#include <string>
#include <iostream>
#include <list>
#include <vector>

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"	

#define MARGIN 50

class Color
	{
	public:
		Color(sf::Color c, int x, int y);
		int _x;
		int _y;
		sf::Color _c;
	};

class Paint
{
	
private:
	enum Type
	{
		CARRE,
		CERCLE,
		PAINT
	};
	Type _type;
	sf::Color _currentColor;
	sf::RenderWindow *_App;
	
	std::vector<Color *> _colors;
	struct Fingers
	{	
		int x;
		int y;
	};
	std::list<Fingers> _fingers;
	Fingers _f;
	bool _pressed;
public:
	Paint();
	void init();
	void loop();
};

#endif