#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>


#include<iostream>
#include<random>
#include<time.h>

using namespace std;

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0



#define AIRPLANE_ROTATION_RADIUS 300.0f

unsigned int timestamp = 1, timestamp_house = 1;
unsigned int timestamp_sword = 1, timestamp_water;
int house_ampli = 1;
float airplane_s_factor;
#define airplane_num  10;
float tmp_x = 0, tmp_y = 0;
using namespace std;
int housenum = 1;

int prevx, prevy;
int tx, ty;
int waterx, watery;
int watercount = 0;
int rightbuttonpressed = 0;
int leftbuttonpressed = 0;
int centerpressed = 0; 

int win_width = 0, win_height = 0; 
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

void prepare_axes(void) { // Draw axes in their MC.
	axes[0][0] = -win_width / 2.5f; axes[0][1] = 0.0f;
	axes[1][0] = win_width / 2.5f; axes[1][1] = 0.0f;
	axes[2][0] = 0.0f; axes[2][1] = -win_height / 2.5f;
	axes[3][0] = 0.0f; axes[3][1] = win_height / 2.5f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	axes[0][0] = -win_width / 2.5f; axes[1][0] = win_width / 2.5f; 
	axes[2][1] = -win_height / 2.5f;
	axes[3][1] = win_height / 2.5f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f; 
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f; 
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) { // Draw line in its MC.
	// y = x - win_height/4
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}



class juice {
public:
	float x1, y1, x2, y2;
};



//draw cocktail
#define COCKTAIL_NECK 0
#define COCKTAIL_LIQUID 1
#define COCKTAIL_REMAIN 2
#define COCKTAIL_STRAW 3
#define COCKTAIL_DECO 4

GLfloat neck[6][2] = { { -6.0, -12.0 },{ -6.0, -11.0 },{ -1.0, 0.0 },{ 1.0, 0.0 },{ 6.0, -11.0 },{ 6.0, -12.0 } };
GLfloat liquid[6][2] = { { -1.0, 0.0 },{ -9.0, 4.0 },{ -12.0, 7.0 },{ 12.0, 7.0 },{ 9.0, 4.0 },{ 1.0, 0.0 } };
GLfloat remain[4][2] = { { -12.0, 7.0 },{ -12.0, 10.0 },{ 12.0, 10.0 },{ 12.0, 7.0 } };
GLfloat straw[4][2] = { { 7.0, 7.0 },{ 12.0, 12.0 },{ 14.0, 12.0 },{ 9.0, 7.0 } };
GLfloat deco[8][2] = { { 12.0, 12.0 },{ 10.0, 14.0 },{ 10.0, 16.0 },{ 12.0, 18.0 },{ 14.0, 18.0 },{ 16.0, 16.0 },{ 16.0, 14.0 },{ 14.0, 12.0 } };

GLfloat cocktail_color[5][3] = {
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 0 / 255.0f, 63 / 255.0f, 122 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 191 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 218 / 255.0f, 165 / 255.0f, 32 / 255.0f }
};

GLuint VBO_cocktail, VAO_cocktail;
void prepare_cocktail() {
	GLsizeiptr buffer_size = sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw)
		+ sizeof(deco);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(neck), neck);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck), sizeof(liquid), liquid);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid), sizeof(remain), remain);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain), sizeof(straw), straw);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw),
		sizeof(deco), deco);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cocktail);
	glBindVertexArray(VAO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cocktail() {
	glBindVertexArray(VAO_cocktail);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_NECK]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_LIQUID]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_REMAIN]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_STRAW]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_DECO]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 8);

	glBindVertexArray(0);
}





#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6
GLfloat big_wing[6][2] = { { 0.0, 0.0 }, { -20.0, 15.0 }, { -20.0, 20.0 }, { 0.0, 23.0 }, { 20.0, 20.0 }, { 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 }, { -11.0, -12.0 }, { -12.0, -7.0 }, { 0.0, -10.0 }, { 12.0, -7.0 }, { 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 }, { -6.0, 0.0 }, { -6.0, 22.0 }, { 6.0, 22.0 }, { 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 }, { -7.0, 24.0 }, { -7.0, 21.0 }, { 7.0, 21.0 }, { 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 }, { -18.0, 3.0 }, { -16.0, 10.0 }, { -18.0, 20.0 }, { -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 }, { 18.0, 3.0 }, { 16.0, 10.0 }, { 18.0, 20.0 }, { 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };
GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLuint VBO_airplane, VAO_airplane;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1)+sizeof(sidewinder2)+sizeof(center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1)+sizeof(sidewinder2), sizeof(center), center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { // Draw airplane in its MC.
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}





//house
#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

