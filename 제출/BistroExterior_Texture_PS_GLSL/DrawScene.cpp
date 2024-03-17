//
//  DrawScene.cpp
//
//  Written for CSE4170
//  Department of Computer Science and Engineering
//  Copyright © 2023 Sogang University. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "LoadScene.h"

// Begin of shader setup
#include "Shaders/LoadShaders.h"
#include "ShadingInfo.h"

extern SCENE scene;
using namespace std;

// for simple shaders
GLuint h_ShaderProgram_simple; // handle to shader program
GLuint h_ShaderProgram_background, h_ShaderProgram_equiToCube;
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// for PBR
GLuint h_ShaderProgram_TXPBR;
#define NUMBER_OF_LIGHT_SUPPORTED 1
GLint loc_global_ambient_color;
GLint loc_lightCount;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
GLint loc_ModelViewProjectionMatrix_TXPBR, loc_ModelViewMatrix_TXPBR, loc_ModelViewMatrixInvTrans_TXPBR;
GLint loc_cameraPos;

#define TEXTURE_INDEX_DIFFUSE	(0)
#define TEXTURE_INDEX_NORMAL	(1)
#define TEXTURE_INDEX_SPECULAR	(2)
#define TEXTURE_INDEX_EMISSIVE	(3)
#define TEXTURE_INDEX_SKYMAP	(4)

// for skybox shaders
GLuint h_ShaderProgram_skybox;
GLint loc_cubemap_skybox;
GLint loc_ModelViewProjectionMatrix_SKY;

// include glm/*.hpp only if necessary
// #include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
// ViewProjectionMatrix = ProjectionMatrix * ViewMatrix
glm::mat4 ViewProjectionMatrix, ViewMatrix, ProjectionMatrix;
// ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix
glm::mat4 ModelViewProjectionMatrix; // This one is sent to vertex shader when ready.
glm::mat4 ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
float window_height, window_width; 
int prevy, prevx;
int leftbuttonpressed, rightbuttonpressed, center_selected;

struct pts {
	float x;
	float y;
	float z;
};

vector<pts> wolf_displacement = { {5408.648438, -2905.653809, 283.968231},
	{5408.648438-200, -2955.653809-100, 333.968231},
	{5408.648438-200, -3005.653809-100, 383.968231},
	{5408.648438-200, -3055.653809-100, 433.968231},
	{5408.648438-200, -3105.653809-100, 483.968231},
	{5408.648438-200, -3155.653809-100, 533.968262},
	{5408.648438-200, -3205.653809-100, 583.968262},
	{5408.648438-200, -3255.653809-100, 633.968262},
	{5408.648438-200, -3305.653809-100, 683.968262},
	{5408.648438-200, -3355.653809-100, 733.968262},
	{5408.648438-200, -3405.653809-100, 783.968262},
	{5408.648438-200, -3455.653809-100, 833.968262},
	{5408.648438-200, -3505.653809-100, 883.968262},
	{5408.648438-200, -3555.653809-100, 933.968262},
	{5408.648438-200, -3505.653809+100, 983.968262},
	{5408.648438-200, -3455.653809+100, 1033.968262},
	{5408.648438-200, -3405.653809+100, 1083.968262},
	{5408.648438-200, -3355.653809+100, 1133.968262},
	{5408.648438-200, -3305.653809+100, 1083.968262},
	{5408.648438-200, -3255.653809+100, 1033.968262},
	{5408.648438-200, -3205.653809+100, 983.968262}	,
	{5408.648438-200, -3155.653809+100, 1033.968262},
	{5408.648438-200, -3105.653809+100, 1083.968262},
	{5408.648438-200, -3055.653809+100, 1133.968262},
	{5408.648438-200, -3005.653809+100, 1083.968262},
	{5408.648438-200, -2955.653809+100, 1033.968262},
	{5408.648438-200, -2905.653809+100, 983.968262}	,
	{5408.648438-200, -2855.653809+100, 933.968262}	,
	{5408.648438-200, -2805.653809+100, 883.968262}	,
	{5408.648438-200, -2855.653809-100, 833.968262}	,
	{5408.648438-200, -2905.653809-100, 783.968262}	,
	{5408.648438-200, -2955.653809-100, 733.968262}	,
	{5408.648438-200, -3005.653809-100, 683.968262}	,
	{5408.648438-200, -3055.653809-100, 633.968262}	,
	{5408.648438-200, -3105.653809-100, 583.968262}	,
	{5408.648438-200, -3155.653809-100, 533.968262}	,
	{5408.648438-200, -3205.653809-100, 483.968262}	,
	{5408.648438-200, -3255.653809-100, 433.968262}	,
	{5408.648438-200, -3305.653809-100, 383.968262}	,
	{5408.648438-200, -3355.653809-100, 333.968262}	,
	{5408.648438-200, -3405.653809-100, 283.968262}	,
};

vector<pts> spider_displacement = { {1358.648193, 3844.346191, 283.968231},
	{1408.648193, 3844.346191, 283.968231},
	{1458.648193, 3844.346191, 283.968231}, 
	{1508.648193, 3794.346191, 283.968231},
	{1458.648193, 3744.346191, 283.968231},
	{1408.648193, 3694.346191, 283.968231},
	{1408.648193, 3644.346191, 283.968231},
	{1408.648193, 3594.346191, 283.968231},
	{1408.648193, 3544.346191, 283.968231},
	{1408.648193, 3494.346191, 283.968231},
	{1358.648193, 3444.346191, 283.968231},
	{1358.648193, 3394.346191, 283.968231},
	{1308.648193, 3344.346191, 283.968231},
	{1258.648193, 3294.346191, 283.968231},
	{1208.648193, 3244.346191, 283.968231},
	{1208.648193, 3194.346191, 283.968231},
	{1158.648193, 3144.346191, 283.968231},
	{1158.648193, 3094.346191, 283.968231},
	{1108.648193, 3044.346191, 283.968231},
	{1108.648193, 2994.346191, 283.968231},
	{1058.648193, 2944.346191, 283.968231},
	{1058.648193, 2894.346191, 283.968231},
	{1008.648193, 2844.346191, 283.968231},
	{1008.648193, 2794.346191, 283.968231},
	{958.648193, 2744.346191, 283.968231},
	{958.648193, 2694.346191, 283.968231},
	{908.648193, 2644.346191, 283.968231},
	{858.648193, 2594.346191, 283.968231}, 
	{808.648193, 2544.346191, 283.968231},
	{808.648193, 2494.346191, 283.968231},
	{758.648193, 2444.346191, 283.968231},
	{708.648193, 2394.346191, 283.968231},
	{658.648193, 2344.346191, 283.968231},
	{608.648193, 2294.346191, 283.968231},
	{558.648193, 2244.346191, 283.968231},
	{508.648193, 2194.346191, 283.968231},
	{458.648193, 2144.346191, 283.968231},
	{408.648193, 2094.346191, 283.968231},
	{358.648193, 2044.346191, 283.968231},
	{308.648193, 1994.346191, 283.968231},
	{258.648193, 1944.346191, 283.968231},
	{208.648193, 1894.346191, 283.968231},
	{158.648193, 1844.346191, 283.968231}, 
};

/*********************************  START: camera *********************************/
typedef enum {
	CAMERA_1,
	CAMERA_2,
	CAMERA_3,
	CAMERA_4,
	CAMERA_5,
	CAMERA_6,
	CAMERA_U,
	CAMERA_I,
	CAMERA_O,
	CAMERA_P,
	CAMERA_A,
	CAMERA_T,
	CAMERA_G,
	NUM_CAMERAS
} CAMERA_INDEX;

typedef struct _Camera {
	float pos[3];
	float uaxis[3], vaxis[3], naxis[3];
	float fovy, aspect_ratio, near_c, far_c;
	int move, rotation_axis;
} Camera;

Camera camera_info[NUM_CAMERAS];
Camera current_camera;
using glm::mat4;
void set_ViewMatrix_from_camera_frame(void) {
	ViewMatrix = glm::mat4(current_camera.uaxis[0], current_camera.vaxis[0], current_camera.naxis[0], 0.0f,
		current_camera.uaxis[1], current_camera.vaxis[1], current_camera.naxis[1], 0.0f,
		current_camera.uaxis[2], current_camera.vaxis[2], current_camera.naxis[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-current_camera.pos[0], -current_camera.pos[1], -current_camera.pos[2]));
}

