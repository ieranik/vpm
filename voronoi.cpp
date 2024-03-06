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

#include <boost/polygon/voronoi.hpp>
using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;
using boost::polygon::x;
using boost::polygon::y;
using boost::polygon::low;
using boost::polygon::high;

using namespace std;

#define pi (2*acos(0.0))
#define eps 10.0
#define MAX_ITER 20


FILE* fout;
FILE* fin;

double X;
double Y;
int N;


struct Point
{
    double a;
    double b;
    Point(double x, double y) : a(x), b(y) {}
};

vector< vector<Point> > edges;


struct Segment
{
    Point p0;
    Point p1;
    Segment(int x1, int y1, int x2, int y2) : p0(x1, y1), p1(x2, y2) {}
};


void print_point(Point p)
{
    cout << p.a << " " << p.b << endl;
}


vector<Point> points;

namespace boost
{
    namespace polygon
    {
        template <>
        struct geometry_concept<Point>
        {
            typedef point_concept type;
        };

        template <>
        struct point_traits<Point>
        {
            typedef int coordinate_type;

            static inline coordinate_type get(const Point& point, orientation_2d orient)
            {
                return (orient == HORIZONTAL) ? point.a : point.b;
            }
        };

        template <>
        struct geometry_concept<Segment>
        {
            typedef segment_concept type;
        };

        template <>
        struct segment_traits<Segment>
        {
            typedef int coordinate_type;
            typedef Point point_type;

            static inline point_type get(const Segment& segment, direction_1d dir)
            {
                return dir.to_int() ? segment.p1 : segment.p0;
            }
        };
    }  // polygon
}  // boost


void drawSquare(double x, double y, double s)
{
	glBegin(GL_QUADS);{
		glVertex3f( x + s, y + s, 0);
		glVertex3f( x + s, y - s, 0);
		glVertex3f( x - s, y - s, 0);
		glVertex3f( x - s, y + s, 0);
	}glEnd();
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


	gluLookAt(0,0,0,	0,0,-1,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

    glColor3f(0, 0, 1);
    for (int i = 0; i < N; i++) drawSquare(points[i].a, points[i].b, 2.0);

    glColor3f(1, 0, 0);
    for (int i = 0; i < edges.size(); i++)
    {
        for (int j = 0; j < edges[i].size() - 1; j++)
        {
            glBegin(GL_LINES);
            {
                glVertex3f(edges[i][j].a, edges[i][j].b, 0.1);
                glVertex3f(edges[i][j + 1].a, edges[i][j + 1].b, 0.1);
            }
            glEnd();
        }
        glBegin(GL_LINES);
        {
            glVertex3f(edges[i][0].a, edges[i][0].b, 0.1);
            glVertex3f(edges[i][edges[i].size() - 1].a, edges[i][edges[i].size() - 1].b, 0.1);
        }
        glEnd();
    }

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}



void animate(int val){
	//codes for any changes in Models, Camera


    glutPostRedisplay();
	glutTimerFunc(50, animate, 0);
}



void input_parameters()
{
    fin = fopen("input.txt", "r");

	fscanf(fin, " %lf", &X);
	fscanf(fin, " %lf", &Y);

	fscanf(fin, " %d", &N);

	points.clear();

	srand(time(NULL));

	for (int i = 0; i < N; i++)
    {
        Point tp(0, 0);
        tp.a = 1 + rand() % ((int)floor(X) - 1);
        tp.b = 1 + rand() % ((int)floor(Y) - 1);
        points.push_back(tp);
    }

    fclose(fin);
}


void write_file()
{
    fout= fopen("voronoi.txt", "w");

	fprintf(fout, "%d\n", N);

	for (int i = 0; i < edges.size(); i++)
    {
        fprintf(fout, "%d\n", edges[i].size());
        for (int j = 0; j < edges[i].size(); j++) fprintf(fout, "%f %f\n", edges[i][j].a, edges[i][j].b);
    }

    fclose(fout);
}


void augment_points()
{
    for (int i = 0; i < N; i++)
    {
        Point p = points[i];
        Point np(0, 0);
        Point ep(0, 0);
        Point wp(0, 0);
        Point sp(0, 0);

        ep.a = 2 * X - p.a;
        ep.b = p.b;

        wp.a = -p.a;
        wp.b = p.b;

        np.a = p.a;
        np.b = 2 * Y - p.b;

        sp.a = p.a;
        sp.b = -p.b;

        points.push_back(ep);
        points.push_back(wp);
        points.push_back(np);
        points.push_back(sp);
    }

}


double area(Point a, Point b, Point c)
{
    return 0.5 * (a.a * (b.b - c.b) + b.a * (c.b - a.b) + c.a * (a.b - b.b));
}


double area_cell(vector<Point> vp)
{
    double sum = 0.0;
    for (int i = 1; i < vp.size() - 1; i++) sum += area(vp[0], vp[i], vp[i + 1]);
    return sum;
}


Point centroid_cell(vector<Point> vp)
{
    double x = 0.0;
    double y = 0.0;
    for (int i = 0; i < vp.size(); i++)
    {
        x += vp[i].a;
        y += vp[i].b;
    }
    x /= (double)vp.size();
    y /= (double)vp.size();
    return Point(x, y);
}


// Traversing Voronoi edges using cell iterator.
vector<Point> process_voronoi_cell(voronoi_diagram<double>::const_cell_iterator &it)
{
    vector<Point> ret;
    ret.clear();

    const voronoi_diagram<double>::cell_type& cell = *it;
    const voronoi_diagram<double>::edge_type* edge = cell.incident_edge();
    // This is convenient way to iterate edges around Voronoi cell.
    do
    {
        if (edge->is_primary() && edge->is_finite())
        {
            const voronoi_diagram<double>::vertex_type* vertex_s = edge->vertex0();
            Point tp(0, 0);
            tp.a = vertex_s->x();
            tp.b = vertex_s->y();
            ret.push_back(tp);
        }
        edge = edge->next();
    } while (edge != cell.incident_edge());
    return ret;
}


void init()
{
	//codes for initialization


	input_parameters();



	for (int i = 0; i < MAX_ITER; i++)
    {
        augment_points();

        edges.clear();
        for (int j = 0; j < N; j++)
        {
            vector<Point> tmp;
            tmp.clear();
            edges.push_back(tmp);
        }


        // Preparing Input Geometries.
        std::vector<Segment> segments;
        segments.clear();

        // Construction of the Voronoi Diagram.
        voronoi_diagram<double> vd;
        construct_voronoi(points.begin(), points.end(), segments.begin(), segments.end(), &vd);

        // Linking Voronoi cells with input geometries.
        {
            for (voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin(); it != vd.cells().end(); ++it)
            {
                if (it->contains_point())
                {
                    if (it->source_category() == boost::polygon::SOURCE_CATEGORY_SINGLE_POINT)
                    {
                        std::size_t index = it->source_index();
                        if (index < N)
                        {
                            edges[index] = process_voronoi_cell(it);
                        }
                        Point p = points[index];
                    }
                }
            }
        }

        bool flag = false;

        for (int j = 0; j < N; j++)
        {
            if (abs(area_cell(edges[j]) - (X * Y / (double)N)) > eps)
            {
                flag = false;
                break;
            }
            flag = true;
        }

        if (flag == true) break;

        vector<Point> next;
        next.clear();

        for (int j = 0; j < N; j++) next.push_back(centroid_cell(edges[j]));

        points = next;
    }

    write_file();



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
	gluOrtho2D(-10, 800 + 10, -10, 600 + 10);



	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}


int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(820, 620);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Baseline Voronoi");

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


