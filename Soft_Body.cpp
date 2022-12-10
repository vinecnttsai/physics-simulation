#include <iostream>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;
const int pixel=1600,wid=10,height=10,particle_amount=wid*height,ks,kd,l0,mask[9][8]={{0,0,0,1,1,1,0,0},{1,1,0,0,0,0,0,1},{0,0,0,0,0,1,1,1},{0,1,1,1,0,0,0,0},{1,1,1,0,0,0,0,0},{0,0,0,0,1,1,1,0},{0,0,1,1,1,0,0,0},{1,0,0,0,0,0,1,1},{1,1,1,1,1,1,1,1}},mask_pos[8][2]={{-1,-1},{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1}};
const double delta_t=0.1;
RenderWindow window(VideoMode(pixel, pixel), "Fluid Simulation");
CircleShape circle;
class particle
{
    public:
        double pos[2],F[2],mass,velocity[2];
    void update();
};
void particle::update()
{
    for(int j=0;j<2;j++)
    {
        velocity[j]+=F[j]/mass*delta_t;
        pos[j]+=velocity[j]*delta_t;
    }
}
particle p[particle_amount];
bool collision()
{
    
}
double hooke(double posa,double posb)
{
    return ks*(abs(posa-posb)-l0);
}
double dis(int A,int B)
{
    int posa=p[A].pos[0],posa2=p[A].pos[1];
    int posb=p[B].pos[0],posb2=p[B].pos[1];
    return sqrt(pow(posa-posb,2)+pow(posa2-posb2,2));
}
void spring_mass_model()
{
    for(int i=0;i<particle_amount;i++)
    {
        int mask_num;
        if(!i)mask_num=0;
        else if(i==particle_amount-1)mask_num=1;
        else if(i/wid==height-1)mask_num=2;
        else if(i==wid-1)mask_num=3;
        else if(!(i+1)%wid)mask_num=4;
        else if(!i%wid)mask_num=5;
        else if(!i/wid)mask_num=6;
        else if(i/wid==height-1)mask_num=7;
        else mask_num=8;
        for(int k=0;k<8;k++)
        {
            if(mask[mask_num][k])
            {
                int A=i,B=i+mask_pos[k][0]+mask_pos[k][1]*height;
                int damping,rx=p[B].pos[0]-p[A].pos[0],ry=p[B].pos[1]-p[A].pos[1],vx=p[B].velocity[0]-p[A].velocity[0],vy=p[B].velocity[1]-p[A].velocity[1];
                damping=kd*(rx*vx+ry*vy)/dis(A,B);
                for(int j=0;j<2;j++)
                {
                    p[i].F[j]=hooke(p[A].pos[j], p[B].pos[j])+damping;
                }
            }
        }
    }//F
    
    for(int i=0;i<particle_amount;i++)
    {
        if(collision())p[i].update();
        else
        {
            
        }
    }
}
int main()
{
    circle.setRadius();
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