void set_current_camera(int camera_num) {
	Camera* pCamera = &camera_info[camera_num];

	memcpy(&current_camera, pCamera, sizeof(Camera));
	set_ViewMatrix_from_camera_frame();
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void initialize_camera(void) {
	//CAMERA_1 : original view
	Camera* pCamera = &camera_info[CAMERA_1];
	for (int k = 0; k < 3; k++)
	{
		pCamera->pos[k] = scene.camera.e[k];
		pCamera->uaxis[k] = scene.camera.u[k];
		pCamera->vaxis[k] = scene.camera.v[k];
		pCamera->naxis[k] = scene.camera.n[k];
	}

	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_2 : bistro view
	pCamera = &camera_info[CAMERA_2];
	pCamera->pos[0] = -241.351807f; pCamera->pos[1] = 1444.346313f; pCamera->pos[2] = 283.968231f;
	pCamera->uaxis[0] = -0.998411f; pCamera->uaxis[1] = 0.049119f; pCamera->uaxis[2] = -0.027553f;
	pCamera->vaxis[0] = -0.028485f; pCamera->vaxis[1] = -0.018375f; pCamera->vaxis[2] = 0.999417f;
	pCamera->naxis[0] = 0.048585f; pCamera->naxis[1] = 0.998617f; pCamera->naxis[2] = 0.019746f;
	pCamera->move = 1;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;
	//
	////CAMERA_3 : tree view
	pCamera = &camera_info[CAMERA_3];
	pCamera->pos[0] = 1974.856567f; pCamera->pos[1] = -1588.545776f; pCamera->pos[2] = 98.843971f;
	pCamera->uaxis[0] = -0.357811f; pCamera->uaxis[1] = -0.933725f; pCamera->uaxis[2] = 0.010082f;
	pCamera->vaxis[0] = -0.180880f; pCamera->vaxis[1] = 0.079899f; pCamera->vaxis[2] = 0.980231f;
	pCamera->naxis[0] = -0.916095f; pCamera->naxis[1] = 0.348920f; pCamera->naxis[2] = -0.197483f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;
	//
	////CAMERA_4 : top view
	//pCamera = &camera_info[CAMERA_4];
	//pCamera->pos[0] = 0.0f; pCamera->pos[1] = 0.0f; pCamera->pos[2] = 18300.0f;
	//pCamera->uaxis[0] = 1.0f; pCamera->uaxis[1] = 0.0f; pCamera->uaxis[2] = 0.0f;
	//pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 1.0f; pCamera->vaxis[2] = 0.0f;
	//pCamera->naxis[0] = 0.0f; pCamera->naxis[1] = 0.0f; pCamera->naxis[2] = 1.0f;
	//pCamera->move = 0;
	//pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;
	//
	////CAMERA_5 : front view
	//pCamera = &camera_info[CAMERA_5];
	//pCamera->pos[0] = 0.0f; pCamera->pos[1] = 11700.0f; pCamera->pos[2] = 0.0f;
	//pCamera->uaxis[0] = 1.0f; pCamera->uaxis[1] = 0.0f; pCamera->uaxis[2] = 0.0f;
	//pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 0.0f; pCamera->vaxis[2] = 1.0f;
	//pCamera->naxis[0] = 0.0f; pCamera->naxis[1] = 1.0f; pCamera->naxis[2] = 0.0f;
	//pCamera->move = 0;
	//pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;
	//
	////CAMERA_6 : side view
	//pCamera = &camera_info[CAMERA_6];
	//pCamera->pos[0] = 14600.0f; pCamera->pos[1] = 0.0f; pCamera->pos[2] = 0.0f;
	//pCamera->uaxis[0] = 0.0f; pCamera->uaxis[1] = 1.0f; pCamera->uaxis[2] = 0.0f;
	//pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 0.0f; pCamera->vaxis[2] = 1.0f;
	//pCamera->naxis[0] = 1.0f; pCamera->naxis[1] = 0.0f; pCamera->naxis[2] = 0.0f;
	//pCamera->move = 0;
	//pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	pCamera = &camera_info[CAMERA_U];
	pCamera->pos[0] = 2258.648193f; pCamera->pos[1] = 4544.346191f; pCamera->pos[2] = 2883.968262f;
	pCamera->uaxis[0] = -0.823909f; pCamera->uaxis[1] = 0.564103f; pCamera->uaxis[2] = -0.054386f;
	pCamera->vaxis[0] = -0.296026f; pCamera->vaxis[1] = -0.346550f; pCamera->vaxis[2] = 0.890089f;
	pCamera->naxis[0] = 0.483256f; pCamera->naxis[1] = 0.749454f; pCamera->naxis[2] = 0.452517f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;
	

	pCamera = &camera_info[CAMERA_I];
	pCamera->pos[0] = 6208.648438f; pCamera->pos[1] = -3155.653809f; pCamera->pos[2] = 2683.968262f;
	pCamera->uaxis[0] = 0.295564f; pCamera->uaxis[1] = 0.953450f; pCamera->uaxis[2] = -0.059789f;
	pCamera->vaxis[0] = -0.530903f; pCamera->vaxis[1] = 0.215963f; pCamera->vaxis[2] = 0.819445f;
	pCamera->naxis[0] = 0.794214f; pCamera->naxis[1] = -0.210458f; pCamera->naxis[2] = 0.570022f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;
	
	pCamera = &camera_info[CAMERA_O];
	pCamera->pos[0] = -7041.351562f; pCamera->pos[1] = -3105.653809f; pCamera->pos[2] = 9833.968750f;
	pCamera->uaxis[0] = 0.321958f; pCamera->uaxis[1] = -0.945992f; pCamera->uaxis[2] = -0.037980f;
	pCamera->vaxis[0] = 0.686251f; pCamera->vaxis[1] = 0.205546f; pCamera->vaxis[2] = 0.697712f;
	pCamera->naxis[0] = -0.652226f; pCamera->naxis[1] = -0.250697f; pCamera->naxis[2] = 0.715367f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	pCamera = &camera_info[CAMERA_P];
	pCamera->pos[0] = -2491.351807f; pCamera->pos[1] = 1044.346191f; pCamera->pos[2] = 3433.968262f;
	pCamera->uaxis[0] = -0.251225f; pCamera->uaxis[1] = -0.967734f; pCamera->uaxis[2] = 0.019423f;
	pCamera->vaxis[0] = 0.651908f; pCamera->vaxis[1] = -0.154337f; pCamera->vaxis[2] = 0.742414f;
	pCamera->naxis[0] = -0.715464f; pCamera->naxis[1] = 0.199176f; pCamera->naxis[2] = 0.669651f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	pCamera = &camera_info[CAMERA_A];
	pCamera->pos[0] = 358.648193f; pCamera->pos[1] = -1005.653687f; pCamera->pos[2] = 283.968231f;
	pCamera->uaxis[0] = -0.272761f; pCamera->uaxis[1] = -0.962034f; pCamera->uaxis[2] = 0.009458f;
	pCamera->vaxis[0] = -0.018977f; pCamera->vaxis[1] = 0.015208f; pCamera->vaxis[2] = 0.999696f;
	pCamera->naxis[0] = -0.961888f; pCamera->naxis[1] = 0.272500f; pCamera->naxis[2] = -0.022404f;
	pCamera->move = 1;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;
	set_current_camera(CAMERA_1);
}
/*********************************  END: camera *********************************/

/******************************  START: shader setup ****************************/
// Begin of Callback function definitions
void prepare_shader_program(void) {
	char string[256];

	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram_simple);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");

	ShaderInfo shader_info_TXPBR[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Background/PBR_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Background/PBR_Tx.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_TXPBR = LoadShaders(shader_info_TXPBR);
	glUseProgram(h_ShaderProgram_TXPBR);

	loc_ModelViewProjectionMatrix_TXPBR = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPBR = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPBR = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_ModelViewMatrixInvTrans");

	loc_lightCount = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_light_count");

	for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPBR, string);
		sprintf(string, "u_light[%d].color", i);
		loc_light[i].color = glGetUniformLocation(h_ShaderProgram_TXPBR, string);
	}

	loc_cameraPos = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_camPos");

	//Textures
	loc_material.diffuseTex = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_albedoMap");
	loc_material.normalTex = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_normalMap");
	loc_material.specularTex = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_metallicRoughnessMap");
	loc_material.emissiveTex = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_emissiveMap");

	ShaderInfo shader_info_skybox[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Background/skybox.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Background/skybox.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_skybox = LoadShaders(shader_info_skybox);
	loc_cubemap_skybox = glGetUniformLocation(h_ShaderProgram_skybox, "u_skymap");
	loc_ModelViewProjectionMatrix_SKY = glGetUniformLocation(h_ShaderProgram_skybox, "u_ModelViewProjectionMatrix");
}
/*******************************  END: shder setup ******************************/

