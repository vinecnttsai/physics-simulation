#include <iostream>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;
RenderWindow window(VideoMode(pixel+radius*2, pixel+radius*2), "Fluid Simulation");
CircleShape circle;
int main()
{
    circle.setRadius(radius);
    circle.setFillColor(Color::Blue);
    while (window.isOpen())
    {
        Event event;
        
        while (window.pollEvent(event))
        {
            if (event.type==Event::Closed)
            {
                window.close();
            }
        }
        usleep(0);
    }
    return 0;
}
