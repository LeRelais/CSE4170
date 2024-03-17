#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define TO_RADIAN		0.017453292519943296 

int rightbuttonpressed = 0;
int leftbuttonpressed = 0, center_selected = 0;

float r, g, b; // Background color
float px, py, qx, qy; // Line (px, py) --> (qx, qy)
int n_object_points = 6;
float object[6][2], object_center_x, object_center_y;
float polygon[8][2], polygon_center_x, polygon_center_y;
float rotation_angle_in_degree;
int window_width, window_height;

int tmp_x, tmp_y;
float tmp_m[3][3];
float pt_m[3][1];
bool picked_blue = false;

struct point {
	float y;
	float x;
};

point pts;

point matrixmult(float** m1, float m2[][1]) {
	point cal;

	float m3[3][1] = { 0.0f, };

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 1; j++) {
			for (int k = 0; k < 3; k++) {
				m3[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}

	cal.x = m3[0][0];
	cal.y = m3[1][0];

	return cal;
}

float** matrixmult2(float** m1, float** m2) {
	float** arr;
	arr = (float**)calloc(3, sizeof(float*));
	for (int i = 0; i < 3; i++) {
		arr[i] = (float*)calloc(3, sizeof(float));
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				arr[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}

	return arr;
}

float** create_Matrix_R(float angle) {
	float** arr;
	arr = (float**)calloc(3, sizeof(float*));
	for (int i = 0; i < 3; i++) {
		arr[i] = (float*)calloc(3, sizeof(float));
	}

	arr[0][0] = cos(angle);
	arr[0][1] = -1 * sin(angle);
	arr[1][0] = sin(angle);
	arr[1][1] = cos(angle);
	arr[2][2] = 1;

	return arr;
}

float** create_Matrix_S(float a, float x, float y, int con) {
	float** arr;
	arr = (float**)calloc(3, sizeof(float*));
	for (int i = 0; i < 3; i++) {
		arr[i] = (float*)calloc(3, sizeof(float));
	}
	if (con == 1) {
		arr[0][0] = ((a / 250) * x + x) / x;
		arr[1][1] = ((a / 250) * y + y) / y;
		arr[2][2] = 1;
	}
	else {
		arr[0][0] = 1 / (((a / 250) * x + x) / x);
		arr[1][1] = 1 / (((a / 250) * y + y) / y);
		arr[2][2] = 1;
	}

	return arr;
}

float** create_Matrix_shearx(float y, float x) {                 //추가구현
	float** arr;
	arr = (float**)calloc(3, sizeof(float*));
	for (int i = 0; i < 3; i++) {
		arr[i] = (float*)calloc(3, sizeof(float));
	}

	arr[0][0] = 1;
	arr[1][1] = 1;
	arr[0][1] = x / 250;
	arr[2][2] = 1;

	return arr;
}

float** create_Matrix_sheary(float y, float x) {                 //추가구현
	float** arr;
	arr = (float**)calloc(3, sizeof(float*));
	for (int i = 0; i < 3; i++) {
		arr[i] = (float*)calloc(3, sizeof(float));
	}

	arr[0][0] = 1;
	arr[1][1] = 1;
	arr[1][0] = y / 250;
	arr[2][2] = 1;

	return arr;
}

float** create_Reflect(int type) {
	float** arr;
	arr = (float**)calloc(3, sizeof(float*));
	for (int i = 0; i < 3; i++) {
		arr[i] = (float*)calloc(3, sizeof(float));
	}

	if (type == 1) {
		arr[0][0] = -1;
		arr[1][1] = -1;
		arr[2][2] = 1;
	}
	else {
		arr[0][0] = -1;
		arr[1][1] = 1;
		arr[2][2] = 1;
	}

	return arr;
}

float** create_Matrix_T(float y, float x) {
	float** arr;
	arr = (float**)calloc(3, sizeof(float*));
	for (int i = 0; i < 3; i++) {
		arr[i] = (float*)calloc(3, sizeof(float));
	}
	arr[0][0] = 1;
	arr[1][1] = 1;
	arr[0][2] = x / 250;
	arr[1][2] = y / 250;
	arr[2][2] = 1;

	return arr;
}

void create_Point(float x, float y) {
	pt_m[0][0] = x;
	pt_m[1][0] = y;
	pt_m[2][0] = 1;
}

void draw_axes() {
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.975f, 0.025f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.975f, -0.025f);
	glVertex2f(1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.025f, 0.975f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(-0.025f, 0.975f);
	glVertex2f(0.0f, 1.0f);
	glEnd();
	glLineWidth(1.0f);

	glPointSize(7.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glEnd();
	glPointSize(1.0f);
}
void draw_line(float px, float py, float qx, float qy) {
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(px, py);
	glVertex2f(qx, qy);
	glEnd();
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(px, py);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(qx, qy);
	glEnd();
	glPointSize(1.0f);

}

void draw_object(void) {
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 1.0f, 0.0f);
	for (int i = 0; i < 6; i++)
		glVertex2f(object[i][0], object[i][1]);
	glEnd();
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 6; i++)
		glVertex2f(object[i][0], object[i][1]);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(object_center_x, object_center_y);
	glEnd();
	glPointSize(1.0f);
}

void draw_polygon(void) {
	glBegin(GL_LINE_LOOP);
	glColor3f(0.21f, 0.3f, 0.7f);
	for (int i = 0; i < 8; i++)
		glVertex2f(polygon[i][0], polygon[i][1]);
	glEnd();
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glColor3f(0.76f, 0.2f, 0.45f);
	for (int i = 0; i < 8; i++)
		glVertex2f(polygon[i][0], polygon[i][1]);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(polygon_center_x, polygon_center_y);
	glEnd();
	glPointSize(1.0f);
}

void display(void) {
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	draw_axes();
	draw_line(px, py, qx, qy);
	draw_object();
	draw_polygon();
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 'r':
		r = 1.0f; g = b = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'g':
		g = 1.0f; r = b = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'b':
		b = 1.0f; r = g = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 's':
		r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
}

void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		r -= 0.1f;
		if (r < 0.0f) r = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		r += 0.1f;
		if (r > 1.0f) r = 1.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		g -= 0.1f;
		if (g < 0.0f) g = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		g += 0.1f;
		if (g > 1.0f) g = 1.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	}
}

