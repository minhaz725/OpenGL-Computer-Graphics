#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <GL/Glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double theta = pi/180;
double move_dist = 2;
double bighemiradius=30;
double smallhemi_and_cylinder_radius=10;
double cylinder_height=100;

double totalpart;
double totalpart_minus_first_hemi;
double totalpart_minus_two_hemis;
double onlybarrelpart;
int bulletcount;


struct point
{
	double x,y,z;

	point(){
	}
	point(double a,double b,double c)
	{
	    x=a;
	    y=b;
	    z=c;
	}

	point scalermul(double value)
	{
	    return point(x*value, y*value ,z*value);
	}

	double dot(point p)
	{
	    return x*p.x+y*p.y+z*p.z;
	}
	point cross(point p)
	{
	    return point(
                    y*p.z-z*p.y,
                    z*p.x-x*p.z,
                    x*p.y-y*p.x
                    );
	}

	double magnitude(point p)
	{
	    return sqrt(x*x+y*y+z*z);
	}

	point add(point p)
	{
	    return point(x+p.x,y+p.y,z+p.z);
	}
	point sub(point p)
	{
	    return point(x-p.x,y-p.y,z-p.z);
	}

	point rotation(point p, double rotationangle)
	{
	    point rotated;
	    point self(x,y,z);
	    rotated = self.scalermul(cos(rotationangle));
	    rotated = rotated.add( p.cross(self).scalermul(sin(rotationangle)) );
	    return rotated;

	}

};

point bulletlocations[1000];
point pos(100,100,0), u(0,0,1), r(-1/sqrt(2),1/sqrt(2),0),l(-1/sqrt(2),-1/sqrt(2),0);

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
		    glColor3f(1.0, 0.0, 0.0);
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);
				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);



				glVertex3f(i*10, 0, -90);
				glVertex3f(i*10, 0,  90);

				glVertex3f(-90, 0, i*10);
				glVertex3f(90, 0, i*10);


				//glVertex3f(0, -90, i*10);
				//glVertex3f(0,  90, i*10);

				//glVertex3f(0, i*10,-90);
				//glVertex3f(0, i*10,90);


			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCylinder(double radius,double height,int segments)
{
    int i;
    double shade;
    int altercolor = 1;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        //glColor3f(shade,shade,shade);

        altercolor = 1 - altercolor;
        glColor3f(altercolor,altercolor,altercolor);
        glBegin(GL_QUADS);
        {
            glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
			glVertex3f(points[i].x,points[i].y,height);
			glVertex3f(points[i+1].x,points[i+1].y,height);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}



void drawHemiSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	int altercolor = 1;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));

		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
		    altercolor = 1 - altercolor;
		    glColor3f(altercolor,altercolor,altercolor);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

			}glEnd();
		}
	}
}



void drawFlower(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	int altercolor=1;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		r = 2 * radius - r;
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
		    altercolor = 1 - altercolor;
		    glColor3f(altercolor,altercolor,altercolor);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

			}glEnd();
		}
	}
}




void drawSS()
{

    glRotatef(90,0,1,0);
    //white board
    glPushMatrix();
    {
        glTranslatef(0.0,0.0,-800.0);
        glColor3f(1.0,1.0,1.0);
        drawSquare(300);
    }
    glPopMatrix();
    glPushMatrix();
    {

        {
            //first hemi
            glRotatef(totalpart,1,0,0);
            drawHemiSphere(bighemiradius,100,20);
        }

        {
            //2nd hemi

            glRotatef(totalpart_minus_first_hemi,0,1,0);

            glRotatef(180,0,1,0);
            drawHemiSphere(bighemiradius,80,20);
        }

        {
            //3rd hemi
            //3rder center shuru 2nd tar center + 3rder rad
            glRotatef(onlybarrelpart,0,0,1);
            glTranslatef(0,0,bighemiradius);
            glRotatef(totalpart_minus_two_hemis,0,1,0);
            glTranslatef(0,0,-bighemiradius);
            glTranslatef(0,0,bighemiradius+smallhemi_and_cylinder_radius);
            glRotatef(180,0,1,0);
            drawHemiSphere(smallhemi_and_cylinder_radius,80,20);
        }


        {
            //cyclinder
             glRotatef(180,0,1,0);
             drawCylinder(smallhemi_and_cylinder_radius,cylinder_height,80);

        }


        {
            //flower
            glTranslatef(0,0,cylinder_height);
            drawFlower(smallhemi_and_cylinder_radius,80,20);
        }


    }
    glPopMatrix();



    for(int i=0;i<bulletcount;i++)
    {
        glPushMatrix();
        {

                glRotatef(bulletlocations[i].x,1,0,0);
                glRotatef(bulletlocations[i].y,0,1,0);
                glRotatef(bulletlocations[i].z,0,0,1);
                glTranslatef(0.0,0.0,-795.0);
                glColor3f(1.0,0.0,0.0);
                drawSquare(10);
        }
        glPopMatrix();
    }




}

