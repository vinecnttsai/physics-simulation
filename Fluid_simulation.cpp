#include <iostream>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <SFML/Graphics.hpp>
#define pi 3.1415926
using namespace std;
using namespace sf;
const int pixel=1500,b_up=pixel,b_bottom=0,b_left=0,b_right=pixel,particlerange=pixel/2,particle_amount=20*20;
const double mu=0.0,damp=-0.5,g=-10,K=1.0,rho0=10,radius=10.0,m=pi*pow(radius,2)*rho0,H=3*radius,initial_dis=sqrt(pi*pow(radius,2)),delta_t=0.001;
const double normalize=(45.0*m)/(pi*pow(H,6));
RenderWindow window(VideoMode(pixel+radius*2, pixel+radius*2), "Fluid Simulation");
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
            circle.setPosition(group[i].v[0].pos,1500-group[i].v[1].pos);
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
                if(abs(temp.v[0].pos-temp2.v[0].pos)<=H&&abs(temp.v[1].pos-temp2.v[1].pos)<=H)
                {
                    if(pow(temp.v[0].pos-temp2.v[0].pos,2)+pow(temp.v[1].pos-temp2.v[1].pos,2)<=pow(H,2))
                    {
                        group[k].in_range[i]=true;
                    }
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
double dis(int a,int b)
{
    return sqrt(pow(group[a].v[0].pos-group[b].v[0].pos,2)+pow(group[a].v[1].pos-group[b].v[1].pos,2));
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
                    temp.rho+=pow((pow(H,2)-pow(temp.pos-temp2.pos,2)),3);
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
                    double r=dis(i,k);
                    temp.pressure+=((temp.p+temp2.p)/(2*temp.rho*temp2.rho))*pow(H-r,2)*(temp.pos-temp2.pos);
                    if(r)temp.pressure/=r;
                    //else temp.pressure=0;
                    //if(temp.rho*temp2.rho)temp.pressure/=2*temp.rho*temp2.rho;
                    //cout<<temp.pressure<<endl;
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
                    double r=dis(i,k);
                    temp.velocity+=(temp2.v-temp.v)*(H-r);
                    if(temp.rho*temp2.rho)temp.velocity/=temp.rho*temp2.rho;
                    //else temp.velocity=0;
                }
            }
        }
        for(int j=0;j<2;j++)
        {
            group[i].v[j].velocity*=normalize*mu;
            //cout<<group[i].v[j].velocity<<endl;
        }
    }//黏度
    
    for(int i=0;i<particle_amount;i++)
    {
        for(int j=0;j<2;j++)
        {
            value& temp=group[i].v[j];
            temp.a=temp.pressure+temp.velocity;
            if(j)temp.a+=g*m;
            temp.v+=temp.a;
            temp.pos+=temp.v*delta_t;
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
        group[i].cin_pos(pixel/4.0+i%20*initial_dis, i/20*initial_dis);
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
        usleep(10000);
        caculate();
    }
    return 0;
}