GLfloat roof[3][2] = { { -12.0, 0.0 },{ 0.0, 12.0 },{ 12.0, 0.0 } };
GLfloat house_body[4][2] = { { -12.0, -14.0 },{ -12.0, 0.0 },{ 12.0, 0.0 },{ 12.0, -14.0 } };
GLfloat chimney[4][2] = { { 6.0, 6.0 },{ 6.0, 14.0 },{ 10.0, 14.0 },{ 10.0, 2.0 } };
GLfloat door[4][2] = { { -8.0, -14.0 },{ -8.0, -8.0 },{ -4.0, -8.0 },{ -4.0, -14.0 } };
GLfloat window[4][2] = { { 4.0, -6.0 },{ 4.0, -2.0 },{ 8.0, -2.0 },{ 8.0, -6.0 } };

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};

GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door)
		+ sizeof(window);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door),
		sizeof(window), window);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house() {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_ROOF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_CHIMNEY]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}






#define CAKE_FIRE 0
#define CAKE_CANDLE 1
#define CAKE_BODY 2
#define CAKE_BOTTOM 3
#define CAKE_DECORATE 4

GLfloat cake_fire[4][2] = { { -5.5, 30.0 },{ -0.5, 13.0 },{ 0.5, 13.0 },{ 5.5, 30.0 } };
GLfloat cake_candle[4][2] = { { -5.0, 8.0 } ,{ -5.0, 13.0 },{ 5.0, 13.0 },{ 5.0, 8.0 } };
GLfloat cake_body[4][2] = { { 8.0, 5.0 },{ -8.0, 5.0 } ,{ -8.0, 8.0 },{ 8.0, 8.0 } };
GLfloat cake_bottom[4][2] = { { -10.0, 1.0 },{ -10.0, 15.0 },{ 10.0, 15.0 },{ 10.0, 1.0 } };
GLfloat cake_decorate[4][2] = { { -10.0, 0.0 },{ -10.0, 1.0 },{ 10.0, 1.0 },{ 10.0, 0.0 } };

GLfloat cake_color[5][3] = {
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

GLuint VBO_cake, VAO_cake;

void prepare_cake() {
	int size = sizeof(cake_fire);
	GLsizeiptr buffer_size = sizeof(cake_fire) * 5;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, size, cake_fire);
	glBufferSubData(GL_ARRAY_BUFFER, size, size, cake_candle);
	glBufferSubData(GL_ARRAY_BUFFER, size * 2, size, cake_body);
	glBufferSubData(GL_ARRAY_BUFFER, size * 3, size, cake_bottom);
	glBufferSubData(GL_ARRAY_BUFFER, size * 4, size, cake_decorate);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cake);
	glBindVertexArray(VAO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cake() {
	glBindVertexArray(VAO_cake);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_FIRE]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_CANDLE]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_DECORATE]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glBindVertexArray(0);
}






#define WATER_BODY 0
#define WATER_BODY2 1
#define WATER_HEAD 2
#define WATER_HEAD2 3
#define WATER_IN 4
#define WATER_DOWN 5
#define WATER_BODY_IN 6

GLfloat water_body[4][2] = { { -2.0, 0.0 },{ -2.0, -4.0 },{ 2.0, -4.0 },{ 2.0, 0.0 } };
GLfloat water_body2[4][2] = { { -2.0, -4.0 },{ -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -4.0 } };
GLfloat water_head[4][2] = { { 0.0, 0.0 },{ 0.0, 0.0 } ,{ 0.0, 0.0 },{ 0.0, 0.0 } };
GLfloat water_head2[3][2] = { { 0.0, 0.0 },{ 0.0, 0 } ,{ 0, 0.0 } };
GLfloat water_in[4][2] = { { -0.3, 0.7 },{ -0.3, 15.3 } ,{ 0.3, 15.3 },{ 0.3, 0.7 } };
GLfloat water_down[4][2] = { { -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -8.0 },{ -2.0, -8.0 } };
GLfloat water_body_in[4][2] = { { 0.0, -1.0 } ,{ 1.0, -2.732 },{ 0.0, -4.464 },{ -1.0, -2.732 } };

GLfloat water_color[7][3] = {
	{ 0 / 255.0f, 0 / 255.0f, 255 / 255.0f },
{ 0 / 255.0f, 0 / 255.0f, 225 / 255.0f },
{  0 / 255.0f, 0 / 255.0f, 225 / 255.0f },
{  0 / 255.0f, 0 / 255.0f, 225 / 255.0f },
{  0 / 255.0f, 0 / 255.0f, 225 / 255.0f},
{  0 / 255.0f, 0 / 255.0f, 225 / 255.0f },
{  0 / 255.0f, 0 / 255.0f, 225 / 255.0f }
};