void keyboardListener(unsigned char key, int x,int y){

	switch(key){


		case '1':
			r=r.rotation(u,theta);
			l=l.rotation(u,theta);
			break;
		case '2':
			r=r.rotation(u,-theta);
			l=l.rotation(u,-theta);
			break;
		case '3':
			l=l.rotation(r,theta);
			u=u.rotation(r,theta);
			break;
		case '4':
			l=l.rotation(r,-theta);
			u=u.rotation(r,-theta);
			break;
		case '5':
			r=r.rotation(l,theta);
			u=u.rotation(l,theta);
			break;
		case '6':
		    r=r.rotation(l,-theta);
			u=u.rotation(l,-theta);

			break;
		case '9':
			drawgrid=1-drawgrid;
			break;
		case 'q':
		    if(totalpart<-45) break;
		    totalpart=(totalpart-1);

			break;
		case 'w':
		    if(totalpart>45) break;
		    totalpart=(totalpart+1);

			break;
		case 'e':
		    if(totalpart_minus_first_hemi<-45) break;
		    totalpart_minus_first_hemi=(totalpart_minus_first_hemi-1);

			break;
		case 'r':
		    if(totalpart_minus_first_hemi>45) break;
		    totalpart_minus_first_hemi=(totalpart_minus_first_hemi+1);

			break;
		case 'a':
		    if(totalpart_minus_two_hemis>45) break;
		    totalpart_minus_two_hemis=(totalpart_minus_two_hemis+1);

			break;
		case 's':
		    if(totalpart_minus_two_hemis<-45) break;
		    totalpart_minus_two_hemis=(totalpart_minus_two_hemis-1);


			break;
		case 'd':
		    if(onlybarrelpart>45) break;
		    onlybarrelpart=(onlybarrelpart+1);

			break;
		case 'f':
		    if(onlybarrelpart<-45) break;
		    onlybarrelpart=(onlybarrelpart-1);

			break;


		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			pos = pos.sub(l.scalermul(move_dist));
			break;
		case GLUT_KEY_UP:		// up arrow key
			//cameraHeight += 3.0;
			pos = pos.add(l.scalermul(move_dist));
			break;

		case GLUT_KEY_RIGHT:
			//cameraAngle += 0.03;
			pos = pos.add(r.scalermul(move_dist));
			break;
		case GLUT_KEY_LEFT:
			//cameraAngle -= 0.03;
			pos = pos.sub(r.scalermul(move_dist));
			break;

		case GLUT_KEY_PAGE_UP:
		    pos = pos.add(u.scalermul(move_dist));
			break;
		case GLUT_KEY_PAGE_DOWN:
		    pos = pos.sub(u.scalermul(move_dist));
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
		    if(state == GLUT_DOWN){
                double  angleLimit = (atan2 (300, 800))*180/pi;
               // if ( fabs(totalpart) < angleLimit && fabs(totalpart_minus_first_hemi+ totalpart_minus_two_hemis) < angleLimit )
               //    {
                        bulletlocations[bulletcount].x=totalpart;
                        bulletlocations[bulletcount].y=totalpart_minus_first_hemi;
                        bulletlocations[bulletcount].z=totalpart_minus_two_hemis;
                        bulletcount++;
              //      }
		    }
			break;

		case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
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
	gluLookAt(pos.x,pos.y,pos.z,	pos.x+l.x, pos.y+l.y, pos.z+l.z,	  u.x,u.y,u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();
    drawSS();
	glutSwapBuffers();
}


void animate(){
	//angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

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

	glutCreateWindow("1605093 Problem 1");

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