/****************************  START: geometry setup ****************************/
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define INDEX_VERTEX_POSITION	0
#define INDEX_NORMAL			1
#define INDEX_TEX_COORD			2

bool b_draw_grid = false;

//axes
GLuint axes_VBO, axes_VAO;
GLfloat axes_vertices[6][3] = {
	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };

void prepare_axes(void) {
	// Initialize vertex buffer object.
	glGenBuffers(1, &axes_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), &axes_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &axes_VAO);
	glBindVertexArray(axes_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	fprintf(stdout, " * Loaded axes into graphics memory.\n");
}

void draw_axes(void) {
	if (!b_draw_grid)
		return;

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(8000.0f, 8000.0f, 8000.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	glBindVertexArray(axes_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glLineWidth(1.0f);
	glUseProgram(0);
}

//grid
#define GRID_LENGTH			(100)
#define NUM_GRID_VETICES	((2 * GRID_LENGTH + 1) * 4)
GLuint grid_VBO, grid_VAO;
GLfloat grid_vertices[NUM_GRID_VETICES][3];
GLfloat grid_color[3] = { 0.5f, 0.5f, 0.5f };

void prepare_grid(void) {

	//set grid vertices
	int vertex_idx = 0;
	for (int x_idx = -GRID_LENGTH; x_idx <= GRID_LENGTH; x_idx++)
	{
		grid_vertices[vertex_idx][0] = x_idx;
		grid_vertices[vertex_idx][1] = -GRID_LENGTH;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;

		grid_vertices[vertex_idx][0] = x_idx;
		grid_vertices[vertex_idx][1] = GRID_LENGTH;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;
	}

	for (int y_idx = -GRID_LENGTH; y_idx <= GRID_LENGTH; y_idx++)
	{
		grid_vertices[vertex_idx][0] = -GRID_LENGTH;
		grid_vertices[vertex_idx][1] = y_idx;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;

		grid_vertices[vertex_idx][0] = GRID_LENGTH;
		grid_vertices[vertex_idx][1] = y_idx;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;
	}

	// Initialize vertex buffer object.
	glGenBuffers(1, &grid_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, grid_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grid_vertices), &grid_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &grid_VAO);
	glBindVertexArray(grid_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, grid_VAO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	fprintf(stdout, " * Loaded grid into graphics memory.\n");
}

void draw_grid(void) {
	if (!b_draw_grid)
		return;

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(1.0f);
	glBindVertexArray(grid_VAO);
	glUniform3fv(loc_primitive_color, 1, grid_color);
	glDrawArrays(GL_LINES, 0, NUM_GRID_VETICES);
	glBindVertexArray(0);
	glLineWidth(1.0f);
	glUseProgram(0);
}

// bistro_exterior
GLuint* bistro_exterior_VBO;
GLuint* bistro_exterior_VAO;
int* bistro_exterior_n_triangles;
int* bistro_exterior_vertex_offset;
GLfloat** bistro_exterior_vertices;
GLuint* bistro_exterior_texture_names;

int flag_fog;
bool* flag_texture_mapping;

void initialize_lights(void) { // follow OpenGL conventions for initialization
	glUseProgram(h_ShaderProgram_TXPBR);

	glUniform1f(loc_lightCount, scene.n_lights);

	for (int i = 0; i < scene.n_lights; i++) {
		glUniform4f(loc_light[i].position,
			scene.light_list[i].pos[0],
			scene.light_list[i].pos[1],
			scene.light_list[i].pos[2],
			0.0f);

		glUniform3f(loc_light[i].color,
			scene.light_list[i].color[0],
			scene.light_list[i].color[1],
			scene.light_list[i].color[2]);
	}

	glUseProgram(0);
}

bool readTexImage2D_from_file(char* filename) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap, * tx_pixmap_32;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	if (tx_pixmap == NULL)
		return false;
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	//fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);
	GLenum format, internalFormat;
	if (tx_bits_per_pixel == 32) {
		format = GL_BGRA;
		internalFormat = GL_RGBA;
	}
	else if (tx_bits_per_pixel == 24) {
		format = GL_BGR;
		internalFormat = GL_RGB;
	}
	else {
		fprintf(stdout, " * Converting texture from %d bits to 32 bits...\n", tx_bits_per_pixel);
		tx_pixmap = FreeImage_ConvertTo32Bits(tx_pixmap);
		format = GL_BGRA;
		internalFormat = GL_RGBA;
	}

	width = FreeImage_GetWidth(tx_pixmap);
	height = FreeImage_GetHeight(tx_pixmap);
	data = FreeImage_GetBits(tx_pixmap);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	//fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap);

	return true;
}

void prepare_bistro_exterior(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	// VBO, VAO malloc
	bistro_exterior_VBO = (GLuint*)malloc(sizeof(GLuint) * scene.n_materials);
	bistro_exterior_VAO = (GLuint*)malloc(sizeof(GLuint) * scene.n_materials);

	bistro_exterior_n_triangles = (int*)malloc(sizeof(int) * scene.n_materials);
	bistro_exterior_vertex_offset = (int*)malloc(sizeof(int) * scene.n_materials);

	flag_texture_mapping = (bool*)malloc(sizeof(bool) * scene.n_textures);

	// vertices
	bistro_exterior_vertices = (GLfloat**)malloc(sizeof(GLfloat*) * scene.n_materials);

	for (int materialIdx = 0; materialIdx < scene.n_materials; materialIdx++) {
		MATERIAL* pMaterial = &(scene.material_list[materialIdx]);
		GEOMETRY_TRIANGULAR_MESH* tm = &(pMaterial->geometry.tm);

		// vertex
		bistro_exterior_vertices[materialIdx] = (GLfloat*)malloc(sizeof(GLfloat) * 8 * tm->n_triangle * 3);

		int vertexIdx = 0;
		for (int triIdx = 0; triIdx < tm->n_triangle; triIdx++) {
			TRIANGLE tri = tm->triangle_list[triIdx];
			for (int triVertex = 0; triVertex < 3; triVertex++) {
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].x;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].y;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].z;

				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].x;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].y;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].z;

				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.texture_list[triVertex][0].u;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.texture_list[triVertex][0].v;
			}
		}

		// # of triangles
		bistro_exterior_n_triangles[materialIdx] = tm->n_triangle;

		if (materialIdx == 0)
			bistro_exterior_vertex_offset[materialIdx] = 0;
		else
			bistro_exterior_vertex_offset[materialIdx] = bistro_exterior_vertex_offset[materialIdx - 1] + 3 * bistro_exterior_n_triangles[materialIdx - 1];

		glGenBuffers(1, &bistro_exterior_VBO[materialIdx]);

		glBindBuffer(GL_ARRAY_BUFFER, bistro_exterior_VBO[materialIdx]);
		glBufferData(GL_ARRAY_BUFFER, bistro_exterior_n_triangles[materialIdx] * 3 * n_bytes_per_vertex,
			bistro_exterior_vertices[materialIdx], GL_STATIC_DRAW);

		// As the geometry data exists now in graphics memory, ...
		free(bistro_exterior_vertices[materialIdx]);

		// Initialize vertex array object.
		glGenVertexArrays(1, &bistro_exterior_VAO[materialIdx]);
		glBindVertexArray(bistro_exterior_VAO[materialIdx]);

		glBindBuffer(GL_ARRAY_BUFFER, bistro_exterior_VBO[materialIdx]);
		glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(0));
		glEnableVertexAttribArray(INDEX_VERTEX_POSITION);
		glVertexAttribPointer(INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
		glEnableVertexAttribArray(INDEX_NORMAL);
		glVertexAttribPointer(INDEX_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(INDEX_TEX_COORD);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if ((materialIdx > 0) && (materialIdx % 100 == 0))
			fprintf(stdout, " * Loaded %d bistro exterior materials into graphics memory.\n", materialIdx / 100 * 100);
	}
	fprintf(stdout, " * Loaded %d bistro exterior materials into graphics memory.\n", scene.n_materials);

	// textures
	bistro_exterior_texture_names = (GLuint*)malloc(sizeof(GLuint) * scene.n_textures);
	glGenTextures(scene.n_textures, bistro_exterior_texture_names);

	for (int texId = 0; texId < scene.n_textures; texId++) {
		glBindTexture(GL_TEXTURE_2D, bistro_exterior_texture_names[texId]);

		bool bReturn = readTexImage2D_from_file(scene.texture_file_name[texId]);

		if (bReturn) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			//glGenerateMipmap(GL_TEXTURE_2D);
			flag_texture_mapping[texId] = true;
		}
		else {
			flag_texture_mapping[texId] = false;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	fprintf(stdout, " * Loaded bistro exterior textures into graphics memory.\n");

	free(bistro_exterior_vertices);
}

void bindTexture(GLuint tex, int glTextureId, int texId) {
	if (INVALID_TEX_ID != texId) {
		glActiveTexture(GL_TEXTURE0 + glTextureId);
		glBindTexture(GL_TEXTURE_2D, bistro_exterior_texture_names[texId]);
		glUniform1i(tex, glTextureId);
	}
}

void draw_bistro_exterior(void) {
	glUseProgram(h_ShaderProgram_TXPBR);
	ModelViewMatrix = ViewMatrix;
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPBR, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPBR, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPBR, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	glUniform4fv(loc_cameraPos, 1, current_camera.pos);

	for (int materialIdx = 0; materialIdx < scene.n_materials; materialIdx++) {
		int diffuseTexId = scene.material_list[materialIdx].diffuseTexId;
		int normalMapTexId = scene.material_list[materialIdx].normalMapTexId;
		int specularTexId = scene.material_list[materialIdx].specularTexId;;
		int emissiveTexId = scene.material_list[materialIdx].emissiveTexId;

		bindTexture(loc_material.diffuseTex, TEXTURE_INDEX_DIFFUSE, diffuseTexId);
		bindTexture(loc_material.normalTex, TEXTURE_INDEX_NORMAL, normalMapTexId);
		bindTexture(loc_material.specularTex, TEXTURE_INDEX_SPECULAR, specularTexId);
		bindTexture(loc_material.emissiveTex, TEXTURE_INDEX_EMISSIVE, emissiveTexId);
		glEnable(GL_TEXTURE_2D);

		glBindVertexArray(bistro_exterior_VAO[materialIdx]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * bistro_exterior_n_triangles[materialIdx]);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glUseProgram(0);
}

// skybox
GLuint skybox_VBO, skybox_VAO;
GLuint skybox_texture_name;

GLfloat cube_vertices[72][3] = {
	// vertices enumerated clockwise
	  // 6*2*3 * 2 (POS & NORM)

	// position
	-1.0f,  1.0f, -1.0f,    1.0f,  1.0f, -1.0f,    1.0f,  1.0f,  1.0f, //right
	 1.0f,  1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f, //left
	 1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f,

	-1.0f, -1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f, //top
	 1.0f,  1.0f,  1.0f,    1.0f, -1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f, //bottom
	 1.0f, -1.0f, -1.0f,    1.0f,  1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,   -1.0f, -1.0f, -1.0f,   -1.0f,  1.0f, -1.0f, //back
	-1.0f,  1.0f, -1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,    1.0f, -1.0f,  1.0f,    1.0f,  1.0f,  1.0f, //front
	 1.0f,  1.0f,  1.0f,    1.0f,  1.0f, -1.0f,    1.0f, -1.0f, -1.0f,

	 // normal
	 0.0f, 0.0f, -1.0f,      0.0f, 0.0f, -1.0f,     0.0f, 0.0f, -1.0f,
	 0.0f, 0.0f, -1.0f,      0.0f, 0.0f, -1.0f,     0.0f, 0.0f, -1.0f,

	-1.0f, 0.0f,  0.0f,     -1.0f, 0.0f,  0.0f,    -1.0f, 0.0f,  0.0f,
	-1.0f, 0.0f,  0.0f,     -1.0f, 0.0f,  0.0f,    -1.0f, 0.0f,  0.0f,

	 1.0f, 0.0f,  0.0f,      1.0f, 0.0f,  0.0f,     1.0f, 0.0f,  0.0f,
	 1.0f, 0.0f,  0.0f,      1.0f, 0.0f,  0.0f,     1.0f, 0.0f,  0.0f,

	 0.0f, 0.0f, 1.0f,      0.0f, 0.0f, 1.0f,     0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,      0.0f, 0.0f, 1.0f,     0.0f, 0.0f, 1.0f,

	 0.0f, 1.0f, 0.0f,      0.0f, 1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,      0.0f, 1.0f, 0.0f,     0.0f, 1.0f, 0.0f,

	 0.0f, -1.0f, 0.0f,      0.0f, -1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
	 0.0f, -1.0f, 0.0f,      0.0f, -1.0f, 0.0f,     0.0f, -1.0f, 0.0f
};

void readTexImage2DForCubeMap(const char* filename, GLenum texture_target) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	//fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);

	width = FreeImage_GetWidth(tx_pixmap);
	height = FreeImage_GetHeight(tx_pixmap);
	FreeImage_FlipVertical(tx_pixmap);
	data = FreeImage_GetBits(tx_pixmap);

	glTexImage2D(texture_target, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	//fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap);
}

void prepare_skybox(void) { // Draw skybox.
	glGenVertexArrays(1, &skybox_VAO);
	glGenBuffers(1, &skybox_VBO);

	glBindVertexArray(skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GLfloat), &cube_vertices[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenTextures(1, &skybox_texture_name);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_name);

	readTexImage2DForCubeMap("Scene/Cubemap/px.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	readTexImage2DForCubeMap("Scene/Cubemap/nx.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	readTexImage2DForCubeMap("Scene/Cubemap/py.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	readTexImage2DForCubeMap("Scene/Cubemap/ny.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	readTexImage2DForCubeMap("Scene/Cubemap/pz.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	readTexImage2DForCubeMap("Scene/Cubemap/nz.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	fprintf(stdout, " * Loaded cube map textures into graphics memory.\n\n");

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void draw_skybox(void) {
	glUseProgram(h_ShaderProgram_skybox);

	glUniform1i(loc_cubemap_skybox, TEXTURE_INDEX_SKYMAP);

	ModelViewMatrix = ViewMatrix * glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(20000, 20000, 20000));
	//ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(20000.0f, 20000.0f, 20000.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_SKY, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(skybox_VAO);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_INDEX_SKYMAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_name);

	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 2 * 3);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glUseProgram(0);
}



/************************************* tiger start ******************************/
/************************************* tiger start ******************************/
/************************************* tiger start ******************************/
/************************************* tiger start ******************************/
/************************************* tiger start ******************************/

bool zoom;
bool xaxis, yaxis, zaxis;
bool t, g;

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2

// tiger object
#define N_TIGER_FRAMES 12
GLuint tiger_VBO, tiger_VAO;
int tiger_n_triangles[N_TIGER_FRAMES];
int tiger_vertex_offset[N_TIGER_FRAMES];
GLfloat* tiger_vertices[N_TIGER_FRAMES];

// ironman object
GLuint ironman_VBO, ironman_VAO;
int ironman_n_triangles;
GLfloat* ironman_vertices;

// optimus object
GLuint optimus_VBO, optimus_VAO;
int optimus_n_triangles;
GLfloat* optimus_vertices;

// spider object
#define N_SPIDER_FRAMES 16
GLuint spider_VBO, spider_VAO;
int spider_n_triangles[N_SPIDER_FRAMES];
int spider_vertex_offset[N_SPIDER_FRAMES];
GLfloat* spider_vertices[N_SPIDER_FRAMES];

// ben object
#define N_BEN_FRAMES 30
GLuint ben_VBO, ben_VAO;
int ben_n_triangles[N_BEN_FRAMES];
int ben_vertex_offset[N_BEN_FRAMES];
GLfloat* ben_vertices[N_BEN_FRAMES];

// bus object
GLuint bus_VBO, bus_VAO;
int bus_n_triangles;
GLfloat* bus_vertices;


// tank object
GLuint tank_VBO, tank_VAO;
int tank_n_triangles;
GLfloat* tank_vertices;

// wolf object
#define N_WOLF_FRAMES 17
GLuint wolf_VBO, wolf_VAO;
int wolf_n_triangles[N_WOLF_FRAMES];
int wolf_vertex_offset[N_WOLF_FRAMES];
GLfloat* wolf_vertices[N_WOLF_FRAMES];

//Material_Parameters material_tiger;

// for tiger animation
unsigned int timestamp_scene = 0, timestamp_tiger = 0;// the global clock in the scene
int flag_tiger_animation, flag_polygon_fill;
int cur_frame_tiger = 0, cur_frame_ben = 0, cur_frame_wolf = 0, cur_frame_spider = 0, idx_ben = 0, idx_tmp = 0, idx_wolf = 0, flag = 0;
float rotation_angle_tiger = 0.0f;

int read_geometry_20181625(GLfloat** object, int bytes_per_primitive, char* filename) {
	int n_triangles;
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}
	fread(&n_triangles, sizeof(int), 1, fp);

	*object = (float*)malloc(n_triangles * bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	fread(*object, bytes_per_primitive, n_triangles, fp);
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

void prepare_spider_20181625(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, spider_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_SPIDER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/spider/spider_vnt_%d%d.geom", i / 10, i % 10);
		spider_n_triangles[i] = read_geometry_20181625(&spider_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		spider_n_total_triangles += spider_n_triangles[i];

		if (i == 0)
			spider_vertex_offset[i] = 0;
		else
			spider_vertex_offset[i] = spider_vertex_offset[i - 1] + 3 * spider_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &spider_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glBufferData(GL_ARRAY_BUFFER, spider_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_SPIDER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, spider_vertex_offset[i] * n_bytes_per_vertex,
			spider_n_triangles[i] * n_bytes_per_triangle, spider_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_SPIDER_FRAMES; i++)
		free(spider_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &spider_VAO);
	glBindVertexArray(spider_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_ben.ambient_color[0] = 0.24725f;
	//material_ben.ambient_color[1] = 0.1995f;
	//material_ben.ambient_color[2] = 0.0745f;
	//material_ben.ambient_color[3] = 1.0f;
	//
	//material_ben.diffuse_color[0] = 0.75164f;
	//material_ben.diffuse_color[1] = 0.60648f;
	//material_ben.diffuse_color[2] = 0.22648f;
	//material_ben.diffuse_color[3] = 1.0f;
	//
	//material_ben.specular_color[0] = 0.728281f;
	//material_ben.specular_color[1] = 0.655802f;
	//material_ben.specular_color[2] = 0.466065f;
	//material_ben.specular_color[3] = 1.0f;
	//
	//material_ben.specular_exponent = 51.2f;
	//
	//material_ben.emissive_color[0] = 0.1f;
	//material_ben.emissive_color[1] = 0.1f;
	//material_ben.emissive_color[2] = 0.0f;
	//material_ben.emissive_color[3] = 1.0f;

	//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	//
	//glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	//glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);
	//
	//My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");
	//
	//glGenerateMipmap(GL_TEXTURE_2D);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void prepare_wolf_20181625(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, wolf_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_WOLF_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/wolf/wolf_%02d_vnt.geom", i);
		wolf_n_triangles[i] = read_geometry_20181625(&wolf_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		wolf_n_total_triangles += wolf_n_triangles[i];

		if (i == 0)
			wolf_vertex_offset[i] = 0;
		else
			wolf_vertex_offset[i] = wolf_vertex_offset[i - 1] + 3 * wolf_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &wolf_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glBufferData(GL_ARRAY_BUFFER, wolf_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_WOLF_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, wolf_vertex_offset[i] * n_bytes_per_vertex,
			wolf_n_triangles[i] * n_bytes_per_triangle, wolf_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_WOLF_FRAMES; i++)
		free(wolf_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &wolf_VAO);
	glBindVertexArray(wolf_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_wolf.ambient_color[0] = 0.24725f;
	//material_wolf.ambient_color[1] = 0.1995f;
	//material_wolf.ambient_color[2] = 0.0745f;
	//material_wolf.ambient_color[3] = 1.0f;
	//
	//material_wolf.diffuse_color[0] = 0.75164f;
	//material_wolf.diffuse_color[1] = 0.60648f;
	//material_wolf.diffuse_color[2] = 0.22648f;
	//material_wolf.diffuse_color[3] = 1.0f;
	//
	//material_wolf.specular_color[0] = 0.728281f;
	//material_wolf.specular_color[1] = 0.655802f;
	//material_wolf.specular_color[2] = 0.466065f;
	//material_wolf.specular_color[3] = 1.0f;
	//
	//material_wolf.specular_exponent = 51.2f;
	//
	//material_wolf.emissive_color[0] = 0.1f;
	//material_wolf.emissive_color[1] = 0.1f;
	//material_wolf.emissive_color[2] = 0.0f;
	//material_wolf.emissive_color[3] = 1.0f;

	//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	//
	//glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	//glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);
	//
	//My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");
	//
	//glGenerateMipmap(GL_TEXTURE_2D);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}



void prepare_optimus_20181625(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, optimus_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/optimus_vnt.geom");
	optimus_n_triangles = read_geometry_20181625(&optimus_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	optimus_n_total_triangles += optimus_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &optimus_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, optimus_VBO);
	glBufferData(GL_ARRAY_BUFFER, optimus_n_total_triangles * 3 * n_bytes_per_vertex, optimus_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(optimus_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &optimus_VAO);
	glBindVertexArray(optimus_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, optimus_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_optimus.ambient_color[0] = 0.24725f;
	//material_optimus.ambient_color[1] = 0.1995f;
	//material_optimus.ambient_color[2] = 0.0745f;
	//material_optimus.ambient_color[3] = 1.0f;
	//
	//material_optimus.diffuse_color[0] = 0.75164f;
	//material_optimus.diffuse_color[1] = 0.60648f;
	//material_optimus.diffuse_color[2] = 0.22648f;
	//material_optimus.diffuse_color[3] = 1.0f;
	//
	//material_optimus.specular_color[0] = 0.728281f;
	//material_optimus.specular_color[1] = 0.655802f;
	//material_optimus.specular_color[2] = 0.466065f;
	//material_optimus.specular_color[3] = 1.0f;
	//
	//material_optimus.specular_exponent = 51.2f;
	//
	//material_optimus.emissive_color[0] = 0.1f;
	//material_optimus.emissive_color[1] = 0.1f;
	//material_optimus.emissive_color[2] = 0.0f;
	//material_optimus.emissive_color[3] = 1.0f;

	//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	//
	//glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	//glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);
	//
	//My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");
	//
	//glGenerateMipmap(GL_TEXTURE_2D);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}



void prepare_tiger_20181625(void) { // vertices enumerated clockwise
	int i, n_bytes_per_vertex, n_bytes_per_triangle, tiger_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/tiger/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		tiger_n_triangles[i] = read_geometry_20181625(&tiger_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		tiger_n_total_triangles += tiger_n_triangles[i];

		if (i == 0)
			tiger_vertex_offset[i] = 0;
		else
			tiger_vertex_offset[i] = tiger_vertex_offset[i - 1] + 3 * tiger_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &tiger_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glBufferData(GL_ARRAY_BUFFER, tiger_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_TIGER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, tiger_vertex_offset[i] * n_bytes_per_vertex,
			tiger_n_triangles[i] * n_bytes_per_triangle, tiger_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_TIGER_FRAMES; i++)
		free(tiger_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &tiger_VAO);
	glBindVertexArray(tiger_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/*material_tiger.ambient_color[0] = 0.24725f;
	material_tiger.ambient_color[1] = 0.1995f;
	material_tiger.ambient_color[2] = 0.0745f;
	material_tiger.ambient_color[3] = 1.0f;

	material_tiger.diffuse_color[0] = 0.75164f;
	material_tiger.diffuse_color[1] = 0.60648f;
	material_tiger.diffuse_color[2] = 0.22648f;
	material_tiger.diffuse_color[3] = 1.0f;

	material_tiger.specular_color[0] = 0.728281f;
	material_tiger.specular_color[1] = 0.655802f;
	material_tiger.specular_color[2] = 0.466065f;
	material_tiger.specular_color[3] = 1.0f;

	material_tiger.specular_exponent = 51.2f;

	material_tiger.emissive_color[0] = 0.1f;
	material_tiger.emissive_color[1] = 0.1f;
	material_tiger.emissive_color[2] = 0.0f;
	material_tiger.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);

	My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
}

void prepare_tank_20181625(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, tank_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/tank_vnt.geom");
	tank_n_triangles = read_geometry_20181625(&tank_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	tank_n_total_triangles += tank_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &tank_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, tank_VBO);
	glBufferData(GL_ARRAY_BUFFER, tank_n_total_triangles * 3 * n_bytes_per_vertex, tank_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(tank_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &tank_VAO);
	glBindVertexArray(tank_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tank_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_tank.ambient_color[0] = 0.24725f;
	//material_tank.ambient_color[1] = 0.1995f;
	//material_tank.ambient_color[2] = 0.0745f;
	//material_tank.ambient_color[3] = 1.0f;
	//
	//material_tank.diffuse_color[0] = 0.75164f;
	//material_tank.diffuse_color[1] = 0.60648f;
	//material_tank.diffuse_color[2] = 0.22648f;
	//material_tank.diffuse_color[3] = 1.0f;
	//
	//material_tank.specular_color[0] = 0.728281f;
	//material_tank.specular_color[1] = 0.655802f;
	//material_tank.specular_color[2] = 0.466065f;
	//material_tank.specular_color[3] = 1.0f;
	//
	//material_tank.specular_exponent = 51.2f;
	//
	//material_tank.emissive_color[0] = 0.1f;
	//material_tank.emissive_color[1] = 0.1f;
	//material_tank.emissive_color[2] = 0.0f;
	//material_tank.emissive_color[3] = 1.0f;

	//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	//
	//glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	//glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);
	//
	//My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");
	//
	//glGenerateMipmap(GL_TEXTURE_2D);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void prepare_bus_20181625(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, bus_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/bus_vnt.geom");
	bus_n_triangles = read_geometry_20181625(&bus_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	bus_n_total_triangles += bus_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &bus_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glBufferData(GL_ARRAY_BUFFER, bus_n_total_triangles * 3 * n_bytes_per_vertex, bus_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(bus_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &bus_VAO);
	glBindVertexArray(bus_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_bus.ambient_color[0] = 0.24725f;
	//material_bus.ambient_color[1] = 0.1995f;
	//material_bus.ambient_color[2] = 0.0745f;
	//material_bus.ambient_color[3] = 1.0f;
	//
	//material_bus.diffuse_color[0] = 0.75164f;
	//material_bus.diffuse_color[1] = 0.60648f;
	//material_bus.diffuse_color[2] = 0.22648f;
	//material_bus.diffuse_color[3] = 1.0f;
	//
	//material_bus.specular_color[0] = 0.728281f;
	//material_bus.specular_color[1] = 0.655802f;
	//material_bus.specular_color[2] = 0.466065f;
	//material_bus.specular_color[3] = 1.0f;
	//
	//material_bus.specular_exponent = 51.2f;
	//
	//material_bus.emissive_color[0] = 0.1f;
	//material_bus.emissive_color[1] = 0.1f;
	//material_bus.emissive_color[2] = 0.0f;
	//material_bus.emissive_color[3] = 1.0f;

	//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	//
	//glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	//glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);
	//
	//My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");
	//
	//glGenerateMipmap(GL_TEXTURE_2D);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void prepare_ironman_20181625(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, ironman_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/ironman_vnt.geom");
	ironman_n_triangles = read_geometry_20181625(&ironman_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	ironman_n_total_triangles += ironman_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &ironman_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, ironman_VBO);
	glBufferData(GL_ARRAY_BUFFER, ironman_n_total_triangles * 3 * n_bytes_per_vertex, ironman_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(ironman_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &ironman_VAO);
	glBindVertexArray(ironman_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ironman_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_ironman.ambient_color[0] = 0.24725f;
	//material_ironman.ambient_color[1] = 0.1995f;
	//material_ironman.ambient_color[2] = 0.0745f;
	//material_ironman.ambient_color[3] = 1.0f;
	//
	//material_ironman.diffuse_color[0] = 0.75164f;
	//material_ironman.diffuse_color[1] = 0.60648f;
	//material_ironman.diffuse_color[2] = 0.22648f;
	//material_ironman.diffuse_color[3] = 1.0f;
	//
	//material_ironman.specular_color[0] = 0.728281f;
	//material_ironman.specular_color[1] = 0.655802f;
	//material_ironman.specular_color[2] = 0.466065f;
	//material_ironman.specular_color[3] = 1.0f;
	//
	//material_ironman.specular_exponent = 51.2f;
	//
	//material_ironman.emissive_color[0] = 0.1f;
	//material_ironman.emissive_color[1] = 0.1f;
	//material_ironman.emissive_color[2] = 0.0f;
	//material_ironman.emissive_color[3] = 1.0f;

	//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	//
	//glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	//glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);
	//
	//My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");
	//
	//glGenerateMipmap(GL_TEXTURE_2D);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


void draw_optimus_20181625(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);
	glUseProgram(h_ShaderProgram_simple);
	glUniform3f(loc_primitive_color, 255.0f, 0.0f, 0.0f);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(1500.0f, -800.0f, 200.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -rotation_angle_tiger, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(200.0f, 0.0f, 0.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glBindVertexArray(optimus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * optimus_n_triangles);
	glBindVertexArray(0);
	glUseProgram(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void draw_optimus1_20181625(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);
	glUseProgram(h_ShaderProgram_simple);
	glUniform3f(loc_primitive_color, 0.0f, 0.0f, 255.0f);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-2000.0f, -1300.0f, 2500.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -rotation_angle_tiger, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(200.0f, 0.0f, 0.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glBindVertexArray(optimus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * optimus_n_triangles);
	glBindVertexArray(0);
	glUseProgram(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void draw_spider_20181625(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);
	glUseProgram(h_ShaderProgram_simple);
	glUniform3f(loc_primitive_color, 255.0f, 0.0f, 0.0f);

	//set_material_tiger();
	//glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	//ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(1208.0f, 3844.0f, 284.0f));
	if (idx_ben < spider_displacement.size()) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(spider_displacement[idx_ben].x, spider_displacement[idx_ben].y, spider_displacement[idx_ben].z));
	}
	else if (idx_ben >= spider_displacement.size() && idx_ben < spider_displacement.size() * 2) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(spider_displacement[idx_ben].x, spider_displacement[idx_ben].y, spider_displacement[idx_ben].z));
	}
	

	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(200.0f, 200.0f, 200.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -rotation_angle_tiger, glm::vec3(0.0f, 0.0f, 1.0f));
	//ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(200.0f, 0.0f, 0.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glBindVertexArray(spider_VAO);
	glDrawArrays(GL_TRIANGLES, spider_vertex_offset[cur_frame_spider], 3 * spider_n_triangles[cur_frame_spider]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);

}

void draw_tiger_20181625(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);
	glUseProgram(h_ShaderProgram_simple);
	glUniform3f(loc_primitive_color, 255.0f, 0.0f, 0.0f);

	//set_material_tiger();
	//glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(4600.0f, -2500.0f, 50.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -rotation_angle_tiger, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(200.0f, 0.0f, 0.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	//glUniformMatrix4fv(loc_ModelViewMatrix, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	//glUniformMatrix3fv(loc_ModelViewMatrixInvTrans, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	glBindVertexArray(tiger_VAO);
	glDrawArrays(GL_LINES, tiger_vertex_offset[cur_frame_tiger], 3 * tiger_n_triangles[cur_frame_tiger]);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(0);
}

void draw_wolf_20181625(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);
	glUseProgram(h_ShaderProgram_simple);
	glUniform3f(loc_primitive_color, 255.0f, 0.0f, 0.0f);

	//set_material_tiger();
	//glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(wolf_displacement[idx_wolf].x, wolf_displacement[idx_wolf].y, wolf_displacement[idx_wolf].z));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -rotation_angle_tiger, glm::vec3(0.0f, 1.0f, 0.0f));
	//ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(200.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(500.0f, 500.0f, 500.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	
	glBindVertexArray(wolf_VAO);
	glDrawArrays(GL_TRIANGLES, wolf_vertex_offset[cur_frame_wolf], 3 * wolf_n_triangles[cur_frame_wolf]);
	glBindVertexArray(0);
	glUseProgram(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void draw_bus_20181625(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);
	glUseProgram(h_ShaderProgram_simple);
	glUniform3f(loc_primitive_color, 227, 0, 34);
	ModelViewMatrix = glm::rotate(ViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(100.0f, -80.0f, 900.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(13.0f, 13.0f, 13.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 1.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -45.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glBindVertexArray(bus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * bus_n_triangles);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void draw_ironman_20181625(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);
	glUseProgram(h_ShaderProgram_simple);
	glUniform3f(loc_primitive_color, 227, 0, 34);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(4000.0f, -2500.0f, 50.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(150.0f, 150.0f, 150.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 1.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -45.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glBindVertexArray(ironman_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * ironman_n_triangles);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void draw_tank_20181625(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);
	glUseProgram(h_ShaderProgram_simple);
	glUniform3f(loc_primitive_color, 227, 0, 34);
	//ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-361.351807f, 350.346313f, 105.968231f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 1.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -45.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glBindVertexArray(tank_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tank_n_triangles);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/*****************************  END: geometry setup *****************************/

/********************  START: callback function definitions *********************/
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_grid();
	draw_axes();
	draw_bistro_exterior();
	draw_skybox();


	glLineWidth(3.0f);
	draw_tiger_20181625();
	draw_spider_20181625();
	draw_wolf_20181625();
	draw_ironman_20181625();
	draw_optimus_20181625();
	draw_optimus1_20181625();
	draw_tank_20181625();
	draw_bus_20181625();
	glutSwapBuffers();

}

void timer_tiger_20181625(int value) {
	timestamp_tiger = (timestamp_tiger + 1) % UINT_MAX;
	cur_frame_tiger = timestamp_tiger % N_TIGER_FRAMES;
	rotation_angle_tiger = (timestamp_tiger % 360) * TO_RADIAN;
	if (t) {
		//set_current_camera(CAMERA_1);
		ViewMatrix = glm::mat4(current_camera.uaxis[0], current_camera.vaxis[0], current_camera.naxis[0], 0.0f,
			current_camera.uaxis[1], current_camera.vaxis[1], current_camera.naxis[1], 0.0f,
			current_camera.uaxis[2], current_camera.vaxis[2], current_camera.naxis[2], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		//ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -88.0f, 62.0f));
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-200.0f, 0.0f, 0.0f));
		ViewMatrix = glm::rotate(ViewMatrix, rotation_angle_tiger + 1, glm::vec3(0.0f, 0.0f, 1.0f));
		ViewMatrix = glm::scale(ViewMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
		if (((int)rotation_angle_tiger) % 2 == 0)
			ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-4600.0f, 2500.0f, -350.0f));
		else if (((int)rotation_angle_tiger) % 2 == 1)
			ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-4600.0f, 2500.0f, -310.0f));
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, -88.0f, 62.0f));
		//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	}
	else if (g) {
		set_current_camera(CAMERA_1);
		ViewMatrix = glm::mat4(current_camera.uaxis[0], current_camera.vaxis[0], current_camera.naxis[0], 0.0f,
			current_camera.uaxis[1], current_camera.vaxis[1], current_camera.naxis[1], 0.0f,
			current_camera.uaxis[2], current_camera.vaxis[2], current_camera.naxis[2], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-200.0f, 0.0f, 0.0f));
		ViewMatrix = glm::rotate(ViewMatrix, rotation_angle_tiger - 1, glm::vec3(0.0f, 0.0f, 1.0f));
		ViewMatrix = glm::scale(ViewMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-4600.0f, 2500.0f, -450.0f));
		//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	}
	glutPostRedisplay();
	if (flag_tiger_animation)
		glutTimerFunc(10, timer_tiger_20181625, 0);
}

void timer_scene_20181625(int value) {
	timestamp_scene = (timestamp_scene + 1) % UINT_MAX;
	idx_ben = timestamp_scene % 86;
	if (idx_ben < spider_displacement.size())
		idx_tmp = 0;
	if (idx_ben >= spider_displacement.size() - 1) {
		idx_ben = spider_displacement.size() - 1 - idx_tmp;
		idx_tmp++;
	}

	flag++;

	if (flag == 5) {
		idx_wolf++;
		if (idx_wolf > wolf_displacement.size()-1)
			idx_wolf = 0;
		flag = 0;
	}

	//cur_frame_ben = timestamp_scene % N_BEN_FRAMES;
	cur_frame_wolf = timestamp_scene % N_WOLF_FRAMES;
	cur_frame_spider = timestamp_scene % N_SPIDER_FRAMES;


	
	glutPostRedisplay();
	glutTimerFunc(10, timer_scene_20181625, 0);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'f':
		b_draw_grid = b_draw_grid ? false : true;
		glutPostRedisplay();
		break;
	case 's':
		flag_tiger_animation = 1 - flag_tiger_animation;
		if (flag_tiger_animation) {
			glutTimerFunc(10, timer_tiger_20181625, 0);
			fprintf(stdout, "^^^ Animation mode ON.\n");
		}
		else
			fprintf(stdout, "^^^ Animation mode OFF.\n");
		break;
	case 'u':
		t = false;
		g = false;
		set_current_camera(CAMERA_U);
		glutPostRedisplay();
		break;
	case 'i':
		t = false;
		g = false;
		set_current_camera(CAMERA_I);
		glutPostRedisplay();
		break;
	case 'o':
		t = false;
		g = false;
		set_current_camera(CAMERA_O);
		glutPostRedisplay();
		break;
	case 'p':
		t = false;
		g = false;
		set_current_camera(CAMERA_P);
		glutPostRedisplay();
		break;
	case 't':
		if (t == false) {
			t = true;
			set_current_camera(CAMERA_1);
		}
		g = false;
		ViewMatrix = glm::mat4(current_camera.uaxis[0], current_camera.vaxis[0], current_camera.naxis[0], 0.0f,
			current_camera.uaxis[1], current_camera.vaxis[1], current_camera.naxis[1], 0.0f,
			current_camera.uaxis[2], current_camera.vaxis[2], current_camera.naxis[2], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-200.0f, 0.0f, 0.0f));
		ViewMatrix = glm::rotate(ViewMatrix, rotation_angle_tiger+1, glm::vec3(0.0f, 0.0f, 1.0f));
		ViewMatrix = glm::scale(ViewMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
		if(((int)rotation_angle_tiger) % 2 == 0)
			ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-4600.0f, 2500.0f, -350.0f));
		else if(((int)rotation_angle_tiger) % 2 == 1)
			ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-4600.0f, 2500.0f, -310.0f));
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, -88.0f, 62.0f));
		//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
		break;
	case 'g':
		if (g == false)
			g = true;
		t = false;
		set_current_camera(CAMERA_1);
		ViewMatrix = glm::mat4(current_camera.uaxis[0], current_camera.vaxis[0], current_camera.naxis[0], 0.0f,
			current_camera.uaxis[1], current_camera.vaxis[1], current_camera.naxis[1], 0.0f,
			current_camera.uaxis[2], current_camera.vaxis[2], current_camera.naxis[2], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-200.0f, 0.0f, 0.0f));
		ViewMatrix = glm::rotate(ViewMatrix, rotation_angle_tiger-1, glm::vec3(0.0f, 0.0f, 1.0f));
		ViewMatrix = glm::scale(ViewMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-4600.0f, 2500.0f, -350.0f));
		//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
		break;
	case 'a':
		set_current_camera(CAMERA_A);
		glutPostRedisplay();
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

void special(int key, int x, int y) {
	float cameraSpeed = 50.0f;
	int modifier = glutGetModifiers();
	if (modifier & GLUT_ACTIVE_CTRL) {
		zaxis = true;

	}
	else {
		zaxis = false;

	}

	if (modifier & GLUT_ACTIVE_SHIFT)
	{
		yaxis = true;
	}
	else {
		yaxis = false;
	}

	if (modifier & GLUT_ACTIVE_ALT)
	{
		xaxis = true;
	}
	else {
		xaxis = false;
	}
	if (current_camera.move == 1) {
		switch (key) {
		case GLUT_KEY_UP:
			//fprintf(stdout, "%f\n", current_camera.pos[1] * cameraSpeed);
			current_camera.pos[1] -= cameraSpeed;
			set_ViewMatrix_from_camera_frame();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			//fprintf(stdout, "%f\n", current_camera.pos[1] * cameraSpeed);
			current_camera.pos[1] += cameraSpeed;
			set_ViewMatrix_from_camera_frame();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:
			//fprintf(stdout, "%f\n", current_camera.pos[1] * cameraSpeed);
			current_camera.pos[0] += cameraSpeed;
			set_ViewMatrix_from_camera_frame();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			//fprintf(stdout, "%f\n", current_camera.pos[1] * cameraSpeed);
			current_camera.pos[0] -= cameraSpeed;
			set_ViewMatrix_from_camera_frame();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
			break;
		case GLUT_KEY_PAGE_UP:
			current_camera.pos[2] += cameraSpeed;
			set_ViewMatrix_from_camera_frame();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
			break;
		case GLUT_KEY_PAGE_DOWN:
			current_camera.pos[2] -= cameraSpeed;
			set_ViewMatrix_from_camera_frame();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
			break;
		}

	}


	fprintf(stdout, "%f %f %f \n", current_camera.pos[0], current_camera.pos[1], current_camera.pos[2]);
	fprintf(stdout, "%f %f %f \n", current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);
	fprintf(stdout, "%f %f %f \n", current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]);
	fprintf(stdout, "%f %f %f \n", current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
	fprintf(stdout, "\n\n");
	glutPostRedisplay();
}

void mousewheel_20181625(int wheel, int direction, int x, int y) {
	int fovtmp = 1;
	int modifier = glutGetModifiers();
	if (modifier & GLUT_ACTIVE_CTRL) {
		zoom = true;
	}
	else {
		zoom = false;

	}
	if (zoom) {
		if (direction > 0) {
			current_camera.fovy -= 0.1f;
			fprintf(stdout, "%f\n", current_camera.fovy);
			if (current_camera.fovy < 1.0f * TO_RADIAN)
				current_camera.fovy = 1.0f * TO_RADIAN;
			set_ViewMatrix_from_camera_frame();
			ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
			fprintf(stdout, "Zoom in\n");
		}
		else {
			current_camera.fovy += 0.1f;
			fprintf(stdout, "%f\n", current_camera.fovy);
			if (current_camera.fovy > 90.0f * TO_RADIAN)
				current_camera.fovy = 90.0f * TO_RADIAN;
			set_ViewMatrix_from_camera_frame();
			ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
			fprintf(stdout, "Zoom out\n");
		}
		//glutPostRedisplay();
	}
}

void mouseevent_20181625(int button, int state, int x, int y) {
	if ((state == GLUT_DOWN) && button == GLUT_LEFT_BUTTON) {
		leftbuttonpressed = 1;
	}
	else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
		leftbuttonpressed = 0;

	if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) {
		rightbuttonpressed = 1;
	}
	else if (state == GLUT_UP && button == GLUT_RIGHT_BUTTON){
		rightbuttonpressed = 0;
	}

	if (state == GLUT_DOWN && button == GLUT_MIDDLE_BUTTON) {
		center_selected = 1;
	}
	else if (state == GLUT_UP && button == GLUT_MIDDLE_BUTTON){
		center_selected = 0;
	}
}

void mouse_20181625(int x, int y) {
	int dx = prevx - x;
	int dy = prevy - y;
	
	float sensitivity = 0.5f;

	//ViewMatrix = glm::mat4(current_camera.uaxis[0], current_camera.vaxis[0], current_camera.naxis[0], 0.0f,
	//	current_camera.uaxis[1], current_camera.vaxis[1], current_camera.naxis[1], 0.0f,
	//	current_camera.uaxis[2], current_camera.vaxis[2], current_camera.naxis[2], 0.0f,
	//	0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat3 Rotation;
	glm::vec3 updatevector;
	if (rightbuttonpressed && current_camera.move == 1) {
		//ViewMatrix = glm::rotate(ViewMatrix, -1 * sensitivity * dy * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		Rotation = glm::mat3(glm::rotate(glm::mat4(1.0f), sensitivity * dy * TO_RADIAN, glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2])));
		updatevector = Rotation * glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]);
		current_camera.vaxis[0] = updatevector.x; current_camera.vaxis[1] = updatevector.y; current_camera.vaxis[2] = updatevector.z;
		updatevector = Rotation * glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
		current_camera.naxis[0] = updatevector.x; current_camera.naxis[1] = updatevector.y; current_camera.naxis[2] = updatevector.z;
	}
	if (center_selected && current_camera.move == 1){
		//ViewMatrix = glm::rotate(ViewMatrix, sensitivity * dx * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		Rotation = glm::mat3(glm::rotate(glm::mat4(1.0f), sensitivity * dx * TO_RADIAN, glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2])));
		updatevector = Rotation * glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);
		current_camera.uaxis[0] = updatevector.x; current_camera.uaxis[1] = updatevector.y; current_camera.uaxis[2] = updatevector.z;
		updatevector = Rotation * glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]);
		current_camera.vaxis[0] = updatevector.x; current_camera.vaxis[1] = updatevector.y; current_camera.vaxis[2] = updatevector.z;
	}
	if (leftbuttonpressed && current_camera.move == 1) {
		//ViewMatrix = glm::rotate(ViewMatrix, sensitivity * dx * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	
		Rotation = glm::mat3(glm::rotate(glm::mat4(1.0f), sensitivity * dx * TO_RADIAN, glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2])));
		updatevector = Rotation * glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);
		current_camera.uaxis[0] = updatevector.x; current_camera.uaxis[1] = updatevector.y; current_camera.uaxis[2] = updatevector.z;
		updatevector = Rotation * glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
		current_camera.naxis[0] = updatevector.x; current_camera.naxis[1] = updatevector.y; current_camera.naxis[2] = updatevector.z;
	}
	//glm::mat3 rotationMatrix = glm::mat3(ViewMatrix);
	//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-current_camera.pos[0], -current_camera.pos[1], -current_camera.pos[2]));
	//ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
	//ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	//fprintf(stdout, "%f %f %f\n", ViewMatrix[0][0], ViewMatrix[0][1], ViewMatrix[0][2]);
	prevx = x; prevy = y;
	set_ViewMatrix_from_camera_frame();
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glutPostRedisplay();
}

