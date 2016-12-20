/*


*/


#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#define WIN_W	1024
#define WIN_H	768

unsigned width = 620;
unsigned height = 255;

unsigned startx;
unsigned starty;
unsigned drag = false;



/*=============================================================================
 * SHAPES
 */
void mirrorSquareWave()
{
 float dw = 2;
 glVertex2f(0,0);
 glVertex2f( 20, 20);
 for(float n = 0; n <= 1; n += .1)
    glVertex2f( dw*sin(2*3.14*n) -20, 20-40*n);
 glVertex2f(-20, -20);
 for(float n = 1; n >= 0; n -= .1)
    glVertex2f( dw*sin(2*3.14*n) +20, 20-40*n);
 glVertex2f(20,20);
}

void mirrorCircle60()
{
 float r = 60/2.;
 for(float a = 0; a <= 2*3.1419; a += .2)
    glVertex2f(r*cos(a), r*sin(a));
}

void mirrorCircle100()
{
 float r = 103.5/2.;
 for(float a = 0; a <= 2*3.1419; a += .2)
    glVertex2f(r*cos(a), r*sin(a));
}









/*=============================================================================
 * CLASS: MIRROR
 */
class mirrorC { public:
    mirrorC*	Next;

    unsigned X;
    unsigned Y;
    signed A;
//    bool Selected;
    void (*Vertexes)();

    mirrorC(void (*)());
    ~mirrorC();
};


mirrorC* mirrorSelected = 0;
mirrorC* mirrorHook = 0;

mirrorC::mirrorC(void (*v)())
{
 mirrorC** h;
 for(h = &mirrorHook; *h; h = &(*h)->Next);
 *h = this;

 Next = 0;
 Vertexes = v;

 X = 100;
 Y = 100;
 A = 0;
}

mirrorC::~mirrorC()
{
 mirrorC** h;
 for(h = &mirrorHook; *h != this; h = &(*h)->Next);
 *h = Next;
}





void selectClosestMirror(int xx, int yy)
{
 float x = (xx*width)/(float)WIN_W;
 float y = ((WIN_H-yy)*height)/(float)WIN_H;

 mirrorC* best = 0;
 float bestD = 50*50;

 for(mirrorC* m = mirrorHook; m; m = m->Next)
 {
    float dw = x - m->X;
    float dh = y - m->Y;
    float d = dw*dw + dh*dh;
    if(d < bestD) { best = m; bestD = d; }
 }

 mirrorSelected = best;
}




/*=============================================================================
 * MAIN DISPLAY
 */
void main_display()
{
 glClear(GL_COLOR_BUFFER_BIT);

 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glTranslated(-1, -1, 0);
 glScaled(WIN_W/(double)WIN_W, WIN_W/(double)WIN_H, 1);
 glScaled(2./width, 2./width, 1);
    glScaled(103./28.5,103./24.5,1);
    glTranslated(-340,-40,0);
 glMatrixMode(GL_MODELVIEW);


 //room shape
 int lower_height = 25;
 glColor3f(.8,.8,.8);
 glLoadIdentity();
 glBegin(GL_TRIANGLE_FAN);
    glVertex2f(width/2., height);	// top
    glVertex2f(width, lower_height);
    glVertex2f(width, 0);		// bottom left
    glVertex2f(0,0);
    glVertex2f(0, lower_height);
    glVertex2f(width/2., height);	//top
 glEnd();


 // bed
 glColor3f(240/255.,180/255.,0);
 glLoadIdentity();
 glBegin(GL_TRIANGLE_FAN);
    glVertex2f(width/2.+80, 15);	// top
    glVertex2f(width, 15);
    glVertex2f(width, 0);		// bottom left
    glVertex2f(width/2.+80,0);
 glEnd();


 // mirrors
 for(mirrorC* m = mirrorHook; m; m = m->Next)
 {
    glColor3f(.4, .4, 1);
    glLoadIdentity();
    glTranslated(m->X, m->Y, 0);
    glRotated(m->A, 0, 0, -1);
    glBegin(GL_TRIANGLE_FAN);
	m->Vertexes();
    glEnd();

    if(m == mirrorSelected)
    {
	glColor3f(0,1,0);
	glBegin(GL_LINE_STRIP);
	    m->Vertexes();
	glEnd();
    }
 }

 glutSwapBuffers();
}





/*=============================================================================
 * INPUT & MOUSE
 */
