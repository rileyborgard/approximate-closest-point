
#include <iostream>
#include <complex>
#include <vector>
#include <algorithm>
#include <tuple>
#include <GL/freeglut.h>

using namespace std;

#define COLOR(x) get<0>(x), get<1>(x), get<2>(x)

#define pt complex<double>
#define x real()
#define y imag()
#define dot(u, v) (conj(u) * (v)).x
#define cross(u, v) (conj(u) * (v)).y

const double EPS = 1e-10;
const pt I(0, 1);
const int N = 200; // number of points
const int SEED = 20, SEED2 = 5; // number of seeds
const int PREC = 512;

int width = 800, height = 800;

typedef tuple<float, float, float> color;
color backgroundCol = {1.f, 1.f, 1.f};
color pointCol = {0.f, 0.f, 0.f};
vector<color> cols;

vector<pt> points;
vector<pair<float, int>> hilbert[SEED];

//rotate/flip a quadrant appropriately
void rot(int n, int *X, int *Y, int rx, int ry) {
    if (ry == 0) {
        if (rx == 1) {
            *X = n - 1 - *X;
            *Y = n - 1 - *Y;
        }

        //Swap x and y
        int t  = *X;
        *X = *Y;
        *Y = t;
    }
}

//convert (x,y) to d
int xy2d (int n, int X, int Y) {
    int rx, ry, s, d=0;
    for(s = n / 2; s > 0; s /= 2) {
        rx = (X & s) > 0;
        ry = (Y & s) > 0;
        d += s * s * ((3 * rx) ^ ry);
        rot(n, &X, &Y, rx, ry);
    }
    return d;
}
float p2d(int n, pt p) {
    p *= n;
    return 1.f * xy2d(n, floor(p.x), floor(p.y)) / (n * n - 1);
}
float p2dseed(int seed, int n, pt p) {
    // rotate by angle from seed
    double ang = seed;
    p = (p - 0.5 - 0.5 * I) * polar(0.5, ang) + 0.5 + 0.5 * I + polar(0.1, ang);
    return p2d(n, p);

    // p = ((p - 0.5 - 0.5 * I) * 0.5) + 0.5 + 0.5 * I + 0.5 * (seed % SEED2) * (1. / SEED2) + (0.5) * (seed / SEED2) * (1. * SEED / SEED2) * I;
    // return p2d(n, p);
}

int nearestseed(int s, pt p) {
    float d = p2dseed(s, PREC, p);
    int i = lower_bound(hilbert[s].begin(), hilbert[s].end(), make_pair(d, 0)) - hilbert[s].begin();
    if(i == 0) return hilbert[s][0].second;
    if(i == N) return hilbert[s][N - 1].second;
    return d - hilbert[s][i - 1].first < hilbert[s][i].first - d ? hilbert[s][i - 1].second : hilbert[s][i].second;
    // return norm(points[i] - p) < norm(points[i - 1] - p) ? hilbert[i].second : hilbert[i - 1].second;
}
int nearest(pt p) {
    int i = -1;
    for(int s = 0; s < SEED; s++) {
        int j = nearestseed(s, p);
        if(i == -1 || norm(points[j] - p) < norm(points[i] - p)) {
            i = j;
        }
    }
    return i;
}

double randnum() {
    return ((double) rand()) / RAND_MAX;
}

color HSVtoRGB(int H, double S, double V) {
	double C = S * V;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	// double m = V - C;
	color c;
	if(H >= 0 && H < 60) {
        c = {C, X, 0};
	}else if(H >= 60 && H < 120) {
        c = {X, C, 0};	
	}else if(H >= 120 && H < 180) {
        c = {0, C, X};
	}else if(H >= 180 && H < 240) {
        c = {0, X, C};
	}else if(H >= 240 && H < 300) {
        c = {X, 0, C};
	}else {
        c = {C, 0, X};
	}
    return c;
}

color randcol() {
    // color c = {randnum(), randnum(), randnum()};
    return HSVtoRGB(randnum() * 360, 1.f, 1.f);
}

pt randpt(double minx, double maxx, double miny, double maxy) {
    return pt(randnum() * (maxx - minx) + minx, randnum() * (maxy - miny) + miny);
}

void init() {
    glClearColor(COLOR(backgroundCol), 1.f);
	glPointSize(4.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0);
    srand(time(NULL));

    // generate n random colors
    for(int i = 0; i < N; i++) {
        cols.push_back(randcol());
    }

    // generate n random points
    points.clear();
    for(int i = 0; i < N; i++) {
        points.push_back(randpt(0.0, 1.0, 0.0, 1.0));
        for(int s = 0; s < SEED; s++) {
            hilbert[s].emplace_back(p2dseed(s, PREC, points[i]), i);
        }
    }
    for(int s = 0; s < SEED; s++) {
        sort(hilbert[s].begin(), hilbert[s].end());
    }
}

void rect(pt p, double width, double height) {
    glVertex2f(p.x, p.y);
    glVertex2f(p.x + width, p.y);
    glVertex2f(p.x + width, p.y + height);
    glVertex2f(p.x, p.y + height);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_QUADS);
    {
        for(int i = 0; i < PREC; i++) {
            for(int j = 0; j < PREC; j++) {
                pt p = (1.0 / PREC) * pt(i, j);
                int k = nearest(p);
                glColor3f(COLOR(cols[k]));
                rect(p, 1.f / PREC, 1.f / PREC);
            }
        }
    }
    glEnd();

    // draw points
    glBegin(GL_POINTS);
    {
        for(int i = 0; i < N; i++) {
            auto [r, g, b] = cols[i];
            glColor3f(r / 2, g / 2, b / 2);
            glVertex2f(points[i].x, points[i].y);
        }
    }
    glEnd();

    glFlush();
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    // window initialization
	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - width) / 2,
                           (glutGet(GLUT_SCREEN_HEIGHT) - height) / 2);

	// create window
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("Hilbert Voronoi Diagram");

	init();
	// glutMouseFunc(mouse_press);
	// glutPassiveMotionFunc(mouse_move);
	// glutMotionFunc(mouse_drag);
	// glutKeyboardFunc(key_press);
	// glutKeyboardUpFunc(key_release);
	// glutReshapeFunc(resize);
	// glutIdleFunc(idle);

	glutDisplayFunc(display);

	glutMainLoop();
}