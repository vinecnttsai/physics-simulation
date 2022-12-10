#include <iostream>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <SFML/Graphics.hpp>
#define pi 3.1415926
using namespace std;
using namespace sf;
const int pixel=1500,b_up=pixel,b_bottom=0,b_left=0,b_right=pixel,particlerange=pixel/2,particle_amount=20*20;
const double mu=0.5,damp=-0.5,g=-9.8,K=1.0,rho0=1.0,radius=5.0,m=radius*0.15,H=2.5*radius;
const double normalize=(45.0*m)/(pi*pow(H,6));
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
        value v[2];
        bool in_range[particle_amount];
};
void particle::cin_pos(double x,double y)
{
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
            if(group[k].in_range[i]&&i!=k)
            {
                for(int j=0;j<2;j++)
                {
                    value& temp=group[i].v[j],temp2=group[k].v[j];
                    temp.rho+=pow((pow(H,2)-abs(temp.pos-temp2.pos)),3);
                }
            }
        }
        for(int j=0;j<2;j++)
        {
            value& temp=group[i].v[j];
            temp.rho*=(m*315.0)/(64.0*pi*pow(H,9));
            temp.p=K*(temp.rho-rho0);
            //cout<<temp.rho<<" "<<temp.p<<" "<<normalize<<endl;
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
                    temp.pressure+=((temp.p+temp2.p)/(2*temp.rho*temp2.rho))*pow(H,2)*(temp.pos-temp2.pos);
                    if(r)temp.pressure/=r;
                    //if(temp.rho*temp2.rho)temp.pressure/=2*temp.rho*temp2.rho;
                    //else temp.pressure=0;
                    cout<<temp.pressure<<endl;
                }
            }
        }
        for(int j=0;j<2;j++)
        {
            group[i].v[j].pressure*=normalize;
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
                    temp.velocity+=(temp.v-temp2.v)*(H-r)/(temp.rho*temp2.rho);
                }
            }
        }
        for(int j=0;j<2;j++)
        {
            group[i].v[j].velocity*=normalize;
            //cout<<group[i].v[j].velocity<<endl;
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
    //cout<<normalize<<endl;
    for(int i=0;i<particle_amount;i++)
    {
        //group[i].cin_pos(pixel/4.0+i%(particlerange/(2*radius))*radius*2, i/(particlerange/(2*radius))*radius*2);
        group[i].cin_pos(pixel/4.0+i%20*radius*2, i/20*radius*2);
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
        usleep(0);
        caculate();
    }
    return 0;
}