void reshape(int width, int height) {
	window_width = width;
	window_height = height; 
	float aspect_ratio;

	glViewport(0, 0, width, height);

	ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(1, &grid_VAO);
	glDeleteBuffers(1, &grid_VBO);

	glDeleteVertexArrays(scene.n_materials, bistro_exterior_VAO);
	glDeleteBuffers(scene.n_materials, bistro_exterior_VBO);
	glDeleteTextures(scene.n_textures, bistro_exterior_texture_names);

	glDeleteVertexArrays(1, &skybox_VAO);
	glDeleteBuffers(1, &skybox_VBO);

	free(bistro_exterior_n_triangles);
	free(bistro_exterior_vertex_offset);

	free(bistro_exterior_VAO);
	free(bistro_exterior_VBO);

	free(bistro_exterior_texture_names);
	free(flag_texture_mapping);
}
/*********************  END: callback function definitions **********************/

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouseevent_20181625);
	glutMotionFunc(mouse_20181625);
	//glutPassiveMotionFunc(mouse);
	glutMouseWheelFunc(mousewheel_20181625);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_tiger_20181625, 0);
	glutTimerFunc(100, timer_scene_20181625, 0);
	glutCloseFunc(cleanup);
}

void initialize_flags_20181625(void) {
	flag_tiger_animation = 1;
	flag_polygon_fill = 1;
	//flag_texture_mapping = 1;
	//flag_fog = 0;

	//glUseProgram(h_ShaderProgram_TXPS);
	//glUniform1i(loc_flag_fog, flag_fog);
	//glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
	glUseProgram(0);
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix = glm::mat4(1.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	initialize_lights();
	initialize_flags_20181625();
}

void prepare_scene(void) {
	prepare_axes();
	prepare_grid();
	prepare_bistro_exterior();
	prepare_skybox();
	prepare_tiger_20181625();
	prepare_wolf_20181625();
	prepare_ironman_20181625();
	prepare_optimus_20181625();
	prepare_tank_20181625();
	prepare_bus_20181625();
	prepare_spider_20181625();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
	initialize_camera();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "********************************************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "********************************************************************************\n\n");
}

void print_message(const char* m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "********************************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n********************************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 9
void drawScene(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 Bistro Exterior Scene";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used:",
		"		'f' : draw x, y, z axes and grid",
		"		'1' : set the camera for original view",
		"		'2' : set the camera for bistro view",
		"		'3' : set the camera for tree view",
		"		'4' : set the camera for top view",
		"		'5' : set the camera for front view",
		"		'6' : set the camera for side view",
		"		'ESC' : program close",
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(900, 600);
	glutInitWindowPosition(20, 20);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
