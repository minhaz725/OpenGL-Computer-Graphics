#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <GL/glut.h>
#include <time.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;

double smallCircRadius=10;
double bigCircRadius=50;
double sqrEdge=100;
#define bubblenumber 6
bool inside[bubblenumber];
bool bubble_bounced_inside_big[bubblenumber];
int delay=1000;
int initdelay=1000;
double speed=1;
double initvelocity=0.05;
int pause=1;


struct point
{
	double x,y,z;
};


struct velocity
{
    double v,angle,vx,vy;

};

double compx(velocity vel)
{
    return  vel.v*(cos(vel.angle*pi/180));
}

double compy(velocity vel)
{
    return  vel.v*(sin(vel.angle*pi/180));
}

double getangle(double y , double x)
{
     return atan(y/x)*(180/pi);
    //if(180+ang >= 180) return ang;
    //else return 180+ang;
}
struct point center={0,0};
struct velocity v1[bubblenumber];
struct velocity v2;
struct velocity v3;

struct point p;
struct point pb[bubblenumber];




void drawSquare(double a)
{
    glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINES);{
	    glVertex3f( a, a,0);
		glVertex3f( a, -a,0);

        glVertex3f( a,-a,0);
		glVertex3f( -a,-a,0);

		glVertex3f( -a,-a,0);
		glVertex3f( -a,a,0);

		glVertex3f( -a,a,0);
		glVertex3f( a,a,0);

	}glEnd();
}


void drawCircle(double radius,int segments, point cent)
{
    int i;
    struct point points[100];
    //glColor3f(0.7,0.7,0.7);

    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    //printf("cnx %lf",center.x);
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x+cent.x+radius, points[i].y+cent.y+radius,0);
			glVertex3f(points[i+1].x+cent.x+radius, points[i+1].y+cent.y+radius, 0);
        }
        glEnd();
    }
}



void drawSS()
{

   // drawCircle(30,24);
   if(pause==1)
    {
        if(delay<=bubblenumber*initdelay) delay++;
    }
    drawSquare(sqrEdge);

    glColor3f(1.0,0.0,0.0);
    drawCircle(bigCircRadius,50,p);

    int i;
    for(i=0;i<delay/initdelay;i++)
	{
	    drawCircle(smallCircRadius,50,pb[i]);
	}






}


void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case 'p':
			pause=1-pause;
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow

		    if(speed<=0) break;
		    speed = speed - 0.5;
			break;
		case GLUT_KEY_UP:
		    	// up arrow key
		    if(speed>5) break;
			speed = speed + 0.5;

			break;

		case GLUT_KEY_RIGHT:

			break;
		case GLUT_KEY_LEFT:

			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/

    drawSS();

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

double getdistance(point p,point q)
{
    p.x=p.x+smallCircRadius;
    p.y=p.y+smallCircRadius;
    if(q.x!=0 && q.y!=0)
    {
        q.x=q.x+smallCircRadius;
        q.y=q.y+smallCircRadius;
    }
    return sqrt( (p.x-q.x)*(p.x-q.x) + (p.y-q.y)*(p.y-q.y));
}


void checkSqrBounce(int i)
{
    if(pb[i].x + 2*smallCircRadius  >= sqrEdge)  v1[i].vx=-v1[i].vx;
    if(pb[i].x  +  sqrEdge <  0) v1[i].vx=-v1[i].vx;
    if(pb[i].y + 2*smallCircRadius - sqrEdge >= 0.1) v1[i].vy=-v1[i].vy;
    if(pb[i].y + sqrEdge < 0) v1[i].vy=-v1[i].vy;

}


velocity add(velocity v1,velocity v2)
{
    struct velocity v;
    v.vx=v1.vx+v2.vx;
    v.vy=v1.vy+v2.vy;
    return v;
}

velocity sub(velocity v1,velocity v2)
{
    struct velocity v;
    v.vx=v1.vx-v2.vx;
    v.vy=v1.vy-v2.vy;
    return v;
}


velocity projection(velocity v1,velocity v2)
{
    double val=(v2.vx*v1.vx + v2.vy*v1.vy)/(v2.vx*v2.vx + v2.vy*v2.vy);
    v2.vx=v2.vx*val;
    v2.vy=v2.vy*val;
    return v2;

}

