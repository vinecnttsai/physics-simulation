#include <iostream>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;
consst int pixel=1600,wid,height,particle_amount=wid*height;
RenderWindow window(VideoMode(pixel, pixel), "Fluid Simulation");
CircleShape circle;
class particle
{
    vector<double> pos,F,mass,velocity;
};
particle p[particle_amount];
void spring_mass_model()
{
    for(int i=0;i<particle_amount;i++)
    {
        if(!i||i==wid-1||i==particle_amount-1||i/wid==height-1)
        else if(!i%(wid+1)||!i%(wid))
        else if()
        
    }
}
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