int prevx, prevy;
void mousepress(int button, int state, int x, int y) {

	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		rightbuttonpressed = 1;

	}
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)) {
		rightbuttonpressed = 0;
		prevx = x;
		prevy = y;
	}
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		leftbuttonpressed = 1;
		float mid_x = ((float)window_width / 2);
		float mid_y = (float)window_height / 2;

		float t_x = (float)x - mid_x;
		float t_y = (float)y - mid_y;

		if ((t_x / 250 - px >= -0.005f && t_x / 250 - px <= 0.005f) || (t_y / 250 - py >= -0.005f && t_y / 250 - py <= 0.005f))
		{
			//fprintf(stdout, "blue is picked\n");
			picked_blue = true;
		}

	}
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)) {
		leftbuttonpressed = 0;
		picked_blue = false;
		prevx = x;
		prevy = y;
	}

	else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN)) {
		center_selected = 1;
		//fprintf(stdout, "center is pressed\n");
	}
	else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_UP)) {
		center_selected = 0;
	}
}

void mousewheel(int button, int dir, int x, int y) {
	//Translation 행렬로 구현 해야함
	//float tmp_py = py;
	//float tmp_px = px;

	//py -= tmp_py;
	//px -= tmp_px;
	//qy -= tmp_py;
	//qx -= tmp_px;

	point pivot;
	pivot.x = px;
	pivot.y = py;

	create_Point(qx, qy);
	if (dir > 0)
	{
		float** r = create_Matrix_R(TO_RADIAN * rotation_angle_in_degree);
		float** arr = create_Matrix_T(-pivot.y * 250, -pivot.x * 250);
		float** arr1 = create_Matrix_T(pivot.y * 250, pivot.x * 250);

		pts = matrixmult(arr, pt_m);
		create_Point(pts.x, pts.y);
		pts = matrixmult(r, pt_m);
		create_Point(pts.x, pts.y);
		pts = matrixmult(arr1, pt_m);
		qx = pts.x;
		qy = pts.y;

	}
	else {
		float** r = create_Matrix_R(-1 * TO_RADIAN * rotation_angle_in_degree);
		float** arr = create_Matrix_T(-pivot.y * 250, -pivot.x * 250);
		float** arr1 = create_Matrix_T(pivot.y * 250, pivot.x * 250);

		pts = matrixmult(arr, pt_m);
		create_Point(pts.x, pts.y);
		pts = matrixmult(r, pt_m);
		create_Point(pts.x, pts.y);
		pts = matrixmult(arr1, pt_m);
		qx = pts.x;
		qy = pts.y;
	}

	//py += tmp_py;
	//px += tmp_px;
	//qy += tmp_py;
	//qx += tmp_px;

	glutPostRedisplay();
}