GLfloat water_color2[7][3] = {
	{ 210 / 255.0f, 0 / 255.0f, 0/ 255.0f },
{ 210 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 210 / 255.0f, 0 / 255.0f, 0/ 255.0f},
{ 210 / 255.0f, 0 / 255.0f, 0/ 255.0f },
{ 210 / 255.0f, 0 / 255.0f, 0/ 255.0f},
{ 210 / 255.0f, 0 / 255.0f, 0/ 255.0f },
{ 210 / 255.0f, 0 / 255.0f, 0/ 255.0f }
};

GLuint VBO_water, VAO_water;

void prepare_water() {
	GLsizeiptr buffer_size = sizeof(water_body) + sizeof(water_body2) + sizeof(water_head) + sizeof(water_head2) + sizeof(water_in) + sizeof(water_down) + sizeof(water_body_in);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_water);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_water);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(water_body), water_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body), sizeof(water_body2), water_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body) + sizeof(water_body2), sizeof(water_head), water_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body) + sizeof(water_body2) + sizeof(water_head), sizeof(water_head2), water_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body) + sizeof(water_body2) + sizeof(water_head) + sizeof(water_head2), sizeof(water_in), water_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body) + sizeof(water_body2) + sizeof(water_head) + sizeof(water_head2) + sizeof(water_in), sizeof(water_down), water_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body) + sizeof(water_body2) + sizeof(water_head) + sizeof(water_head2) + sizeof(water_in) + sizeof(water_down), sizeof(water_body_in), water_body_in);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_water);
	glBindVertexArray(VAO_water);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_water);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_water() {
	glBindVertexArray(VAO_water);

	glUniform3fv(loc_primitive_color, 1, water_color[WATER_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, water_color[WATER_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, water_color[WATER_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, water_color[WATER_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);			 
													 
	glUniform3fv(loc_primitive_color, 1, water_color[WATER_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);			 
													 
	glUniform3fv(loc_primitive_color, 1, water_color[WATER_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);			 
													 
	glUniform3fv(loc_primitive_color, 1, water_color[WATER_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}

void prepare_water2() {
	GLsizeiptr buffer_size = sizeof(water_body) + sizeof(water_body2) + sizeof(water_head) + sizeof(water_head2) + sizeof(water_in) + sizeof(water_down) + sizeof(water_body_in);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_water);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_water);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(water_body), water_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body), sizeof(water_body2), water_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body) + sizeof(water_body2), sizeof(water_head), water_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body) + sizeof(water_body2) + sizeof(water_head), sizeof(water_head2), water_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body) + sizeof(water_body2) + sizeof(water_head) + sizeof(water_head2), sizeof(water_in), water_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body) + sizeof(water_body2) + sizeof(water_head) + sizeof(water_head2) + sizeof(water_in), sizeof(water_down), water_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(water_body) + sizeof(water_body2) + sizeof(water_head) + sizeof(water_head2) + sizeof(water_in) + sizeof(water_down), sizeof(water_body_in), water_body_in);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_water);
	glBindVertexArray(VAO_water);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_water);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_water2() {
	glBindVertexArray(VAO_water);

	glUniform3fv(loc_primitive_color, 1, water_color2[WATER_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, water_color2[WATER_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, water_color2[WATER_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, water_color2[WATER_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);

	glUniform3fv(loc_primitive_color, 1, water_color2[WATER_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glUniform3fv(loc_primitive_color, 1, water_color2[WATER_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);

	glUniform3fv(loc_primitive_color, 1, water_color2[WATER_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}




//shirt
#define SHIRT_LEFT_BODY 0
#define SHIRT_RIGHT_BODY 1
#define SHIRT_LEFT_COLLAR 2
#define SHIRT_RIGHT_COLLAR 3
#define SHIRT_FRONT_POCKET 4
#define SHIRT_BUTTON1 5
#define SHIRT_BUTTON2 6
#define SHIRT_BUTTON3 7
#define SHIRT_BUTTON4 8
GLfloat left_body[6][2] = { { 0.0, -9.0 },{ -8.0, -9.0 },{ -11.0, 8.0 },{ -6.0, 10.0 },{ -3.0, 7.0 },{ 0.0, 9.0 } };
GLfloat right_body[6][2] = { { 0.0, -9.0 },{ 0.0, 9.0 },{ 3.0, 7.0 },{ 6.0, 10.0 },{ 11.0, 8.0 },{ 8.0, -9.0 } };
GLfloat left_collar[4][2] = { { 0.0, 9.0 },{ -3.0, 7.0 },{ -6.0, 10.0 },{ -4.0, 11.0 } };
GLfloat right_collar[4][2] = { { 0.0, 9.0 },{ 4.0, 11.0 },{ 6.0, 10.0 },{ 3.0, 7.0 } };
GLfloat front_pocket[6][2] = { { 5.0, 0.0 },{ 4.0, 1.0 },{ 4.0, 3.0 },{ 7.0, 3.0 },{ 7.0, 1.0 },{ 6.0, 0.0 } };
GLfloat button1[3][2] = { { -1.0, 6.0 },{ 1.0, 6.0 },{ 0.0, 5.0 } };
GLfloat button2[3][2] = { { -1.0, 3.0 },{ 1.0, 3.0 },{ 0.0, 2.0 } };
GLfloat button3[3][2] = { { -1.0, 0.0 },{ 1.0, 0.0 },{ 0.0, -1.0 } };
GLfloat button4[3][2] = { { -1.0, -3.0 },{ 1.0, -3.0 },{ 0.0, -4.0 } };

GLfloat shirt_color[9][3] = {
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f }
};

GLuint VBO_shirt, VAO_shirt;
void prepare_shirt() {
	GLsizeiptr buffer_size = sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3) + sizeof(button4);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(left_body), left_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body), sizeof(right_body), right_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body), sizeof(left_collar), left_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar), sizeof(right_collar), right_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar),
		sizeof(front_pocket), front_pocket);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket), sizeof(button1), button1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1), sizeof(button2), button2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2), sizeof(button3), button3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3), sizeof(button4), button4);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_shirt);
	glBindVertexArray(VAO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_shirt() {
	glBindVertexArray(VAO_shirt);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_FRONT_POCKET]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON1]);
	glDrawArrays(GL_TRIANGLE_FAN, 26, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON2]);
	glDrawArrays(GL_TRIANGLE_FAN, 29, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON3]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON4]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);
	glBindVertexArray(0);
}

