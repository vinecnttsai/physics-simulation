#include <iostream>
#include <math.h>
#include <time.h>
#include <vector>
#include <unistd.h>
//#include <SFML/Graphics.hpp>
using namespace std;
//using namespace sf;
const int pixel=1500,wid=1,height=1,particle_amount=wid*height,ks=0.5,kd=0.5,mask[9][8]={{0,0,0,1,1,1,0,0},{1,1,0,0,0,0,0,1},{0,0,0,0,0,1,1,1},{0,1,1,1,0,0,0,0},{1,1,1,0,0,0,0,0},{0,0,0,0,1,1,1,0},{0,0,1,1,1,0,0,0},{1,0,0,0,0,0,1,1},{1,1,1,1,1,1,1,1}},mask_pos[8][2]={{-1,-1},{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1}},ray_start_point[2]={160/*改變x為最大值*/,163},ob_num=4,wid_wall=500;
const double delta_t=0.1,g=-9.8,radius=0.3,r=1.0,L01=1.0,L02=sqrt(2),d=0.001;
//RenderWindow window(VideoMode(pixel, pixel), "Fluid Simulation");
//CircleShape circle;
class particle
{
    public:
        particle();
        double pos[2],F[2],mass,velocity[2];
        void update();
};
particle::particle()
{
    velocity[0]=0;
    velocity[1]=0;
    mass=1;
}
void particle::update()
{
    for(int j=0;j<2;j++)
    {
        velocity[j]+=F[j]/mass*delta_t;
        pos[j]+=velocity[j]*delta_t;
    }
}
particle p[particle_amount];
int line[ob_num+1][4][7];
void draw(int point)
{
    //circle.setPosition(p[point].pos[0]*10,pixel-p[point].pos[1]*10);
    //window.draw(circle);
}
double dis(int A,int B)
{
    double posa=p[A].pos[0],posa2=p[A].pos[1];
    double posb=p[B].pos[0],posb2=p[B].pos[1];
    //cout<<posa<<" "<<posb<<" "<<posa2<<" "<<posb2<<endl;
    return sqrt(pow(posa-posb,2)+pow(posa2-posb2,2));
}
void change_velocity(int point,int normal_a,int normal_b,float length)
{
    double dot=normal_a*p[point].velocity[0]+normal_b*p[point].velocity[1];
    
    //cout<<p[point].velocity[1]<<endl;
    
    if(normal_a)p[point].velocity[0]-=2*dot/normal_a;
    if(normal_b)p[point].velocity[1]-=2*dot/normal_b;
    
    float r=length/(sqrt(pow(normal_a,2)+pow(normal_b,2))),unit_a,unit_b;
    unit_a=normal_a*r;
    unit_b=normal_b*r;
    p[point].pos[0]+=unit_a+d;
    p[point].pos[1]+=unit_b+d;
    
    //cout<<p[point].velocity[0]<<" "<<p[point].pos[0]<<" "<<p[point].pos[1]<<endl;
    //cout<<normal_a<<" "<<r<<endl;
    //改變位置
    
}
void self_collision(int point)
{
    for(int i=0;i<particle_amount;i++)
    {
        if(i!=point&&dis(i, point)<2*radius)
        {
            change_velocity(i,p[i].pos[0]-p[point].pos[0],-1*(p[i].pos[1]-p[point].pos[1]),radius-dis(i,point));
        }
    }
}
int collision(int point)
{
    int count_ob[ob_num+1];
    memset(count_ob,0,sizeof(count_ob));
    double x1=ray_start_point[0],x2=p[point].pos[0],y1=ray_start_point[1],y2=p[point].pos[1];
    double l_x=y1-y2,l_y=-1*(x1-x2),l_z=x2*y1-x1*y2;
    
    for(int i=1;i<=ob_num;i++)
    {
        for(int k=0;k<4;k++)
        {
            double delta,deltax,deltay,ansa=0,ansb=0;
            delta=line[i][k][0]*l_y-line[i][k][1]*l_x;
            if(delta)
            {
                deltax=line[i][k][2]*l_y-line[i][k][1]*l_z;
                deltay=line[i][k][0]*l_z-line[i][k][2]*l_x;
                ansa=deltax/delta;
                ansb=deltay/delta;
                //cout<<" "<<x1<<" "<<x2<<" "<<" "<<x1-x2<<" "<<" "<<l_y<<p[point].pos[1]<<endl;
                //if(i==1)cout<<i<<" "<<k<<" "<<ansa<<" "<<ansb<<" "<<p[point].pos[1]<<endl;
            }
            
            float rate=(y1-ansb)/(y1-p[point].pos[1]),rate2=(ansb-line[i][k][3])/(line[i][k][4]),rate3=(ansa-line[i][k][5])/(line[i][k][6]);
            //if(y1=p[point].pos[1])rate=1;
            if(!line[i][k][4])rate2=-1;
            if(!line[i][k][6])rate3=-1;
            if(delta!=0)
            {
                //cout<<rate3<<" "<<rate2<<" "<<rate<<endl;
                if((rate3>=0&&rate3<=1.0)||(rate2>=0&&rate2<=1.0))
                {
                    if(rate>=0&&rate<=1)
                    {
                        count_ob[i]++;
                        //cout<<"dd"<<endl;
                    }
                    
                    //cout<<rate<<" "<<rate2<<endl;
                }
            }
        }
        //if(i==1)cout<<p[point].pos[1]<<" "<<count_ob[i]<<endl;
    }
    
    
    for(int i=1;i<=ob_num;i++)
    {
        if(count_ob[i]%2&&count_ob[i])
        {
            //cout<<p[point].pos[1]<<endl;
            return i;
        }
    }
    return 0;
}
double hooke(double posa,double posb,int type)
{
    if(!type)return ks*(abs(posa-posb)-L01);
    else return ks*(abs(posa-posb)-L02);
}
void ob_line(int x1,int y1,int x2,int y2,int lebal,int line_num)
{
    line[lebal][line_num-1][0]=y1-y2;
    line[lebal][line_num-1][1]=-1*(x1-x2);
    line[lebal][line_num-1][2]=x2*y1-x1*y2;
    line[lebal][line_num-1][3]=y1;
    line[lebal][line_num-1][4]=y2-y1;
    line[lebal][line_num-1][3]=x1;
    line[lebal][line_num-1][4]=x2-x1;
    
    //法向量
}
void create_obstacle()
{
    ob_line(0,0,149,0,1,1);
    ob_line(0,-1*wid_wall,149,-1*wid_wall,1,2);
    ob_line(0,-1*wid_wall+1,0,-1,1,3);
    ob_line(149,-1*wid_wall+1,149,-1,1,4);
    
    ob_line(1,0,3,0,2,1);
    ob_line(0,0,0,149,2,2);
    ob_line(1,149,3,149,2,3);
    ob_line(4,0,4,149,2,4);
    
    ob_line(0,149,149,149,3,1);
    ob_line(0,149-4+1,0,148,3,2);
    ob_line(0,149-4,149,149-4,3,3);
    ob_line(149,149-4+1,149,148,3,4);
    
    ob_line(149-4+1,149,148,149,4,1);
    ob_line(149-4,0,149-4,149,4,2);
    ob_line(149-4+1,0,148,0,4,3);
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
                int A=i,B=i+mask_pos[k][0]+mask_pos[k][1]*height,type=abs(mask_pos[k][0]-p[i].pos[0])+abs(mask_pos[k][1]-p[i].pos[1])-1;
                int damping,rx=p[B].pos[0]-p[A].pos[0],ry=p[B].pos[1]-p[A].pos[1],vx=p[B].velocity[0]-p[A].velocity[0],vy=p[B].velocity[1]-p[A].velocity[1];
                damping=kd*(rx*vx+ry*vy)/dis(A,B);
                for(int j=0;j<2;j++)
                {
                    p[i].F[j]=hooke(p[A].pos[j], p[B].pos[j],type)+damping;
                    if(j)p[i].F[j]+=g;
                }
            }
        }
    }//F
    
    for(int i=0;i<particle_amount;i++)
    {
        int lebal=collision(i);
        self_collision(i);
        //cout<<lebal<<" "<<p[i].pos[1]<<endl;
        if(lebal>0)
        {
            float min=float(INT_MAX),distance_;
            int l=0;
            for(int k=0;k<4;k++)
            {
                distance_=abs((p[i].pos[0]*line[lebal][k][0]+p[i].pos[1]*line[lebal][k][1]-line[lebal][k][2]))/sqrt(pow(line[lebal][k][0],2)+pow(line[lebal][k][1],2));
                if(min>distance_)
                {
                    min=distance_;
                    l=k;
                }
            }
            change_velocity(i,line[lebal][l][0],line[lebal][l][1],min);
        }
        else p[i].update();
        draw(i);
    }
    cout<<p[0].pos[0]<<" "<<p[0].pos[1]<<" "<<endl;
}
int main()
{
    //circle.setRadius(radius);
    //circle.setFillColor(Color::Blue);
    for(int i=0;i<particle_amount;i++)
    {
        p[i].pos[0]=pixel/20+i%wid*r;
        p[i].pos[1]=pixel/20+i/wid*r;
    }
    create_obstacle();
    /*
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
     */
    while(true)
    {
        usleep(10);
        spring_mass_model();
    }
    return 0;
}