void main_input(unsigned char key, int x, int y)
{
 switch(key) {
    case 27: exit(-1);

    case 'w': if(mirrorSelected) mirrorSelected->Y++; break;
    case 's': if(mirrorSelected) mirrorSelected->Y--; break;
    case 'a': if(mirrorSelected) mirrorSelected->X--; break;
    case 'd': if(mirrorSelected) mirrorSelected->X++; break;

    case 'q': if(mirrorSelected) mirrorSelected->A += 30; break;
    case 'e': if(mirrorSelected) mirrorSelected->A -= 30; break;
    case 'z': if(mirrorSelected) mirrorSelected->A += 5; break;
    case 'c': if(mirrorSelected) mirrorSelected->A -= 5; break;

    case 'p': void saveAll(); saveAll(); break;

    case 'n': new mirrorC(mirrorSquareWave); break;
    case 'm': new mirrorC(mirrorCircle60); break;
    case ',': new mirrorC(mirrorCircle100); break;

    case '/': if(mirrorSelected)
	{
	    mirrorC* m = mirrorSelected;
	    mirrorSelected = mirrorSelected->Next;
	    delete m;
	}
	break;

    case 9: if(mirrorSelected) mirrorSelected = mirrorSelected->Next; break;
    default: break;
 }

 //if(!mirrorSelected) mirrorSelected = mirrorHook;
 glutPostRedisplay();
}



void main_mouse(int b, int state, int x, int y)
{
 switch(b) {
//    case GLUT_WHEEL_UP: mirrorSelected->A += 30; break;
//    case GLUT_WHEEL_DOWN: mirrorSelected->A -= 30; break;

    case GLUT_RIGHT_BUTTON:
	if(state == GLUT_DOWN)
	{
	    drag = true;
	    startx = x;
	    starty = y;
	}
	else
	{
	    drag = false;
	}
	break;

    case GLUT_LEFT_BUTTON:
	if(state == GLUT_DOWN)
	    selectClosestMirror(x, y);
	break;
 }

 glutPostRedisplay();
}


void main_motion(int x, int y)
{
 if(drag) if(mirrorSelected)
 {
    mirrorSelected->X += x-startx;
    mirrorSelected->Y += starty-y;
    startx = x;
    starty = y;

    double dw = mirrorSelected->X - (double)mirrorHook->X;
    double dh = mirrorSelected->Y - (double)mirrorHook->Y;
    mirrorSelected->A = (int)(90 + 180*atan2(dw, dh)/3.1419);
 }
 glutPostRedisplay();
}




/*=============================================================================
 * MAIN
 */
void main_idle()
{
}


void saveAll()
{
 FILE* f = fopen("out.st", "wt");
 if(!f)
 {
    fprintf(stderr, "WTF!!!!\n");
    return;
 }

 for(mirrorC* m = mirrorHook; m; m = m->Next)
 {
    if(m->Vertexes == mirrorSquareWave) fprintf(f, "w");
    if(m->Vertexes == mirrorCircle60) fprintf(f, "6");
    if(m->Vertexes == mirrorCircle100) fprintf(f, "0");
    fprintf(f,",%d,%d,%d\n", m->X, m->Y, m->A);
 }

 fprintf(f, "\n");
 fclose(f);
 printf("saved\n");
}


int main(int argc, char** argv)
{
 glutInit(&argc, argv);

 for(int i = 1; i < argc; i++)
 {
    unsigned x, y;
    signed a;
    char c;
    sscanf(argv[i], "%c,%d,%d,%d", &c, &x, &y, &a);
    mirrorC*m = new mirrorC(mirrorSquareWave);
    m->X = x;
    m->Y = y;
    m->A = a;
    //printf("%s: %c %d %d %d\n", argv[i], c, x, y, a);

    switch(c) {
	case '6': m->Vertexes = mirrorCircle60; break;
	case '0': m->Vertexes = mirrorCircle100; break;
	case 'w': break;
	default: break;
    }
 }

// mirrorSelected = mirrorHook;

 glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
 glutCreateWindow("");
 glutReshapeWindow(WIN_W, WIN_H);
 glutDisplayFunc(main_display);
 glutIdleFunc(main_idle);
 glutKeyboardFunc(main_input);
 glutMouseFunc(main_mouse);
 glutMotionFunc(main_motion);

 glutMainLoop();
}



//EOF =========================================================================