GLfloat fire_color[9][3] = {
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

void prepare_fire() {
	GLsizeiptr buffer_size = sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3) + sizeof(button4);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(left_body), left_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body), sizeof(right_body), right_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body), sizeof(left_collar), left_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar), sizeof(right_collar), right_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar),
		sizeof(front_pocket), front_pocket);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket), sizeof(button1), button1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1), sizeof(button2), button2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2), sizeof(button3), button3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3), sizeof(button4), button4);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_shirt);
	glBindVertexArray(VAO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_fire() {
	glBindVertexArray(VAO_shirt);

	glUniform3fv(loc_primitive_color, 1, fire_color[SHIRT_LEFT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, fire_color[SHIRT_RIGHT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, fire_color[SHIRT_LEFT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, fire_color[SHIRT_RIGHT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, fire_color[SHIRT_FRONT_POCKET]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 6);

	glUniform3fv(loc_primitive_color, 1, fire_color[SHIRT_BUTTON1]);
	glDrawArrays(GL_TRIANGLE_FAN, 26, 3);

	glUniform3fv(loc_primitive_color, 1, fire_color[SHIRT_BUTTON2]);
	glDrawArrays(GL_TRIANGLE_FAN, 29, 3);

	glUniform3fv(loc_primitive_color, 1, fire_color[SHIRT_BUTTON3]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, fire_color[SHIRT_BUTTON4]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);
	glBindVertexArray(0);
}




#define SWORD_BODY 0
#define SWORD_BODY2 1
#define SWORD_HEAD 2
#define SWORD_HEAD2 3
#define SWORD_IN 4
#define SWORD_DOWN 5
#define SWORD_BODY_IN 6

GLfloat sword_body[4][2]    = { { -6.0, 0.0 },{ -6.0, -4.0 },{ 6.0, -4.0 },{ 6.0, 0.0 } };
GLfloat sword_body2[4][2]   = { { -2.0, -4.0 },{ -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -4.0 } };
GLfloat sword_head[4][2]    = { { -2.0, 0.0 },{ -2.0, 16.0 } ,{ 2.0, 16.0 },{ 2.0, 0.0 } };
GLfloat sword_head2[3][2]   = { { -2.0, 16.0 },{ 0.0, 19.46 } ,{ 2.0, 16.0 } };
GLfloat sword_in[4][2]      = { { -0.3, 0.7 },{ -0.3, 15.3 } ,{ 0.3, 15.3 },{ 0.3, 0.7 } };
GLfloat sword_down[4][2]    = { { -2.0, -6.0 } ,{ 2.0, -6.0 },{ 4.0, -8.0 },{ -4.0, -8.0 } };
GLfloat sword_body_in[4][2] = { { 0.0, -1.0 } ,{ 1.0, -2.732 },{ 0.0, -4.464 },{ -1.0, -2.732 } };

GLfloat sword_color[7][3] = {
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};





int number_of_sword = 10;



GLuint VBO_sword, VAO_sword;

void prepare_sword() {
	GLsizeiptr buffer_size = sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down) + sizeof(sword_body_in);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sword_body), sword_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body), sizeof(sword_body2), sword_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2), sizeof(sword_head), sword_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head), sizeof(sword_head2), sword_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2), sizeof(sword_in), sword_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in), sizeof(sword_down), sword_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down), sizeof(sword_body_in), sword_body_in);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_sword);
	glBindVertexArray(VAO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sword() {
	glBindVertexArray(VAO_sword);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}







void display(void) {
	float x, r, s, delx, delr, dels;
	glm::mat4 ModelMatrix;
	glm::mat4 ModelMatrices[35];

	glm::mat4 FireMatrix[5];

	glClear(GL_COLOR_BUFFER_BIT);

	if (!rightbuttonpressed) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(tx, ty, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 45.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();


		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4 * win_width, -0.4 * win_height, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 45 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (leftbuttonpressed)
			draw_cake();


		int ballclock = timestamp % 361;
		for (int i = 0; i < 5; i++) {
			FireMatrix[i] = glm::translate(glm::mat4(1.0f), glm::vec3(0.2 * win_width, -0.2 * win_height, 0.0f));
			FireMatrix[i] = glm::translate(FireMatrix[i], glm::vec3(-1 * 250.0f * cosf((ballclock + (i * 8)) * TO_RADIAN), 140.0f * cosf((ballclock + (i * 8)) * TO_RADIAN), 0.0f));
			//if (ballclock <= 181)
			FireMatrix[i] = glm::scale(FireMatrix[i], glm::vec3(i * 1.0f, i * 1.0f, 0.0f));
			//else
				//FireMatrix[i] = glm::scale(FireMatrix[i], glm::vec3(i * 1.0f, i * 1.0f, 0.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * FireMatrix[i];
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			if (leftbuttonpressed)
				draw_fire();
		}

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.4 * win_width, 0.4 * win_height, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 135 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (leftbuttonpressed)
			draw_cake();

		for (int i = 0; i < 5; i++) {
			FireMatrix[i] = glm::translate(glm::mat4(1.0f), glm::vec3(0.2 * win_width, 0.2 * win_height, 0.0f));
			FireMatrix[i] = glm::translate(FireMatrix[i], glm::vec3(-1 * 250.0f * cosf((ballclock + (i * 8)) * TO_RADIAN), -1 * 140.0f * cosf((ballclock + (i * 8)) * TO_RADIAN), 0.0f));
			//if (ballclock <= 181)
			FireMatrix[i] = glm::scale(FireMatrix[i], glm::vec3(i * 1.0f, i * 1.0f, 0.0f));
			FireMatrix[i] = glm::rotate(FireMatrix[i], 10 * ballclock * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
			//else
				//FireMatrix[i] = glm::scale(FireMatrix[i], glm::vec3(i * 1.0f, i * 1.0f, 0.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * FireMatrix[i];
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			if (leftbuttonpressed)
				draw_fire();
		}

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.4 * win_width, 0.4 * win_height, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 225 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (leftbuttonpressed)
			draw_cake();


		for (int i = 0; i < 5; i++) {
			FireMatrix[i] = glm::translate(glm::mat4(1.0f), glm::vec3(-1 * 0.2 * win_width, 0.2 * win_height, 0.0f));
			FireMatrix[i] = glm::translate(FireMatrix[i], glm::vec3(1 * 250.0f * cosf((ballclock + (i * 8)) * TO_RADIAN), -1 * 140.0f * cosf((ballclock + (i * 8)) * TO_RADIAN), 0.0f));
			//if (ballclock <= 181)
			FireMatrix[i] = glm::scale(FireMatrix[i], glm::vec3(i * 1.0f, i * 1.0f, 0.0f));
			FireMatrix[i] = glm::rotate(FireMatrix[i], 10 * ballclock * TO_RADIAN, glm::vec3(1.0f, 1.0f, 0.0f));
			//else
				//FireMatrix[i] = glm::scale(FireMatrix[i], glm::vec3(i * 1.0f, i * 1.0f, 0.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * FireMatrix[i];
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			if (leftbuttonpressed)
				draw_fire();
		}

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.4 * win_width, -0.4 * win_height, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 315 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (leftbuttonpressed)
			draw_cake();

		for (int i = 0; i < 5; i++) {
			FireMatrix[i] = glm::translate(glm::mat4(1.0f), glm::vec3(-1 * 0.2 * win_width, -1 * 0.2 * win_height, 0.0f));
			FireMatrix[i] = glm::translate(FireMatrix[i], glm::vec3(1 * 250.0f * cosf((ballclock + (i * 8)) * TO_RADIAN), 1 * 140.0f * cosf((ballclock + (i * 8)) * TO_RADIAN), 0.0f));
			//if (ballclock <= 181)
			FireMatrix[i] = glm::scale(FireMatrix[i], glm::vec3(i * 1.0f, i * 1.0f, 0.0f));
			FireMatrix[i] = glm::rotate(FireMatrix[i], 10 * ballclock * TO_RADIAN, glm::vec3(ballclock, ballclock, ballclock));
			//else
				//FireMatrix[i] = glm::scale(FireMatrix[i], glm::vec3(i * 1.0f, i * 1.0f, 0.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * FireMatrix[i];
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			if (leftbuttonpressed)
				draw_fire();
		}



		glm::mat4 WaterMatrix[35];

		int waterclock = timestamp_water % 721;
		for (int i = 0; i < 35; i++) {

			WaterMatrix[i] = glm::translate(glm::mat4(1.0f), glm::vec3(-1 * (waterclock + i * 4) + waterx, watery, 0.0f));

			WaterMatrix[i] = glm::translate(WaterMatrix[i], glm::vec3(30.0f * cosf((waterclock + i * 4) * TO_RADIAN), 150.0f * sinf((waterclock + i * 4) * TO_RADIAN), 0.0f));


			ModelViewProjectionMatrix = ViewProjectionMatrix * WaterMatrix[i];
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			if (waterclock % 360 >= 90 && waterclock % 360 <= 270)
				draw_water2();
			else
				draw_water();

		}
		//draw_line();
		//draw_airplane();
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		int airplane_clock = timestamp % 720;
		float time_scale = 0.5f;
		for (int i = 0; i < 35; i++) {
			//ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-100.0f + i * 20.0f + centerx, centery, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, airplane_clock * time_scale * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(fabs(AIRPLANE_ROTATION_RADIUS - (sqrt(pow(centerx, 2) + pow(centery, 2)))) * cos(airplane_clock * time_scale * TO_RADIAN), AIRPLANE_ROTATION_RADIUS * sin(airplane_clock * time_scale * TO_RADIAN), 0.0f));
			ModelMatrices[i] = ModelMatrix;
		}



		for (int i = 0; i < 35; i++) {
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrices[i];
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			if (centerpressed)
				draw_airplane();
		}



		glm::mat4 SwordMatrices[10];
		//ModelMatrix = glm::mat4(1.0f);
		default_random_engine generator_sword;
		uniform_real_distribution<float> x_distribution(-150.0f, 150.0f);
		uniform_real_distribution<float> y_distribution(-250.0f, 250.0f);
		uniform_real_distribution<float> angle_distribution(-1.0f, 1.0f);
		uniform_real_distribution<float> scale(0.01f, 2.0f);

		int sword_clock = (timestamp_sword % 720) - 360;
		float rotation_angle_sword = atanf(100.0f * TO_RADIAN * cosf(sword_clock * TO_RADIAN));
		//float scaling_sword =  * (1.0f - fabs(cosf(sword_clock * TO_RADIAN)));

		for (int i = 0; i < number_of_sword; i++) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x_distribution(generator_sword), y_distribution(generator_sword), 0.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3((float)sword_clock, 100.0f + cosf(rotation_angle_sword), 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, rotation_angle_sword * angle_distribution(generator_sword), glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(sqrt(fabs(pow(y_distribution(generator_sword), 2) - pow(x_distribution(generator_sword), 2))) * cosf(rotation_angle_sword), sqrt(fabs(pow(y_distribution(generator_sword), 2) - pow(x_distribution(generator_sword), 2))) * sinf(rotation_angle_sword), 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scale(generator_sword), scale(generator_sword), 1.0f));
			ModelMatrices[i] = ModelMatrix;
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrices[i];
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_sword();
		}

		int house_clock = timestamp_house % 360;
		glm::mat4 ModelMatrices_house1[9];
		glm::mat4 ModelMatrices_house2[9];
		float housedx[8] = { house_clock, sqrt(powf(house_clock,2)) * cosf(45 * TO_RADIAN), 0, -sqrt(powf(house_clock,2)) * cosf(45 * TO_RADIAN), -house_clock, -sqrt(powf(house_clock,2)) * cosf(45 * TO_RADIAN), 0, sqrt(powf(house_clock,2)) * cosf(45 * TO_RADIAN) };
		float housedy[8] = { 0, sqrt(powf(house_clock,2)) * sinf(45 * TO_RADIAN), house_clock, sqrt(powf(house_clock,2)) * sinf(45 * TO_RADIAN), 0, -sqrt(powf(house_clock,2)) * sinf(45 * TO_RADIAN), -house_clock, -sqrt(powf(house_clock,2)) * sinf(45 * TO_RADIAN) };
		float houseangle[] = { 0, };


		for (int i = 0; i < house_ampli; i++) {
			ModelMatrices_house1[i] = glm::translate(glm::mat4(1.0f), glm::vec3(housedx[i], housedy[i], 0.0f));

			if (house_ampli >= 8) {
				ModelMatrices_house1[i] = glm::rotate(ModelMatrices_house1[i], -1 * house_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrices_house1[i] = glm::translate(ModelMatrices_house1[i], glm::vec3(cosf(house_clock * TO_RADIAN) + house_clock, -1 * sinf(house_clock * TO_RADIAN) + house_clock, 0.0f));
			}

			ModelMatrices_house1[i] = glm::translate(ModelMatrices_house1[i], glm::vec3(30.0f * cosf(house_clock * TO_RADIAN), 30.0f * sinf(house_clock * TO_RADIAN), 0.0f));

			ModelMatrices_house1[i] = glm::scale(ModelMatrices_house1[i], glm::vec3(0.1f * (9 - i), 0.1f * (9 - i), 0.0f));
			ModelMatrices_house1[i] = glm::rotate(ModelMatrices_house1[i], house_clock * TO_RADIAN, glm::vec3(5.0f, 5.0f, -1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrices_house1[i];
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_house();
		}
		for (int i = 0; i < house_ampli; i++) {
			ModelMatrices_house2[i] = glm::translate(glm::mat4(1.0f), glm::vec3(housedx[i], housedy[i], 0.0f));

			if (house_ampli >= 8) {
				ModelMatrices_house2[i] = glm::rotate(ModelMatrices_house2[i], house_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrices_house2[i] = glm::translate(ModelMatrices_house2[i], glm::vec3(cosf(house_clock * TO_RADIAN) + house_clock, -1 * sinf(house_clock * TO_RADIAN) + house_clock, 0.0f));
			}

			ModelMatrices_house2[i] = glm::translate(ModelMatrices_house2[i], glm::vec3(30.0f * cosf(house_clock * TO_RADIAN), -1 * 30.0f * sinf(house_clock * TO_RADIAN), 0.0f));

			ModelMatrices_house2[i] = glm::scale(ModelMatrices_house2[i], glm::vec3(0.1f * (9 - i), 0.1f * (9 - i), 0.0f));
			ModelMatrices_house2[i] = glm::rotate(ModelMatrices_house2[i], house_clock * TO_RADIAN, glm::vec3(5.0f, 5.0f, -1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrices_house2[i];
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_house();
		}

	}

	if (rightbuttonpressed) {
		int shirt_clock = timestamp % 1460;
		int st, sy;
		glm::mat4 ShirtMatrix[100];
		for (int i = 0; i < 100; i++) {
			if (i == 0) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-win_width / 2, 0.0f, 0.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(200.0f * cosf((shirt_clock)*TO_RADIAN) + shirt_clock, 400.0f * cosf((shirt_clock)*TO_RADIAN), 0.0f));
				ShirtMatrix[i] = ModelMatrix;
			}
			else {
				ShirtMatrix[i] = glm::translate(ShirtMatrix[i], glm::vec3(-win_width / 2, 0.0f, 0.0f));
				ShirtMatrix[i] = glm::translate(ShirtMatrix[i], glm::vec3(200.0f * cosf((shirt_clock - i * 10) * TO_RADIAN) + shirt_clock - i, 400.0f * cosf((shirt_clock - i) * TO_RADIAN), 0.0f));
			}
			ShirtMatrix[i] = glm::scale(ShirtMatrix[i], glm::vec3((10 - 0.1 * i), 10 - 0.1 * i, 0.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ShirtMatrix[i];
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_shirt();
		}

	

		for (int i = 0; i < 100; i++) {

			ShirtMatrix[i] = glm::translate(ShirtMatrix[i], glm::vec3(win_width / 2, 0.0f, 0.0f));
			ShirtMatrix[i] = glm::translate(ShirtMatrix[i], glm::vec3(200.0f * cosf((shirt_clock - i * 10) * TO_RADIAN) - shirt_clock, 400.0f * cosf((shirt_clock - i) * TO_RADIAN), 0.0f));

			ShirtMatrix[i] = glm::scale(ShirtMatrix[i], glm::vec3(1.0f, 1.0f, 0.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ShirtMatrix[i];
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_shirt();
		}
	}

	/*ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx, centery, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, rotate_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
 	//draw_airplane(); // 0*/
	
	/*ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-win_width / 4.0f, -win_height / 4.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane(); */ // 1
	
	/*ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(win_width / 2.5f, -win_height / 8.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 270.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();  // 2
 
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(win_height / 4.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 45.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f, -1.0f, 1.0f));
	ModelMatrix = glm::rotate(ModelMatrix, -45.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-win_height / 4.0f, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(win_width / 2.5f, -win_height / 8.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-win_width / 2.5f, win_height / 8.0f, 0.0f));
 
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(win_width / 2.5f, -win_height / 8.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 270.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();  // 3
	 
	delx = win_width/14.0f; delr = 15.0f; dels = 1.1f;
	x = -delx; r = delr; s = dels;
	for (i = 0; i < 5; i++, x -= delx, r += delr, s *= dels) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 15.0f*sqrtf(-x), 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, r*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			glTranslatef(x, 15.0f*sqrtf(-x), 0.0f);
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(s, s, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_airplane();  // 4
	}*/
	glFlush();	
}   

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

void special(int key, int x, int y) {
#define SENSITIVITY 2.0
	switch (key) {
	case GLUT_KEY_LEFT:
		centerx -= SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		centerx += SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		centery -= SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		centery += SENSITIVITY;
		glutPostRedisplay();
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		leftbuttonpressed = 1;
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		leftbuttonpressed = 0;
	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		rightbuttonpressed = 1;
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
		rightbuttonpressed = 0;
	if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN))
		centerpressed = 1;
	else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_UP))
		centerpressed = 0;
}

void motion(int x, int y) {
	static int delay = 0;
	static float tmpx = 0.0, tmpy = 0.0;
	float dx, dy;
	
	tx = x - win_width / 2;
	ty = -1 * (y - win_height / 2);

	waterx = x - win_width / 2 - 10;
	watery = -1 * ((y - win_height / 2) + 3);


	if (leftbuttonpressed) {
		centerx =  x - win_width/2.0f, centery = (win_height - y) - win_height/2.0f;
		if (delay == 8) {	
			dx = centerx - tmpx;
			dy = centery - tmpy;
	  
			if (dx > 0.0) {
				rotate_angle = atan(dy / dx) + 90.0f*TO_RADIAN;
			}
			else if (dx < 0.0) {
				rotate_angle = atan(dy / dx) - 90.0f*TO_RADIAN;
			}
			else if (dx == 0.0) {
				if (dy > 0.0) rotate_angle = 180.0f*TO_RADIAN;
				else  rotate_angle = 0.0f;
			}
			tmpx = centerx, tmpy = centery; 




			delay = 0;
		}

		
		glutPostRedisplay();
		delay++;
		
	}


	glutPostRedisplay();
} 
	
void motion1(int x, int y) {

	glutPostRedisplay();
	
}

void reshape(int width, int height) {
	win_width = width, win_height = height;
	
  	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0, 
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();

	glutPostRedisplay();
}




void timer(int value) {
	timestamp = (timestamp + 1) % UINT_MAX;
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

void timer_water(int value) {

	timestamp_water = (timestamp_water + 1) % UINT_MAX;

	glutPostRedisplay();
	glutTimerFunc(10, timer_water, 0);
}

void timer_sword(int value) {
	timestamp_sword = (timestamp_sword + 1) % UINT_MAX;
	glutPostRedisplay();
	glutTimerFunc(30, timer_sword, 0);
}

void timer_house(int value) {
	timestamp_house = (timestamp_house + 1) % UINT_MAX;
	glutPostRedisplay();
	glutTimerFunc(10, timer_house, 0);
}

void timer_house_tmp(int value) {
	glutPostRedisplay();
	if (house_ampli <= 7)
		house_ampli++;;
	glutTimerFunc(3610, timer_house_tmp, 0);
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0);
	glutTimerFunc(10, timer_water, 0);
	glutTimerFunc(30, timer_sword, 0);
	glutTimerFunc(10, timer_house, 0);
	glutTimerFunc(3610, timer_house_tmp, 0);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glClearColor(44 / 255.0f, 180 / 255.0f, 49 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_airplane();
	prepare_sword();
	prepare_house();
	prepare_water();
	prepare_shirt();
	prepare_water2();
	prepare_cake();
	prepare_cocktail();
	prepare_fire();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program(); 
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

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

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 2
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple2DTransformation_GLSL_3.0";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC', four arrows",
		"    - Mouse used: L-click and move"
	};

	glutInit (&argc, argv);
 	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize (1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);
	glutSetCursor(GLUT_CURSOR_NONE);
	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop ();
}


