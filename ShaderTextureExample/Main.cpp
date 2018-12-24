#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <iostream>

#define PI 3.14159265359f

using namespace std;

struct VertexAttribute {
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};

int width = 800;
int height = 600;
const GLdouble X = 0.5;
const int earth_slices = 360;
const int earth_stacks = 180;
const int buffer_size = earth_slices * earth_stacks * 6;
const GLdouble earth_radius = 1;
const GLdouble earth_rot_speed = X;
const GLfloat light_r = 3;
const GLfloat light_rev_speed = X / 10.0;

int pause = 0;
int texture_on = 0;
int normal_on = 0;
int specular_on = 0;
GLdouble earth_rot = 0;
GLfloat light_rev = 0;
unsigned int earth_texture;
unsigned int earth_normal;
unsigned int earth_specular;

GLuint program;
GLuint vboName;

bool start;

VertexAttribute *drawPlane();
void display();
void reshape(GLsizei w, GLsizei h);
void init();
void LoadTexture(char* pFilename, unsigned int &textObj);

VertexAttribute *mySphere(int slice, int stack) {
	VertexAttribute vertices[4];
	VertexAttribute *buf = new VertexAttribute[slice * stack * 6];
	int buf_size = 0;
	int direction[4][2] = { { 0, 0 },{ 1, 0 },{ 0, 1 },{ 1, 1 } };
	int index[6] = { 2, 1, 0, 2, 3, 1 };
	float slice_step = 2 * PI / slice, stack_step = PI / stack;
	for (int i = 0; i < slice; i++) {
		// glBegin(GL_TRIANGLES);
		for (int j = 0; j < stack; j++) {
			for (int k = 0; k < 4; k++) {
				vertices[k].position[0] = vertices[k].normal[0] = sin((j + direction[k][1]) * stack_step) * cos((i + direction[k][0])*slice_step);
				vertices[k].position[1] = vertices[k].normal[1] = cos((j + direction[k][1]) * stack_step);
				vertices[k].position[2] = vertices[k].normal[2] = sin((j + direction[k][1]) * stack_step) * sin((i + direction[k][0])*slice_step);
				vertices[k].texcoord[0] = 1.0 - float(i + direction[k][0]) / (float) slice;
				vertices[k].texcoord[1] = 1.0 - float(j + direction[k][1]) / (float) stack;
			}
			for (int k = 0; k < 6; k++) {
				// glNormal3fv(vertices[index[k]].normal);
				// glTexCoord2fv(vertices[index[k]].texcoord);
				// glVertex3fv(vertices[index[k]].position);
				buf[buf_size++] = vertices[index[k]];
			}
		}
		// glEnd();
	}
	return buf;
}

void idle() {
	if (pause) return;
	earth_rot += earth_rot_speed;
	while (earth_rot >= 360.0)
		earth_rot -= 360.0;
	light_rev += light_rev_speed;
	while (light_rev >= 360.0)
		light_rev -= 360.0;
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1': texture_on ^= 1; break;
	case '2': normal_on ^= 1;  break;
	case '3': specular_on ^= 1;  break;
	case 'p': pause ^= 1; break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");
	glewInit();

	init();

	glGenTextures(1, &earth_texture);
	LoadTexture("earth_texture_map.jpg", earth_texture);
	glGenTextures(1, &earth_normal);
	LoadTexture("earth_normal_map.tif", earth_normal);
	glGenTextures(1, &earth_specular);
	LoadTexture("earth_specular_map.tif", earth_specular);


	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}

void init() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint geom = createShader("Shaders/example.geom", "geometry");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, geom, frag);

	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	VertexAttribute *vertices;
	vertices = mySphere(360, 180);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * 360 * 180 * 6, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void lights() {
	// glEnable(GL_LIGHTING);
	GLfloat light_x = light_r * cos(light_rev / 180.0 * PI);
	GLfloat light_z = light_r * sin(light_rev / 180.0 * PI);
	GLfloat light_position[] = { light_x, 0, light_z };
	GLint lightLoc = glGetUniformLocation(program, "lightPos");
	glUniform3fv(lightLoc, 1, light_position);
	// GLfloat light_diffuse[] = { 1, 1, 1, 1 };
	// GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1 };
	// glEnable(GL_LIGHT0);
	// glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	// glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	// glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void display()
{
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewport transformation
	glViewport(0, 0, width, height);
	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)width / (GLfloat)height, 1, 500);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	lights();
	gluLookAt(0.0f, 0.0f, 3.0f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f);// up

	GLint viewLoc = glGetUniformLocation(program, "viewPos");
	glUniform3f(viewLoc, 0.0f, 0.0f, 3.0f);

	glRotated(-23.5, 0, 0, 1);
	glRotated(earth_rot, 0, 1, 0);

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "model");
	GLint texLoc = glGetUniformLocation(program, "diffuseMap");

	glEnable(GL_TEXTURE_2D);

	glUseProgram(program);

	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earth_texture);
	glUniform1i(texLoc, 0);

	glDrawArrays(GL_TRIANGLES, 0, 360 * 180 * 6);
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(0);

	glutSwapBuffers();

}

VertexAttribute *drawPlane() {
	VertexAttribute *vertices;
	vertices = new VertexAttribute[6];
	vertices[0].position[0] = 0.0f;	vertices[0].position[1] = 0.5f;	vertices[0].position[2] = 0.0f;
	vertices[1].position[0] = 1.0f;	vertices[1].position[1] = 0.5f;	vertices[1].position[2] = 0.0f;
	vertices[2].position[0] = 0.0f;	vertices[2].position[1] = 0.5f;	vertices[2].position[2] = 1.0f;
	vertices[3].position[0] = 1.0f;	vertices[3].position[1] = 0.5f;	vertices[3].position[2] = 0.0f;
	vertices[4].position[0] = 1.0f;	vertices[4].position[1] = 0.5f;	vertices[4].position[2] = 1.0f;
	vertices[5].position[0] = 0.0f;	vertices[5].position[1] = 0.5f;	vertices[5].position[2] = 1.0f;
	vertices[0].texcoord[0] = 1.0f;	vertices[0].texcoord[1] = 0.0f;
	vertices[1].texcoord[0] = 0.0f;	vertices[1].texcoord[1] = 0.0f;
	vertices[2].texcoord[0] = 1.0f;	vertices[2].texcoord[1] = 1.0f;
	vertices[3].texcoord[0] = 0.0f;	vertices[3].texcoord[1] = 0.0f;
	vertices[4].texcoord[0] = 0.0f;	vertices[4].texcoord[1] = 1.0f;
	vertices[5].texcoord[0] = 1.0f;	vertices[5].texcoord[1] = 1.0f;
	return vertices;
}

void reshape(GLsizei w, GLsizei h)
{
	width = w;
	height = h;
}

void LoadTexture(char* pFilename, unsigned int &textObj) {
	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(pFilename, 0), pFilename);
	FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	glBindTexture(GL_TEXTURE_2D, textObj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	float fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	glBindTexture(GL_TEXTURE_2D, 0);

	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
}