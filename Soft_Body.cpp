#include <iostream>
#include <math.h>
#include <time.h>
#include <vector>
#include <unistd.h>
//#include <SFML/Graphics.hpp>
using namespace std;
//using namespace sf;
const int pixel=1500,wid=5,height=5,particle_amount=wid*height,mask[9][8]={{0,0,0,1,1,1,0,0},{0,1,1,1,0,0,0,0},{0,0,0,0,0,1,1,1},{1,1,0,0,0,0,0,1},{0,1,1,1,1,1,0,0},{0,0,0,1,1,1,1,1},{1,1,1,1,0,0,0,1},{1,1,0,0,0,1,1,1},{1,1,1,1,1,1,1,1}},mask_pos[8][2]={{-1,-1},{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1}},ray_start_point[2]={-500/*改變x為最大值*/,163},hooke_vector[8]={2,0,2,1,2,0,2,1},ob_num=4,wid_wall=500,d_radius=10,d_bias=50;
const double delta_t=0.001,g=-10,radius=1,r=7,L0=r,damp1=1,Mass=1,ks=100,kd=0.02*sqrt(4*Mass*ks),bias=0.5;
double damp[2];

/*RenderWindow window(VideoMode(pixel, pixel), "Fluid Simulation");
CircleShape circle;
RectangleShape rect;*/
class particle
{
    public:
        particle();
        double pos[2],F[2],mass,velocity[2];
        int mask_num;
        bool constraint;
        void update();
        void mask(int i);
};
particle::particle()
{
    velocity[0]=0;
    velocity[1]=0;
    mass=Mass;
    constraint=false;
}
void particle::update()
{
    for(int j=0;j<2;j++)
    {
        if(!constraint)
        {
            velocity[j]+=F[j]/mass*delta_t;
            pos[j]+=velocity[j]*delta_t;
            F[j]=0;
        }
    }
}
void particle::mask(int i)
{
    if(!i)mask_num=0;
    else if(i==wid-1)mask_num=1;
    else if(i==particle_amount-wid)mask_num=2;
    else if(i==particle_amount-1)mask_num=3;
    else if(!(i/wid))mask_num=4;
    else if(!(i%wid))mask_num=5;
    else if(!((i+1)%wid))mask_num=6;
    else if(i/wid==height-1)mask_num=7;
    else mask_num=8;
}
particle p[particle_amount];
int line[ob_num+1][4][7];
void draw(int point)
{
    //circle.setPosition(p[point].pos[0]*10,pixel-p[point].pos[1]*10-d_bias);
    //window.draw(circle);
}
void draw_line(int point1,int point2)
{
    /*
    Vertex line[] =
    {
        Vertex(Vector2f((p[point1].pos[0]+radius)*10,pixel-(p[point1].pos[1]-radius)*10-d_bias)),
        Vertex(Vector2f((p[point2].pos[0]+radius)*10,pixel-(p[point2].pos[1]-radius)*10-d_bias))
    };
    window.draw(line, 2, sf::Lines);
    */
}
void draw_rect(int posa1,int posa2,int posb1,int posb2)
{/*
    rect.setFillColor(Color::White);
    rect.setSize(Vector2f(abs(posa1-posb1)*10,abs(posa2-posb2)*10));
    rect.setPosition(Vector2f(posa1*10,pixel-posa2*10-d_bias+d_radius*2));
    window.draw(rect);
    */
}
double dis(int A,int B)
{
    double posa=p[A].pos[0],posa2=p[A].pos[1];
    double posb=p[B].pos[0],posb2=p[B].pos[1];
    //cout<<posa<<" "<<posb<<" "<<posa2<<" "<<posb2<<endl;
    return sqrt(pow(posa-posb,2)+pow(posa2-posb2,2));
}
void change_velocity(int point,double normal_a,double normal_b,double length)
{
    double dot=normal_a*p[point].velocity[0]+normal_b*p[point].velocity[1];
    
    //cout<<p[point].velocity[1]<<endl;
    //cout<<normal_a<<" "<<normal_b<<endl;
    //cout<<p[1].pos[1]<<"  "<<point<<endl;
    //cout<<"v:";
    //cout<<p[point].velocity[0]<<" "<<p[point].velocity[1]<<endl;
    
    if(normal_a)p[point].velocity[0]-=2*dot/normal_a;
    p[point].velocity[0]*=damp1;
    if(normal_b)p[point].velocity[1]-=2*dot/normal_b;
    p[point].velocity[1]*=damp1;
    
    double r=(sqrt(pow(normal_a,2)+pow(normal_b,2))),unit_a,unit_b;
    unit_a=length*(normal_a/r);
    unit_b=length*(normal_b/r);
    p[point].pos[0]+=unit_a;
    p[point].pos[1]+=unit_b;
    
    //cout<<p[point].velocity[0]<<" "<<p[point].velocity[1]<<endl;
    //cout<<normal_a<<" "<<r<<endl;
    //cout<<p[1].pos[1]<<"  "<<point<<endl;
    //改變位置
    
}