velocity normalize(velocity v)
{
    double magnitude=sqrt(v.vx*v.vx+v.vy*v.vy);
    v.vx=v.vx/magnitude;
    v.vy=v.vy/magnitude;
    return v;
}

double dotproduct(velocity v1,velocity v2)
{
    return v1.vx*v2.vx+v1.vy*v2.vy;
}

velocity scalermul(velocity v,double magnitude)
{
    v.vx=v.vx*magnitude;
    v.vy=v.vy*magnitude;
    return v;
}

void checkCircBounce(int i)
{

    double dist=getdistance(pb[i],center);
            if( bigCircRadius < dist + smallCircRadius ){ //

                struct velocity n;
                n.vx=-pb[i].x-smallCircRadius;
                n.vy=-pb[i].y-smallCircRadius;   //n vec

                n=normalize(n);
                //printf("px py %lf %lf nx ny %lf %lf\n",pb.x,pb.y,n.vx,n.vy);
                struct velocity r;  //erporer line gula sirer shutro
                double dn= dotproduct(v1[i],n);
                if(dn <0 ){
                    n=scalermul(n,dn*2);

                    r= sub(v1[i],n);

                    v1[i].vx=r.vx;
                    v1[i].vy=r.vy;  //final updated v
                    bubble_bounced_inside_big[i]=true;
                }
            }


}





void checkBubbleBounce(int i,int j)
{

    double dist=getdistance(pb[i],pb[j]);
            if( dist - 2*smallCircRadius <0){
                if(inside[i]==true && inside[j]==true){ //

                struct velocity n;
                n.vx=pb[j].x-smallCircRadius;
                n.vy=pb[j].y-smallCircRadius;
                struct velocity q;
                q.vx=pb[i].x-smallCircRadius;
                q.vy=pb[i].y-smallCircRadius;
                n=sub(n,q);
                //n[j].vx=-pb[i].x-smallCircRadius;
                //n[j].vy=-pb[i].y-smallCircRadius;   //n vec

                n=normalize(n);
                //printf("px py %lf %lf nx ny %lf %lf\n",pb.x,pb.y,n.vx,n.vy);
                struct velocity r;  //erporer line gula sirer shutro
                double dn= dotproduct(v1[i],n);
                if(dn > 0 ) {
                    n=scalermul(n,dn*2);

                    r= sub(v1[i],n);

                    v1[i].vx=r.vx;
                    v1[i].vy=r.vy;  //final updated v
                    //v1[j].vx=r.vx;
                    //v1[j].vy=r.vy;
                }
               // v1[j].vx=-r.vx;
               // v1[j].vy=-r.vy;

            }
        }


}








void checkCircDistance(int i)
{
    double dist=getdistance(pb[i],center);

    if(bigCircRadius > dist +smallCircRadius +1) {
        inside[i]=true;
    }

}

void animate(){

	//codes for any changes in Models, Camere
    int i;
	for(i=0;i<delay/initdelay;i++)
	{

        checkSqrBounce(i);
        checkCircDistance(i);

        if(inside[i]) {
                checkCircBounce(i);

                {

                    for(int j=0;j<bubblenumber;j++)
                    {
                        if(i==j) continue;
                        checkBubbleBounce(i,j);
                    }

                }

        }

        if(pause==1)
        {
            pb[i].x=pb[i].x + v1[i].vx*speed;
            pb[i].y=pb[i].y + v1[i].vy*speed;
         //   printf("%lf\n",speed);
        }


	}

	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;

    int i;
    for(i=0;i<bubblenumber;i++)
	{
	    pb[i].x=pb[i].y=-100;
	    inside[i]=false;
	    bubble_bounced_inside_big[i]=false;

	}


    p.x=-50;
    p.y=-50;

    //.angle=85-((i+1)*10
	for(i=0;i<bubblenumber;i++)
	{
	    v1[i] = {.v=initvelocity,.angle=(rand()%80 + 10)};
	    v1[i].vx= compx(v1[i]);
        v1[i].vy= compy(v1[i]);
	}
	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color
    srand (time(NULL));
	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
