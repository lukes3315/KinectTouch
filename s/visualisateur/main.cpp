#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

#include "paint.hpp"

int main(int ac, char **av)
{
	sf::RenderWindow App(sf::VideoMode(800, 600), "MAIN");

	App.Draw(sf::Sprite(sf::Image(800, 600, sf::Color(255, 255, 255)), sf::Vector2f(0, 0)));
	sf::Image i;
	i.LoadFromFile("./paint.jpg");
	sf::Sprite *p = new sf::Sprite(i);
	p->Resize(150, 150);
	p->SetX(100);
	p->SetY(100);
	App.Draw(*p);
	i.LoadFromFile("./pong.jpg");

	while (1)
	{
		App.Display();
	}
}