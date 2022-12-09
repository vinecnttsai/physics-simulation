#include <iostream>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <SFML/Graphics.hpp>
#define pi 3.1416
using namespace std;
using namespace sf;
const int pixel=1500,b_up=pixel,b_bottom=0,b_left=0,b_right=pixel,radius=15,particlerange=pixel/2,particle_amount=625;
const double mu=0.5,H=80.0,damp=-0.9,g=-9.8,K=20.0,rho0=1.0;
RenderWindow window(VideoMode(pixel, pixel), "Fluid Simulation");
CircleShape circle;
class value
{
    public:
        value();
        double pos,rho,p,pressure,a,v,velocity;
};
value::value()
{
    pressure=0.0;
    rho=0.0;
    v=0.0;
    a=0.0;
    velocity=0.0;
}
class particle
{
    public:
        void cin_pos(double x,double y);
        void c_rho();
        void reset();
        double mass,h;
        value v[2];
        bool in_range[particle_amount];
};
void particle::cin_pos(double x,double y)
{
    h=H;
    mass=0.1;
    v[0].pos=x;
    v[1].pos=y;
}
void particle::reset()
{
    for(int i=0;i<particle_amount;i++)in_range[i]=false;
}
particle group[particle_amount];
void draw()
{
    window.clear(Color::White);
    for(int i=0;i<particle_amount;i++)
    {
        for(int j=0;j<2;j++)
        {
            circle.setPosition(group[i].v[0].pos+2*radius,1500-group[i].v[1].pos-2*radius);
            window.draw(circle);
        }
    }
    window.display();
}
void range()
{
    for(int i=0;i<particle_amount;i++)
    {
        for(int k=0;k<particle_amount;k++)
        {
            if(i!=k)
            {
                particle temp=group[i],temp2=group[k];
                if(sqrt(pow(temp.v[0].pos-temp2.v[0].pos,2)+pow(temp.v[1].pos-temp2.v[1].pos,2))<H)
                {
                    group[k].in_range[i]=true;
                }
            }
        }
    }
}
void border()
{
    for(int i=0;i<particle_amount;i++)
    {
        if(group[i].v[1].pos+radius>=b_up)
        {
            group[i].v[1].v*=damp;
            group[i].v[1].pos=pixel;
        }
        if(group[i].v[1].pos-radius<=b_bottom)
        {
            group[i].v[1].v*=damp;
            group[i].v[1].pos=0;
        }
        if(group[i].v[0].pos-radius<=b_left)
        {
            group[i].v[0].v*=damp;
            group[i].v[0].pos=0;
        }
        if(group[i].v[0].pos+radius>=b_right)
        {
            group[i].v[0].v*=damp;
            group[i].v[0].pos=pixel;
        }
    }
}
void caculate()
{
    range();
    
    for(int i=0;i<particle_amount;i++)
    {
        for(int k=0;k<particle_amount;k++)
        {
            if(!group[k].in_range[i]&&i!=k)
            {
                for(int j=0;j<2;j++)
                {
                    value& temp=group[i].v[j],temp2=group[k].v[j];
                    temp.rho+=pow(pow(group[i].h,2)-pow(temp.pos-temp2.pos,2),3);
                }
            }
        }
        for(int j=0;j<2;j++)
        {
            value& temp=group[i].v[j];
            temp.rho*=group[i].mass*315.0/(64.0*pi*pow(group[i].h,9));
            temp.p=K*(temp.rho-rho0);
            cout<<temp.rho<<" "<<temp.p<<endl;
        }
    }//密度
    
    for(int i=0;i<particle_amount;i++)
    {
        for(int k=0;k<particle_amount;k++)
        {
            if(group[k].in_range[i]&&i!=k)
            {
                for(int j=0;j<2;j++)
                {
                    value& temp=group[i].v[j],temp2=group[k].v[j];
                    double r=abs(temp.pos-temp2.pos);
                    //temp.pressure+=((temp.p+temp2.p)/(2*temp.rho*temp2.rho))*pow(group[i].h-r,2)*((temp.pos-temp2.pos)/r);
                    //cout<<((temp.p+temp2.p)/(2*temp.rho*temp2.rho))*pow(group[i].h-r,2)*((temp.pos-temp2.pos)/r)<<endl;
                }
            }
        }
        for(int j=0;j<2;j++)
        {
            group[i].v[j].pressure*=group[i].mass*45.0/(pi*pow(group[i].h,6));
        }
    }//壓力
    
    for(int i=0;i<particle_amount;i++)
    {
        for(int k=0;k<particle_amount;k++)
        {
            if(group[k].in_range[i]&&i!=k)
            {
                for(int j=0;j<2;j++)
                {
                    value& temp=group[i].v[j],temp2=group[k].v[j];
                    double r=abs(temp.pos-temp2.pos);
                    temp.velocity+=(temp.v-temp2.v)*(group[i].h-r)/(temp.rho*temp2.rho);
                }
            }
        }
        for(int j=0;j<2;j++)
        {
            group[i].v[j].velocity*=group[i].mass*mu*45.0/(pi*pow(group[i].h,6));
        }
    }//黏度
    
    for(int i=0;i<particle_amount;i++)
    {
        for(int j=0;j<2;j++)
        {
            value& temp=group[i].v[j];
            temp.a=temp.pressure+temp.velocity;
            if(j)temp.a+=g;
            temp.v+=temp.a;
            temp.pos+=temp.v;
        }
        //cout<<group[i].v[0].a<<" "<<group[i].v[1].a<<endl;
    }
    
    border();
    
    draw();
    
    for(int i=0;i<particle_amount;i++)group[i].reset();
}
int main()
{
    circle.setRadius(radius);
    circle.setFillColor(Color::Blue);
    for(int i=0;i<particle_amount;i++)
    {
        group[i].cin_pos(pixel/4.0+i%(particlerange/(2*radius))*radius*2, i/(particlerange/(2*radius))*radius*2);
    }
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
        usleep(100000);
        caculate();
    }
    return 0;
}