void self_collision(int point)
{
    for(int i=0;i<particle_amount;i++)
    {
        //cout<<dis(i, point)<<endl;
        if(i!=point&&dis(i, point)<2*radius)
        {
            //cout<<p[i].pos[0]-p[point].pos[0]<<" "<<-1*(p[i].pos[1]-p[point].pos[1])<<" "<<radius-dis(i,point)<<endl;
            //cout<<"change: ";
            //cout<<-1*(p[i].pos[1]-p[point].pos[1])<<endl;
            change_velocity(i,p[i].pos[0]-p[point].pos[0],p[i].pos[1]-p[point].pos[1],abs(radius-dis(i,point)));
            for(int j=0;j<2;j++)p[i].F[j]=0;
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
                //if(point==1&&i==1)cout<<" "<<x1<<" "<<x2<<" "<<" "<<x1-x2<<" "<<" "<<l_y<<p[point].pos[1]<<endl;
                //if(i==1)cout<<i<<" "<<k<<" "<<ansa<<" "<<ansb<<" "<<p[point].pos[1]<<endl;
            }
            float rate=(y1-ansb)/(y1-p[point].pos[1]),rate2=(ansb-line[i][k][3])/(line[i][k][4]),rate3=(ansa-line[i][k][5])/(line[i][k][6]);
            //if(y1=p[point].pos[1])rate=1;
            if(!line[i][k][4])rate2=-1;
            if(!line[i][k][6])rate3=-1;
            //if(i==4&&point==1&&k==1)cout<<rate<<" "<<rate2<<" "<<rate3<<" "<<ansa<<" "<<ansb<<endl;
            if(delta!=0)
            {
                //cout<<rate3<<" "<<rate2<<" "<<rate<<endl;
                if((rate3>=0&&rate3<=1.0)||(rate2>=0&&rate2<=1.0))
                {
                    if(rate>0&&rate<1)
                    {
                        count_ob[i]++;
                        //cout<<"dd"<<endl;
                    }
                    
                    //cout<<rate<<" "<<rate2<<endl;
                }
                
            }
        }
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
double hooke(double difference)
{
    int differ,v;
    differ=(abs(difference)-L0>0)?1:-1;
    v=(difference>0)?1:-1;
    
    return ks*abs((abs(difference)-L0))*differ*v;
}
void ob_line(int x1,int y1,int x2,int y2,int lebal,int line_num)
{
    line[lebal][line_num-1][0]=y1-y2;
    line[lebal][line_num-1][1]=-1*(x1-x2);
    line[lebal][line_num-1][2]=x2*y1-x1*y2;
    line[lebal][line_num-1][3]=y1;
    line[lebal][line_num-1][4]=y2-y1;
    line[lebal][line_num-1][5]=x1;
    line[lebal][line_num-1][6]=x2-x1;
    
    //法向量
}
void draw_ob()
{
    draw_rect(0,0,149,-wid_wall);
    draw_rect(-wid_wall,149,0,0);
    draw_rect(0,149+wid_wall,149,149);
    draw_rect(149,149,149+wid_wall,0);
}
void create_obstacle()
{
    ob_line(0,0,149,0,1,1);
    ob_line(0,-1*wid_wall,149,-1*wid_wall,1,2);
    ob_line(0,-1*wid_wall+1,0,-1,1,3);
    ob_line(149,-1*wid_wall+1,149,-1,1,4);
    
    ob_line(-wid_wall+1,0,-1,0,2,1);
    ob_line(-wid_wall,0,-wid_wall,149,2,2);
    ob_line(-wid_wall+1,149,-1,149,2,3);
    ob_line(0,0,0,149,2,4);
    
    ob_line(0,149+wid_wall,149,149+wid_wall,3,1);
    ob_line(0,149+1,0,149+wid_wall-1,3,2);
    ob_line(0,149,149,149,3,3);
    ob_line(149,149+1,149,149+wid_wall-1,3,4);
    
    ob_line(149+1,149,149+wid_wall-1,149,4,1);
    ob_line(149,0,149,149,4,2);
    ob_line(149+1,0,149+wid_wall-1,0,4,3);
    ob_line(149+wid_wall,0,149+wid_wall,149,4,4);
    
    
    
}
void spring_mass_model()
{
    for(int i=0;i<particle_amount;i++)
    {
        for(int k=0;k<8;k++)
        {
            if(mask[p[i].mask_num][k])
            {
                int A=i,B=i+mask_pos[k][0]+mask_pos[k][1]*wid;
                
                if(B>=0)
                {
                    for(int j=0;j<2;j++)
                    {
                        damp[j]=-1*kd*(p[A].velocity[j]);
                        if(hooke_vector[k]<2&&hooke_vector[k]==j)
                        {
                            //if(!j)cout<<p[A].pos[j]-p[B].pos[j]<<" hooke: "<<hooke(p[A].pos[j],p[B].pos[j],type)<<" "<<damping<<endl;
                            p[i].F[j]+=hooke(p[B].pos[j]-p[A].pos[j]);
                            if(hooke(p[B].pos[j]-p[A].pos[j])>=bias)p[i].F[j]+=damp[j];
                            //if(B==1&&j)cout<<(abs(p[A].pos[j]-p[B].pos[j])-L01)<<" "<<hooke(p[A].pos[j], p[B].pos[j],type)<<endl;
                        }
                        else if(hooke_vector[k]>=2)
                        {
                            p[i].F[j]+=hooke(p[B].pos[j]-p[A].pos[j]);
                            if(hooke(p[B].pos[j]-p[A].pos[j])>=bias)p[i].F[j]+=damp[j];
                        }
                    
                    }
                    draw_line(A, B);
                }
            }
        }
        p[i].F[1]+=g*Mass;
    }//F
    
    for(int i=0;i<particle_amount;i++)
    {
        int lebal=collision(i);
        //cout<<lebal<<" "<<p[i].pos[1]<<endl;
        //cout<<lebal<<endl;
        self_collision(i);
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
            for(int j=0;j<2;j++)p[i].F[j]=0;
        }
        else p[i].update();
        draw(i);
    }
    //cout<<"F:"<<p[0].F[1]<<" "<<p[0].F[0]<<" "<<p[1].F[1]<<" "<<p[1].F[0]<<endl;
    cout<<p[3].pos[1]<<" "<<p[3].pos[0]<<" "<<p[5].pos[1]<<" "<<p[1].pos[0]<<endl;
}
int main()
{
    //circle.setRadius(d_radius/100);
    //circle.setFillColor(Color::White);
    for(int i=0;i<particle_amount;i++)
    {
        p[i].mask(i);
        p[i].pos[0]=pixel/20+i%wid*r;
        p[i].pos[1]=0+i/wid*r;
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
        usleep(1000);
        window.clear(Color::Black);
        spring_mass_model();
        draw_ob();
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
