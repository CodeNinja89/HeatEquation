#ifndef INTERACTIONS_H
#define INTERACTIONS_H

#define W 640
#define H 640

#define DT 1.f // source intensity increment
#define TITLE_STRING "TEMPERATURE VISUALIZATION"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

float *d_temp = 0;
int iterationCount = 0;
BC bc = {W/2, H/2, W/10.f, 212.f, 70.f, 0.f};

void keyboard(unsigned char key, int x, int y) {
	if(key == 'q') bc.t_s += DT; // increment source temperature
	if(key == 'w') bc.t_s -= DT; // decrement source temperature
	if(key == 'e') bc.t_a += DT; // increment ambient temperature
	if(key == 'r') bc.t_a -= DT; // decrement ambient temperature
	if(key == 't') bc.t_g += DT; // increment ground temperature
	if(key == 'y') bc.t_g -= DT; // decrement ground temperature
	if(key == 'o') bc.rad += DT;
	if(key == 'p') bc.rad = MAX(0.f, bc.rad - DT);
	if(key == 'z') resetTemperature(d_temp, W, H, bc);
	if(key == 27) exit(0);
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	bc.x = x;
	bc.y = y;
	glutPostRedisplay();
}

void idle() {
	++iterationCount;
	glutPostRedisplay();
}

#endif