void mousemove(int x, int y) {
	int mod = glutGetModifiers();
	if (leftbuttonpressed) {
		if (mod == GLUT_ACTIVE_SHIFT) {
			//fprintf(stdout, "mouse position :  %d    %d\n", y, x);
			//fprintf(stdout, "mouse position :  %d    %f\n", y, ((float)x-mid_x) / 250);
			//fprintf(stdout, "activated shift\n\n");

			float dx = (float)((x - prevx));
			float dy = (float)((y - prevy)) * -1;

			float** arr = create_Matrix_T(dy, dx);
			create_Point(px, py);
			if (picked_blue) {
				pts = matrixmult(arr, pt_m);
				px = pts.x;
				py = pts.y;
			}
		}
		else
			picked_blue = false;

		if (mod == GLUT_ACTIVE_ALT) {
			float dx = (float)((x - prevx));
			float dy = (float)((y - prevy)) * -1;

			float** shearmatrixx = create_Matrix_shearx(dy, dx);
			float** shearmatrixy = create_Matrix_sheary(dy, dx);

			for (int i = 0; i < 8; i++) {
				create_Point(polygon[i][0], polygon[i][1]);
				pts = matrixmult(shearmatrixx, pt_m);
				create_Point(pts.x, pts.y);
				pts = matrixmult(shearmatrixy, pt_m);
				polygon[i][0] = pts.x;
				polygon[i][1] = pts.y;
			}

			create_Point(polygon_center_x, polygon_center_y);
			pts = matrixmult(shearmatrixx, pt_m);
			create_Point(pts.x, pts.y);
			pts = matrixmult(shearmatrixy, pt_m);
			polygon_center_x = pts.x;
			polygon_center_y = pts.y;
		}

		glutPostRedisplay();
	}

	if (rightbuttonpressed) {
		if (mod == GLUT_ACTIVE_ALT) {
			float dx = (float)((x - prevx));
			float dy = (float)((y - prevy)) * -1;

			float** arr = create_Matrix_T(dy, dx);

			//fprintf(stdout, "y : %d x : %d ny : %d  nx : %d  dy : %d  dx : %d\n\n", y, x, tmp_y, tmp_x, dy, dx);
			for (int i = 0; i < 6; i++) {
				create_Point(object[i][0], object[i][1]);
				pts = matrixmult(arr, pt_m);
				//fprintf(stdout, "%f   %f\n", pts.y, pts.x);
				object[i][0] = pts.x;
				object[i][1] = pts.y;
			}

			create_Point(object_center_x, object_center_y);
			pts = matrixmult(arr, pt_m);
			object_center_x = pts.x;
			object_center_y = pts.y;

		}

		else if (mod == GLUT_ACTIVE_CTRL) {
			//Translation 행렬로 구현 해야함

			//fprintf(stdout, "activated control\n\n");
			float dx = (float)((x - prevx));

			point center;
			center.y = object_center_y;
			center.x = object_center_x;


			if (dx == 0) {
				for (int i = 0; i < 6; i++) {
					object[i][0] = object[i][0];
					object[i][1] = object[i][1];
				}
			}
			else if (dx > 0) {
				float** arr = create_Matrix_T(-center.y * 250, -center.x * 250);
				float** arr1 = create_Matrix_T(center.y * 250, center.x * 250);

				for (int i = 0; i < 6; i++) {
					create_Point(object[i][0], object[i][1]);
					pts = matrixmult(arr, pt_m);

					object[i][0] = pts.x;
					object[i][1] = pts.y;
				}
				for (int i = 0; i < 6; i++) {
					float** s = create_Matrix_S(dx, object[i][0], object[i][1], 1);
					create_Point(object[i][0], object[i][1]);
					pts = matrixmult(s, pt_m);
					//fprintf(stdout, "%f   %f\n", pts.y, pts.x);
					create_Point(pts.x, pts.y);
					pts = matrixmult(arr1, pt_m);
					object[i][0] = pts.x;
					object[i][1] = pts.y;
				}
			}
			else if (dx < 0) {
				float** arr = create_Matrix_T(-center.y * 250, -center.x * 250);
				float** arr1 = create_Matrix_T(center.y * 250, center.x * 250);

				for (int i = 0; i < 6; i++) {
					create_Point(object[i][0], object[i][1]);
					pts = matrixmult(arr, pt_m);

					object[i][0] = pts.x;
					object[i][1] = pts.y;
				}
				for (int i = 0; i < 6; i++) {
					float** s = create_Matrix_S(-dx, object[i][0], object[i][1], 0);
					create_Point(object[i][0], object[i][1]);
					pts = matrixmult(s, pt_m);
					//fprintf(stdout, "%f   %f\n", pts.y, pts.x);
					create_Point(pts.x, pts.y);
					pts = matrixmult(arr1, pt_m);
					object[i][0] = pts.x;
					object[i][1] = pts.y;
				}
			}
		}
	}

	if (center_selected) {
		float dy = (float)((y - prevy));
		point center;
		center.y = polygon_center_y;
		center.x = polygon_center_x;

		if (dy == 0) {
			for (int i = 0; i < 8; i++) {
				polygon[i][0] = polygon[i][0];
				polygon[i][1] = polygon[i][1];
			}
		}
		else if (dy > 0) {
			float** arr = create_Matrix_T(-center.y * 250, -center.x * 250);
			float** r = create_Matrix_R(TO_RADIAN * rotation_angle_in_degree);
			//float** arr1 = create_Matrix_T(center.y * 250, center.x * 250);

			for (int i = 0; i < 8; i++) {
				create_Point(polygon[i][0], polygon[i][1]);
				pts = matrixmult(arr, pt_m);

				polygon[i][0] = pts.x;
				polygon[i][1] = pts.y;
			}


			create_Point(polygon_center_x, polygon_center_y);
			//pts = matrixmult(arr, pt_m);
			//fprintf(stdout, "%f   %f\n", pts.y, pts.x);
			//create_Point(pts.x, pts.y);
			pts = matrixmult(r, pt_m);
			create_Point(pts.x, pts.y);
			//pts = matrixmult(arr1, pt_m);
			polygon_center_x = pts.x;
			polygon_center_y = pts.y;

			float** arr1 = create_Matrix_T(polygon_center_y * 250, polygon_center_x * 250);
			for (int i = 0; i < 8; i++) {
				float** s = create_Matrix_S(dy, polygon[i][0], polygon[i][1], 1);
				create_Point(polygon[i][0], polygon[i][1]);
				pts = matrixmult(s, pt_m);
				//fprintf(stdout, "%f   %f\n", pts.y, pts.x);
				create_Point(pts.x, pts.y);
				pts = matrixmult(r, pt_m);
				create_Point(pts.x, pts.y);
				pts = matrixmult(arr1, pt_m);
				polygon[i][0] = pts.x;
				polygon[i][1] = pts.y;
			}
		}
		else {
			float** arr = create_Matrix_T(-center.y * 250, -center.x * 250);
			float** r = create_Matrix_R(TO_RADIAN * rotation_angle_in_degree);
			//float** arr1 = create_Matrix_T(center.y * 250, center.x * 250);

			for (int i = 0; i < 8; i++) {
				create_Point(polygon[i][0], polygon[i][1]);
				pts = matrixmult(arr, pt_m);

				polygon[i][0] = pts.x;
				polygon[i][1] = pts.y;
			}


			create_Point(polygon_center_x, polygon_center_y);
			//pts = matrixmult(arr, pt_m);
			//fprintf(stdout, "%f   %f\n", pts.y, pts.x);
			//create_Point(pts.x, pts.y);
			pts = matrixmult(r, pt_m);
			create_Point(pts.x, pts.y);
			//pts = matrixmult(arr1, pt_m);
			polygon_center_x = pts.x;
			polygon_center_y = pts.y;

			float** arr1 = create_Matrix_T(polygon_center_y * 250, polygon_center_x * 250);
			for (int i = 0; i < 8; i++) {
				float** s = create_Matrix_S(-dy, polygon[i][0], polygon[i][1], 0);
				create_Point(polygon[i][0], polygon[i][1]);
				pts = matrixmult(s, pt_m);
				//fprintf(stdout, "%f   %f\n", pts.y, pts.x);
				create_Point(pts.x, pts.y);
				pts = matrixmult(r, pt_m);
				create_Point(pts.x, pts.y);
				pts = matrixmult(arr1, pt_m);
				polygon[i][0] = pts.x;
				polygon[i][1] = pts.y;
			}
		}
	}

	glutPostRedisplay();

	prevx = x;
	prevy = y;
}

