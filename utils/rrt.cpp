#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>
#include <time.h>
#include <stdlib.h>
#include <map>
#include <conio.h>
#include <unordered_set>
#include <queue>

#include <windows.h>
#include <glut.h>

using namespace std;

#define pi (2*acos(0.0))
#define eps 0.000001

#define R 1
#define N 100
#define M 100
#define numobs 40
#define crange 5
#define osa 8
#define osd 4
#define lmax 100.0
#define cpx 6
#define edis 15
#define ncnt 100
#define hor 50


int G[N][M];
bool B[N][M];
int T;
vector< vector<int> > AL;


FILE* finv;


double max2(double a,double b)
{
    return (a>b)?a:b;
}

double min2(double a,double b)
{
    return (a<b)?a:b;
}


class point
{
public:
    double x, y;
    point(double xx = 0.0, double yy = 0.0)
    {
        x = xx;
        y = yy;
    }
};


class cell
{
public:
    int x, y;
    point c;
    cell(int xx = 0, int yy = 0)
    {
        x = xx;
        y = yy;
        c.x = (double)x + 0.5;
        c.y = (double)y + 0.5;
    }
};


class obs
{
public:
    double xl, xh, yl, yh;
    obs(double xxl = 0.0, double xxh = 0.0, double yyl = 0.0, double yyh = 0.0)
    {
        xl = xxl;
        xh = xxh;
        yl = yyl;
        yh = yyh;
    }
};


class rrtnode
{
public:
    point loc;
    int par;
    double dis;
    bool leaf;
    rrtnode(){}
};


vector<obs> obsdata;
vector<point> pos;
vector<rrtnode> gr;
vector<point> gp;
vector<point> gedp;
vector< vector<rrtnode> > rrts;
int gl[N][M];
int bp;


double sqrdis(point p1, point p2)
{
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

void ppoint(point p)
{
    cout << p.x << " " << p.y << endl;
}

bool doesint1(obs a,obs b)
{
    if(((b.xl <= a.xl && a.xl <= b.xh)||(b.xl <= a.xh && a.xh <= b.xh)||(a.xl <= b.xl && b.xl <= a.xh)||(a.xl <= b.xh && b.xh <= a.xh))&&((b.yl <= a.yl && a.yl <= b.yh)||(b.yl <= a.yh && a.yh <= b.yh)||(a.yl <= b.yl && b.yl <= a.yh)||(a.yl <= b.yh && b.yh <= a.yh)))return true;

    return false;
}

bool doesintn(vector<obs> ol,obs o)
{
    int i;
    for(i=0;i<ol.size();i++)if(doesint1(ol[i],o)==true)return true;

    return false;
}

bool isinside1(obs o, point p)
{
    if(o.xl<p.x+eps&&p.x<o.xh+eps&&o.yl<p.y+eps&&p.y<o.yh+eps)return true;

    return false;
}

bool isinsiden(vector<obs> ol, point p)
{
    int i;
    for(i=0;i<ol.size();i++)if(isinside1(ol[i],p)==true)return true;

    return false;
}


//generate obstacles uniformly over the environment at random
void genobs()
{
    obsdata.clear();

    obs o;
    int i, ri, rii;
    double rd;
    srand(time(NULL));

    int cnt = numobs;
    while(cnt != 0)
    {
        o.xl = 1 + rand() % (N - osa - osd);
        ri = rand() % (2 * osd) + (osa - osd);
        o.xh = o.xl + ri;

        o.yl = 1 + rand() % (M - osa - osd);
        ri = rand() % (2 * osd) + (osa - osd);
        o.yh = o.yl + ri;


        if (doesintn(obsdata, o) == false)
        {
            obsdata.push_back(o);
            cnt--;
        }
    }
}


//determine free (true) and occupied (false) grid cells
void genblockedcells()
{
    int i, j, k;

	for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            point tp;

            tp.x = (double)i + 0.5;
            tp.y = (double)j + 0.5;

            bool flag = true;
            for (k = 0; k < numobs; k++)
            {
                if (isinside1(obsdata[k], tp) == true)
                {
                    flag = false;
                    break;
                }
            }
            B[i][j] = flag;
        }
    }
}


double area3p(point a, point b, point c)
{
    return a.x*(b.y-c.y)+b.x*(c.y-a.y)+c.x*(a.y-b.y);
}

