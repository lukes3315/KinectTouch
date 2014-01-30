#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

void zoom(sf::Image &image, sf::RenderWindow &App, sf::Sprite &sprite)
{
	
}

int visualiser()
{
	sf::RenderWindow App(sf::VideoMode(800, 600, 32), "Visualiseur d'images");
	int first[2];
	int second[2];
	first[0] = first[1] = second[0] = second[1] = -1;
	sf::Image image;
	image.LoadFromFile("C:\\Users\\Public\\Pictures\\Sample Pictures\\Koala.jpg");
	sf::Sprite sprite;
	sf::View view;
	sf::View base = App.GetDefaultView();
	//view = App.GetView();
	view.SetFromRect(sf::FloatRect(0, 0, 400, 400));
	sprite.SetImage(image);	
	sprite.Resize(800, 600);
	//App.SetView(view);
	//view.Zoom(0.5f);
	sf::Event e;
		App.Draw(sprite);
	while (1)
	{	

		if (App.GetEvent(e))
		{
			if (e.Type == sf::Event::KeyPressed && e.Key.Code == sf::Key::A)
			{
				const sf::Input& Input = App.GetInput();
				if (first[0] == -1)
				{
					first[0] = Input.GetMouseX();
					first[1] = Input.GetMouseY();
				}
				else
				{
					second[0] = Input.GetMouseX();
					second[1] = Input.GetMouseX();
				}
				if (first[0] != -1 && second[0] != -1)
				{
					view.SetFromRect(sf::FloatRect(first[0], first[1], second[0], second[1]));
					App.SetView(view);
				}
			}
			else if (e.Type == sf::Event::KeyPressed && e.Key.Code == sf::Key::B)
			{
				App.SetView(base);
				first[0] = second[0] = -1;

			}
		}
		App.Draw(sprite);
		App.Display();
	}
	return 0;
}