void passivemousemove(int x, int y) {
	prevx = x;
	prevy = y;
}

void reshape(int width, int height) {
	// DO NOT MODIFY THIS FUNCTION!!!
	window_width = width, window_height = height;
	glViewport(0.0f, 0.0f, window_width, window_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-window_width / 500.0f, window_width / 500.0f, -window_height / 500.0f, window_height / 500.0f, -1.0f, 1.0f);

	glutPostRedisplay();
}


void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMouseWheelFunc(mousewheel);
	glutMotionFunc(mousemove);
	glutPassiveMotionFunc(passivemousemove);
	glutReshapeFunc(reshape);
	glutCloseFunc(close);
}

void initialize_renderer(void) {
	register_callbacks();
	r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f; // Background color = Salmon
	px = -0.5f, py = 0.60f, qx = -0.45f, qy = -0.10f;
	rotation_angle_in_degree = 1.0f; // 1 degree

	float sq_cx = 0.55f, sq_cy = -0.45f, sq_side = 0.35f;
	object[0][0] = 0.4 * sq_cx + sq_side;
	object[0][1] = sq_cy + sq_side;
	object[1][0] = sq_cx + 0.5 * sq_side;
	object[1][1] = sq_cy + 1.5 * sq_side;
	object[2][0] = sq_cx - 1.3 * sq_side;
	object[2][1] = sq_cy + sq_side;
	object[3][0] = sq_cx - sq_side;
	object[3][1] = sq_cy - sq_side;
	object[4][0] = sq_cx - 0.89 * sq_side;
	object[4][1] = 0.45 * sq_cy - 0.5 * sq_side;
	object[5][0] = sq_cx + sq_side;
	object[5][1] = sq_cy - sq_side;
	object_center_x = object_center_y = 0.0f;
	for (int i = 0; i < n_object_points; i++) {
		object_center_x += object[i][0];
		object_center_y += object[i][1];
	}
	object_center_x /= n_object_points;
	object_center_y /= n_object_points;

	sq_cx = -0.4f, sq_cy = 1.5f, sq_side = 0.76f;
	polygon[0][0] = 0.2 + sq_cx * 0.2 + sq_side;
	polygon[0][1] = sq_cy * 0.34 + sq_side;
	polygon[1][0] = -0.6 * sq_cx - sq_side;
	polygon[1][1] = -0.67 * sq_cy;
	polygon[2][0] = -sq_side - sq_cx;
	polygon[2][1] = sq_cy * 0.32 * sq_side;
	polygon[3][0] = -0.5 * sq_cx - sq_side;
	polygon[3][1] = sq_cy - sq_side;
	polygon[4][0] = -0.32 * sq_cx - 0.5 * sq_side;
	polygon[4][1] = 1.2 * sq_cy - 1.6 * sq_side;
	polygon[5][0] = 0.1 * sq_cx;
	polygon[5][1] = 0.7 * sq_cy + 0.5 * sq_side;
	polygon[6][0] = -0.1 * sq_cx + 0.2 * sq_side;
	polygon[6][1] = 0.7 * sq_cy - 0.5 * sq_side;
	polygon[7][0] = 0.5 * sq_cx;
	polygon[7][1] = -0.1 * sq_cx + 0.1 * sq_side;

	polygon_center_x = polygon_center_y = 0.0f;
	for (int i = 0; i < 8; i++) {
		polygon_center_x += polygon[i][0];
		polygon_center_y += polygon[i][1];
	}
	polygon_center_x /= 8;
	polygon_center_y /= 8;
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 4
void main(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple 2D Transformations";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'r', 'g', 'b', 's', 'q'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: SHIFT/L-click and move, ALT/R-click and move, CTRL/R-click and move",
		"    - Wheel used: up and down scroll"
		"    - Other operations: window size change"
	};

	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); // <-- Be sure to use this profile for this example code!
	//	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(750, 750);
	glutInitWindowPosition(500, 200);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	// glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT); // default
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutMainLoop();
	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");
}