bool doesint(point a, point b, point c, point d)
{
    if(area3p(a,b,c)*area3p(a,b,d)<=0&&area3p(c,d,a)*area3p(c,d,b)<=0)
        return true;
    return false;
}

//determine if point p falls within any obstacle
vector<obs> rangequery(point p)
{
    int j;
    vector<obs> ret;
    obs to;
    to.xl = p.x - crange;
    to.xh = p.x + crange;
    to.yl = p.y - crange;
    to.yh = p.y + crange;
    for (j = 0; j < obsdata.size(); j++)
    {
        if(doesint1(obsdata[j], to)==true) ret.push_back(obsdata[j]);
    }
    return ret;
}


void drawSquare(double x, double y, double s)
{
	glBegin(GL_QUADS);{
		glVertex3f( x + s, y + s, 0);
		glVertex3f( x + s, y - s, 0);
		glVertex3f( x - s, y - s, 0);
		glVertex3f( x - s, y + s, 0);
	}glEnd();
}


void drawCircle(double x, double y, double radius, double h)
{
    int i;
    int segments = 16;
    struct point points[17];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x = x + radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y = y + radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(x, y, h);
			glVertex3f(points[i].x,points[i].y,h);
			glVertex3f(points[i+1].x,points[i+1].y,h);
        }
        glEnd();
    }
}


void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			//drawgrid = 1 - drawgrid;
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			break;
		case GLUT_KEY_UP:		// up arrow key
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

