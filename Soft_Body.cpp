#include <iostream>
#include <math.h>
#include <time.h>
#include <vector>
#include <unistd.h>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;
const int pixel=1500,wid=10,height=10,particle_amount=wid*height,ks,kd,l0,mask[9][8]={{0,0,0,1,1,1,0,0},{1,1,0,0,0,0,0,1},{0,0,0,0,0,1,1,1},{0,1,1,1,0,0,0,0},{1,1,1,0,0,0,0,0},{0,0,0,0,1,1,1,0},{0,0,1,1,1,0,0,0},{1,0,0,0,0,0,1,1},{1,1,1,1,1,1,1,1}},mask_pos[8][2]={{-1,-1},{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1}},ray_start_point[2]={pixel/2,pixel/2},ob_num=4,radius;
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
int line[ob_num][4][3];
void draw(int point)
{
    circle.setPosition(p[point].pos[0]*10,pixel-p[point].pos[1]*10);
    window.draw(circle);
}
double dis(int A,int B)
{
    int posa=p[A].pos[0],posa2=p[A].pos[1];
    int posb=p[B].pos[0],posb2=p[B].pos[1];
    return sqrt(pow(posa-posb,2)+pow(posa2-posb2,2));
}
void change_velocity(int point,int normal_a,int normal_b)
{
    double dot=normal_a*p[point].velocity[0]+normal_b*p[point].velocity[1];
    
    p[point].velocity[0]-=2*normal_a*dot;
    p[point].velocity[1]-=2*normal_b*dot;
}
void self_collision(int point)
{
    for(int i=0;i<particle_amount;i++)
    {
        if(i!=point&&dis(i, point)<2*radius)
        {
            change_velocity(i,p[i].pos[0]-p[point].pos[0],-1*(p[i].pos[1]-p[point].pos[1]));
        }
    }
}
int collision(int point)
{
    int count_ob[ob_num+1],posa=ray_start_point[0],posb=ray_start_point[1];
    memset(count_ob,0,sizeof(count_ob));
    double x1=ray_start_point[0],x2=p[point].pos[0],y1=ray_start_point[1],y2=p[point].pos[1];
    double l_x=y1-y2,l_y=-1*(x1-x2),l_z=x2*y1-x1*y2;
    
    for(int i=0;i<ob_num;i++)
    {
        for(int k=0;k<4;k++)
        {
            double delta,deltax,deltay,ansa,ansb;
            delta=line[i][k][0]*l_y-line[i][k][1]*l_x;
            deltax=line[i][k][2]*l_y-line[i][k][1]*l_z;
            deltay=line[i][k][0]*l_z-line[i][k][2]*l_x;
            ansa=deltax/delta;
            ansb=deltay/delta;
            
            if((abs(posa-ansa)<abs(posa-p[point].pos[0]))&&(abs(posb-ansb)<abs(posb-p[point].pos[1])))count_ob[i]++;
        }
    }
    
    
    for(int i=1;i<=ob_num;i++)
    {
        if(!count_ob[i]%2)return i;
    }
    return 0;
}
double hooke(double posa,double posb)
{
    return ks*(abs(posa-posb)-l0);
}
void ob_line(int x1,int y1,int x2,int y2,int lebal,int line_num)
{
    line[lebal][line_num-1][0]=y1-y2;
    line[lebal][line_num-1][1]=-1*(x1-x2);
    line[lebal][line_num-1][2]=x2*y1-x1*y2;
    
    //法向量
}
void create_obstacle()
{
    ob_line(0,5,149,5,1,1);
    ob_line(0,0,149,0,1,2);
    ob_line(0,0,0,5,1,3);
    ob_line(149,0,149,5,1,4);
    
    ob_line(0,0,5,0,2,1);
    ob_line(0,0,0,149,2,2);
    ob_line(0,149,5,149,2,3);
    ob_line(5,0,5,149,2,4);
    
    ob_line(0,149,149,149,3,1);
    ob_line(0,149-5,0,149,3,2);
    ob_line(0,149-5,149,149-5,3,3);
    ob_line(149,149-5,149,149,3,4);
    
    ob_line(149-5,149,149,149,4,1);
    ob_line(149-5,0,149-5,149,4,2);
    ob_line(149-5,0,149,0,4,3);
    ob_line(149,0,149,149,4,4);
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
        int lebal=collision(i);
        self_collision(i);
        if(lebal>0)
        {
            double min=float(INT_MAX),distance_;
            int l;
            for(int k=0;k<4;k++)
            {
                distance_=(p[i].pos[0]*line[lebal][k][0]+p[i].pos[1]*line[lebal][k][1])/sqrt(pow(line[lebal][k][0],2)+pow(line[lebal][k][1],2));
                if(min>distance_)
                {
                    min=distance_;
                    l=k;
                }
            }
            change_velocity(i,line[lebal][l][0],line[lebal][l][1]);
        }
        p[i].update();
        draw(i);
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
        window.clear(Color::White);
        spring_mass_model();
        window.display();
    }
    return 0;
}