//	if (top == 0) gluLookAt(240 + 275 * cos(cameraangle), 240 + 275 * sin(cameraangle), cameraheight,		240,240,0,		0,0,1);
//	else if (top == 1) gluLookAt(0,0,0,	0,0,-1,	0,1,0);

	gluLookAt(0,0,0,	0,0,-1,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects


    int i,j;

    //dras obstacles in red
    glColor3f(1, 0, 0);
    for(int i = 0; i < obsdata.size(); i++)
    {
        glBegin(GL_QUADS);
        {
			glVertex3f(obsdata[i].xl * cpx, obsdata[i].yl * cpx, 0);
			glVertex3f(obsdata[i].xh * cpx, obsdata[i].yl * cpx, 0);
			glVertex3f(obsdata[i].xh * cpx, obsdata[i].yh * cpx, 0);
			glVertex3f(obsdata[i].xl * cpx, obsdata[i].yh * cpx, 0);
        }
        glEnd();
    }


//    //draw robots
//    glColor3f(0, 0, 1);
//    for(int i = 0; i < pos.size(); i++)
//    {
//        drawCircle(pos[i].x * cpx, pos[i].y * cpx, 3, 0.1);
//    }



//    //draw free grid cells according to latency value
//    for (i = 0; i < N; i++)
//    {
//        for (j = 0; j < M; j++)
//        {
//            glColor3f(0, 1 - (double)(G[i][j] / lmax), 0);
//            glBegin(GL_QUADS);
//            {
//                glVertex3f(i * cpx, j * cpx, 0);
//                glVertex3f((i + 1) * cpx, j * cpx, 0);
//                glVertex3f((i + 1) * cpx, (j + 1) * cpx, 0);
//                glVertex3f(i * cpx, (j + 1) * cpx, 0);
//            }
//            glEnd();
//        }
//    }


    glLineWidth(2.0);
    for (i = 0; i < rrts.size(); i++)
    {
        for (j = 0; j < rrts[i].size(); j++)
        {
            if (rrts[i][j].par != -1)
            {
                point p1 = rrts[i][j].loc;
                point p2 = rrts[i][rrts[i][j].par].loc;
                glColor3f(0, 1, 0);
                glBegin(GL_LINES);
                {
                    glVertex3f(p1.x * cpx, p1.y * cpx, 0.1);
                    glVertex3f(p2.x * cpx, p2.y * cpx, 0.1);
                }
                glEnd();

            }
            else
            {
                glColor3f(0, 0, 1);
                drawCircle(rrts[i][j].loc.x * cpx, rrts[i][j].loc.y * cpx, 5, 0.1);
            }
        }
    }



    //drawCircle(gr[bp].loc.x * cpx, gr[bp].loc.y * cpx, 5, 0.1);





	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


//update position of UGVs and latency values of free grid cells at each time step
void animate(int val){
	//codes for any changes in Models, Camera

    T++;

    unordered_set<int> st;


    int cn = T + 2000;
    for (int j = 0; j< AL[cn].size(); j++) st.insert(AL[cn][j]);


    for (int i = 0; i < M * N; i++)
    {
        auto it = st.find(i);
        if (it != st.end()) G[i % N][i / N] = 0;
        else if (G[i % N][i / N] < lmax) G[i % N][i / N]++;
    }

    glutPostRedisplay();
	glutTimerFunc(50, animate, 0);
}


//find the cells visible from a given cell
vector<int> processcell(cell c)
{
    point p = c.c;
    vector<int> ret;
    ret.clear();

    //determine bounding square around cells visible from a UGV
    vector<obs> vo = rangequery(p);
    int lgn = max(0, c.x - (int)(crange));
    int rgn = min(N - 1, c.x + (int)(crange));
    int bgn = max(0, c.y - (int)(crange));
    int tgn = min(M - 1, c.y + (int)(crange));


    //check each cell within the bounding square to see if its visible from the UGV
    int i, j, k;
    for (i = lgn; i <= rgn; i++)
    {
        for (j = bgn; j <= tgn; j++)
        {
            point tp = point((double)i + 0.5, (double)j + 0.5);
            if (B[i][j] == false) continue;
            if ((p.x - tp.x) * (p.x - tp.x) + (p.y - tp.y) * (p.y - tp.y) > crange * crange) continue;
            bool flag = true;
            for (k = 0; k < vo.size(); k++)
            {
                if (doesint(p, tp, point(vo[k].xl, vo[k].yl), point(vo[k].xl, vo[k].yh)))
                {
                    flag = false;
                    break;
                }
                if (doesint(p, tp, point(vo[k].xh, vo[k].yl), point(vo[k].xh, vo[k].yh)))
                {
                    flag = false;
                    break;
                }
                if (doesint(p, tp, point(vo[k].xl, vo[k].yl), point(vo[k].xh, vo[k].yl)))
                {
                    flag = false;
                    break;
                }
                if (doesint(p, tp, point(vo[k].xl, vo[k].yh), point(vo[k].xh, vo[k].yh)))
                {
                    flag = false;
                    break;
                }
            }
            if (flag == true)
            {
                ret.push_back(j * N + i);
            }
        }
    }
    return ret;
}


bool isFree(point p)
{
    return B[(int)floor(p.x)][(int)floor(p.y)];
}


double randd(int r)
{

    double w = rand() % r;
    double f = rand() % 999;
    f = (f + 1) / 1000.0;
    return w + f;
}


point randp()
{
    point p = point(randd(N), randd(M));
    while (isFree(p) == false) p = point(randd(N), randd(M));
    return p;
}


void genpos()
{
    int cnt = 0;
    pos.clear();
    while (cnt < R)
    {
        cnt++;
        pos.push_back(point(50.1, 50.1));
    }
}


cell ptoc(point p)
{
    return cell((int)floor(p.x), (int)floor(p.y));
}



point divpoint(point s, point e, double r)
{
    point ret;
    ret.x = s.x + (e.x - s.x) * r;
    ret.y = s.y + (e.y - s.y) * r;
    return ret;
}



point divpointdis(point s, point e, double d)
{
    double dis = sqrt(sqrdis(s, e));
    double r = d /dis;
    return divpoint(s, e, r);
}



//false if obstructed, true if unobstructed
bool checkobs(point e, point s)
{
    if (isFree(s) == false || isFree(e) == false) return false;
    double dis = sqrt(sqrdis(e, s));
    int loop = (int)floor(dis);

    for (int i = 1; i <= loop; i++)
    {
        point p = divpoint(s, e, (double)i / dis);
        cell c = ptoc(p);
        int x = c.x;
        int y = c.y;
        //if (B[x][y] == false || B[x + 1][y] == false || B[x - 1][y] == false || B[x][y + 1] == false || B[x][y - 1] == false) return false;
        if (B[x][y] == false) return false;
    }
    return true;
}


class robot
{
public:
    int index;
    double dis;
    robot(){}
};


bool comparator(const robot& lhs, const robot& rhs)
{
   return lhs.dis < rhs.dis;
}


int getpar(point p)
{
    vector<robot> rb;
    for (int i = 0; i < gr.size(); i++)
    {
        if (sqrdis(p, gr[i].loc) <= edis * edis)
        {
            robot tr;
            tr.index = i;
            tr.dis = sqrdis(p, gr[i].loc);
            rb.push_back(tr);
        }
    }
    sort(rb.begin(), rb.end(), &comparator);

    for (int i = 0; i < rb.size(); i++)
    {
        if (checkobs(p, gr[rb[i].index].loc)) return rb[i].index;
    }


    return -1;
}


void genrrt()
{
    for (int i = 0; i < R; i++)
    {
        gr.clear();

        rrtnode root;
        root.loc = point(pos[i].x, pos[i].y);
        root.par = -1;
        root.dis = 0.0;
        root.leaf = false;

        gr.push_back(root);

        int cnt = 1;
        while (cnt < ncnt)
        {
            point tp = randp();
            int par = getpar(tp);
            if (par != -1 && gr[par].leaf == false)
            {
                rrtnode node;
                node.loc = tp;
                //ppoint(tp);
                node.par = par;
                node.dis = gr[par].dis + sqrt(sqrdis(tp, gr[par].loc));
                node.leaf = (node.dis - eps > hor) ? true : false;
                gr.push_back(node);
                cnt++;
            }
        }
        rrts.push_back(gr);
        //cout << gr.size() << endl;
    }
}


void genpoints(int cur)
{
    gp.clear();
    while (gr[cur].par != -1)
    {
        gp.push_back(gr[cur].loc);
        cur = gr[cur].par;
    }
    gp.push_back(gr[cur].loc);
    reverse(gp.begin(), gp.end());

    gedp.clear();

    int ind = 0;
    double prevdis = 0.0;
    double curdis = sqrt(sqrdis(gp[ind], gp[ind + 1]));
    int i = 1;
    while (i <= hor)
    {
        if ((double)i <= curdis)
        {
            i++;
            gedp.push_back(divpointdis(gp[ind], gp[ind + 1], (double)i - prevdis));
        }
        else
        {
            while ((double)i > curdis)
            {
                ind++;
                prevdis = curdis;
                curdis += sqrt(sqrdis(gp[ind], gp[ind + 1]));
            }
        }
    }
    cout << gedp.size() << endl;
}


int bestpath()
{
    int maxidx = -1;
    int maxcov = -1;
    for (int i = 0; i < gr.size(); i++)
    {
        if (gr[i].leaf == true)
        {
            genpoints(i);
            //memset(gl, 0, sizeof(gl));
            for (int j = 0; j < N; j++) for (int k = 0; k < M; k++) gl[j][k] = 0;
            for (int j = 0; j < gedp.size(); j++)
            {
                cell tc = ptoc(gedp[j]);
                int cn = tc.y * N + tc.x;
                for (int k = 0; k < (int)AL[cn].size(); k++)
                {
                    int x = AL[cn][k] % N;
                    int y = AL[cn][k] / N;
                    gl[x][y] = 1;
                }
            }
            int cov = 0;
            for (int j = 0; j < N; j++) for (int k = 0; k < M; k++) if (gl[j][k] == 1) cov += G[j][k];

            ppoint(gr[i].loc);
            cout << i << " " << cov << endl << endl;

            if (cov > maxcov)
            {
                maxcov = cov;
                maxidx = i;
            }
        }
    }
    cout << maxidx << endl;
    return maxidx;
}


//generate the trajectories for all voronoi regions
void init(){
	//codes for initialization

	int i, j;
    T = 0;

	genobs();
	genblockedcells();
	genpos();

	rrts.clear();
	genrrt();



	cout << "Processing grid cells..." << endl;
	AL.clear();
    for (i = 0; i < M; i++) for (j = 0; j < N; j++) AL.push_back(processcell(cell(j, i)));
    for (i = 0; i < N; i++) for (j = 0; j < M; j++) G[i][j] = lmax;
    cout << "Grid cells processed." << endl;


    gr = rrts[0];
	bp = bestpath();



//    finv = fopen(inputfilename, "r");
//	  fscanf(finv, " %d", &numvcells);
//
//
//    alltraj.clear();
//
//    for (i = 0; i < numvcells; i++)
//    {
//        inputvoronoi();
//        processvoronoicell();
//        gentraj();
//        alltraj.push_back(traj);
//        cout << "Voronoi cell " << i << " processed." << endl;
//    }
//
//    fclose(finv);




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
	gluOrtho2D(-10, N * cpx + 10, -10, M * cpx + 10);


	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}



int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(N * cpx + 20, M * cpx + 20);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Baseline Lawnmower");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutTimerFunc(50, animate, 0);		//what you want to do in the idle time (when no drawing is occuring)

	//glutIdleFunc(animate);

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}


