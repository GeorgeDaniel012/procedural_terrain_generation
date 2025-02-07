// Codul sursa este adaptat dupa OpenGLBook.com

#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h
#include "loadShaders.h"
#include "fastnoise/FastNoiseLite.h"
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <iostream>
#include "SOIL.h"			//	Biblioteca pentru texturare;

//////////////////////////////////////

const int targetFPS = 60;
const int frameInterval = 1000 / targetFPS;

GLuint
TerrainVaoId,
TerrainVboId,
SphereVaoId,
SphereVboId,
SphereEboId,
ColorBufferId,
ProgramId,
SphProgramId,
myMatrixLocation,
myMatrixLocation2,
viewLocation,
viewLocation2,
projLocation,
projLocation2,
viewPosLocation,
viewPosLocation2,
heightMapLocation,
isDarkLocation,
isDarkLocation2,
lightPosLoc,
uTexelSizeLocation,
heightScaleLocation,
texture;

float PI = 3.141592;
const int height = 1024, width = 1024;
unsigned nr_patches = 100;
GLubyte noiseValue[height][width];

// elemente pentru matricea de vizualizare si matricea de proiectie
float Obsx, Obsy, Obsz;
float Refx = 0.0f, Refy = 0.0f, Refz = 100.0f;
float Vx = 0.0, Vy = 0.0, Vz = 1.0;
float alpha = 0.0f, beta = 0.0f, dist = 1000.0f;
float incr_alpha1 = 0.01f, incr_alpha2 = 0.01f;
float winWidth = 1280, winHeight = 720, znear = 0.1, fov = 45;

// variabile pt desenarea sferei
float const U_MIN = -PI / 2, U_MAX = PI / 2, V_MIN = 0, V_MAX = 2 * PI;
int const NR_PARR = 30, NR_MERID = 30;
float step_u = (U_MAX - U_MIN) / NR_PARR, step_v = (V_MAX - V_MIN) / NR_MERID;
float radius = 50;
int index, index_aux;

// variabile pentru traiectoria sferei
float X0 = 0.0f, Y0 = 600.0f, Z0 = 100.0f; //poz initiala
// viteza = 10 m/s, unghi = 30deg
float vy = - 55 * cos(PI / 7),
	vz = 55 * sin(PI / 7),
	vx = 0;
bool isDark = false;
float currX, currY, currZ; //pozitia curenta
float g = 2.0f; // gravitatie
float t = 0.0f, startTime = 0.0f; // timp
int nrRotations = 0; // numarul de rotatii complete

// variabile pt iluminare
float uTexelSize = 1.0f / width, heightScale = 100.0f;

// matrice
glm::mat4 myMatrix, sphMatrix, view, projection;

FastNoiseLite gen;
double noise(double nx, double ny) {
	// Rescale from -1.0:+1.0 to 0.0:1.0
	return gen.GetNoise(nx, ny) / 2.0 + 0.5;
}

void noiseToHeightMap() {
	glGenTextures(1, &texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, texture);
	//	Desfasurarea imaginii pe orizontala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Modul in care structura de texeli este aplicata pe cea de pixeli;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//gen.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	gen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	gen.SetFrequency(0.000001f);
	gen.SetFractalGain(0.1f);
	gen.SetFractalLacunarity(4.0f);
	gen.SetFractalOctaves(3);
	gen.SetFractalType(FastNoiseLite::FractalType_FBm);
	//gen.SetSeed(42);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float nx = x * width - 0.5;
			float ny = y * height - 0.5;

			double noiseVal = 1.0 * noise(nx, ny)
					 + 0.5 * noise(2 * nx, 2 * ny)
					 + 0.25 * noise(4 * nx, 4 * ny);

			noiseValue[y][x] = noiseVal / 1.75 * 255;
		}
	}

	//int imageWidth = width, imageHeight = height;
	//unsigned char* image = SOIL_load_image("iceland_heightmap.png", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, &noiseValue);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);
}

void processNormalKeys(unsigned char key, int x, int y)
{
	switch (key) {
		// taste zoom
		case '-':
			dist -= 5.0;
			break;
		case '+':
			dist += 5.0;
			break;
		// taste miscare camera
		case 'w':
			Obsx -= 5.0;
			Refx -= 5.0;
			break;
		case 's':
			Obsx += 5.0;
			Refx += 5.0;
			break;
		case 'a':
			Obsy -= 5.0;
			Refy -= 5.0;
			break;
		case 'd':
			Obsy += 5.0;
			Refy += 5.0;
			break;
		case 'q':
			Obsz += 5.0;
			Refz += 5.0;
			break;
		case 'e':
			Obsz -= 5.0;
			Refz -= 5.0;
			break;
	}
	if (key == 27)
		exit(0);
}

void processSpecialKeys(int key, int xx, int yy)
{
	switch (key)
	{
		// taste rotatie camera
		case GLUT_KEY_LEFT:
			beta -= 0.01f;
			break;
		case GLUT_KEY_RIGHT:
			beta += 0.01f;
			break;
		case GLUT_KEY_UP:
			alpha += incr_alpha1;
			if (abs(alpha - PI / 2) < 0.05)
			{
				incr_alpha1 = 0.f;
			}
			else
			{
				incr_alpha1 = 0.01f;
			}
			break;
		case GLUT_KEY_DOWN:
			alpha -= incr_alpha2;
			if (abs(alpha + PI / 2) < 0.05)
			{
				incr_alpha2 = 0.f;
			}
			else
			{
				incr_alpha2 = 0.01f;
			}
			break;
	}
}

void CreateSphereVBO(void)
{
	glm::vec4 Vertices[(NR_PARR + 1) * NR_MERID];
	GLushort Indices[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN + parr * step_u;
			float v = V_MIN + merid * step_v;
			float x_vf = radius * cosf(u) * cosf(v);
			float y_vf = radius * cosf(u) * sinf(v);
			float z_vf = radius * sinf(u);

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Indices[index] = index;
			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices[(NR_PARR + 1) * NR_MERID + index_aux] = index;
			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices[AUX + 4 * index] = index1;
				Indices[AUX + 4 * index + 1] = index2;
				Indices[AUX + 4 * index + 2] = index3;
				Indices[AUX + 4 * index + 3] = index4;
			}
		}
	};

	glGenBuffers(1, &SphereVboId);
	glBindBuffer(GL_ARRAY_BUFFER, SphereVboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &SphereEboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &SphereVaoId);
	glBindVertexArray(SphereVaoId);

	// atributele; 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
}

void CreateTerrainVBO(void)
{
	std::vector<GLfloat> Vertices;

	for (unsigned i = 0; i <= nr_patches - 1; i++)
	{
		for (unsigned j = 0; j <= nr_patches - 1; j++)
		{
			Vertices.push_back(-width / 2.0f + width * i / (float)nr_patches); // v.x
			Vertices.push_back(-height / 2.0f + height * j / (float)nr_patches); // v.z
			Vertices.push_back(0.0f); // v.y
			
			Vertices.push_back(1.0f); // 4th coord
			Vertices.push_back(i / (float)nr_patches); // u
			Vertices.push_back(j / (float)nr_patches); // v

			Vertices.push_back(-width / 2.0f + width * (i + 1) / (float)nr_patches); // v.x
			Vertices.push_back(-height / 2.0f + height * j / (float)nr_patches); // v.z
			Vertices.push_back(0.0f); // v.y
			
			Vertices.push_back(1.0f); // 4th coord
			Vertices.push_back((i + 1) / (float)nr_patches); // u
			Vertices.push_back(j / (float)nr_patches); // v

			Vertices.push_back(-width / 2.0f + width * i / (float)nr_patches); // v.x
			Vertices.push_back(-height / 2.0f + height * (j + 1) / (float)nr_patches); // v.z
			Vertices.push_back(0.0f); // v.y
			
			Vertices.push_back(1.0f); // 4th coord
			Vertices.push_back(i / (float)nr_patches); // u
			Vertices.push_back((j + 1) / (float)nr_patches); // v

			Vertices.push_back(-width / 2.0f + width * (i + 1) / (float)nr_patches); // v.x
			Vertices.push_back(-height / 2.0f + height * (j + 1) / (float)nr_patches); // v.z
			Vertices.push_back(0.0f); // v.y
			
			Vertices.push_back(1.0f); // 4th coord
			Vertices.push_back((i + 1) / (float)nr_patches); // u
			Vertices.push_back((j + 1) / (float)nr_patches); // v
		}
	}

	// se creeaza un buffer nou
	glGenBuffers(1, &TerrainVboId);
	// este setat ca buffer curent
	glBindBuffer(GL_ARRAY_BUFFER, TerrainVboId);
	// varfurile sunt "copiate" in bufferul curent
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(GLfloat), &Vertices[0], GL_STATIC_DRAW);

	// se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
	glGenVertexArrays(1, &TerrainVaoId);
	glBindVertexArray(TerrainVaoId);

	// se activeaza lucrul cu atribute; atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

	// se activeaza lucrul cu atribute; atributul 0 = pozitie
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &TerrainVboId);
	glDeleteBuffers(1, &SphereVboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &TerrainVaoId);
	glDeleteVertexArrays(1, &SphereVaoId);
}

void CreateShaders(void)
{
	// LoadShaders a fost modificat sa incarce si shadere pentru teselare
	ProgramId = LoadShaders("example.vert", "example.frag", "tessellation.tesc", "tessellation.tese");
	glUseProgram(ProgramId);
}

void CreateSphShaders(void)
{
	SphProgramId = LoadShaders("sphere.vert", "sphere.frag");
	glUseProgram(SphProgramId);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
	glDeleteProgram(SphProgramId);
}

void UseTerrainShader(void) {
	glUseProgram(ProgramId);
	glBindVertexArray(TerrainVaoId);
	glBindBuffer(GL_ARRAY_BUFFER, TerrainVboId);

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glUniform3f(viewPosLocation, Obsx, Obsy, Obsz);
	glUniform1i(heightMapLocation, 0);

	glUniform1f(uTexelSizeLocation, uTexelSize);
	glUniform1f(heightScaleLocation, heightScale);
	glUniform3f(lightPosLoc, currX, currY, currZ);
	glUniform1i(isDarkLocation, isDark);
	glutPostRedisplay();
}

void UseSphShader(void) {

	glUseProgram(SphProgramId);
	glBindVertexArray(SphereVaoId);
	glBindBuffer(GL_ARRAY_BUFFER, SphereVboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEboId);

	glUniformMatrix4fv(viewLocation2, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLocation2, 1, GL_FALSE, &projection[0][0]);

	sphMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(currX, currY, currZ));
	glUniformMatrix4fv(myMatrixLocation2, 1, GL_FALSE, &sphMatrix[0][0]);

	glUniform1i(isDarkLocation2, isDark);
	glutPostRedisplay();
}

void SphereMovement(void) {
	if (currY >= -600.f && currY <= -570.f) {
		startTime = t;
		isDark = !isDark;
		nrRotations++;
	}
	t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - startTime * nrRotations;
	currX = X0 + vx * t;
	currY = Y0 + vy * t;
	currZ = Z0 + vz * t - g / 2 * t * t;
}

void UpdateFunc(int val) {

	SphereMovement();

	glutPostRedisplay();

	glutTimerFunc(frameInterval, UpdateFunc, 0);
}

void Initialize(void)
{
	CreateSphereVBO();
	CreateTerrainVBO();
	noiseToHeightMap();
	
	myMatrix = glm::mat4(1.0f);
	CreateShaders();
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	viewPosLocation = glGetUniformLocation(ProgramId, "viewPos");
	heightMapLocation = glGetUniformLocation(ProgramId, "heightMap");
	uTexelSizeLocation = glGetUniformLocation(ProgramId, "uTexelSize");
	heightScaleLocation = glGetUniformLocation(ProgramId, "HEIGHT_SCALE");
	isDarkLocation = glGetUniformLocation(ProgramId, "isDark");
	lightPosLoc = glGetUniformLocation(ProgramId, "inLightPos");
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	
	sphMatrix = glm::mat4(1.0f);
	CreateSphShaders();
	myMatrixLocation2 = glGetUniformLocation(SphProgramId, "myMatrix");
	viewLocation2 = glGetUniformLocation(SphProgramId, "view");
	projLocation2 = glGetUniformLocation(SphProgramId, "projection");
	isDarkLocation2 = glGetUniformLocation(SphProgramId, "isDark");
	glUniformMatrix4fv(myMatrixLocation2, 1, GL_FALSE, &sphMatrix[0][0]);

	glEnable(GL_DEPTH_TEST);
}

void RenderFunction(void)
{
	// culoarea de fond a ecranului
	if (!isDark) { // cand e zi/lumina
		// clamp -570:570 to 0:1
		// /570 -> -1:1
		// +1 -> 0:2
		// /2 -> 0:1

		float clampedCurrY = (currY / 570 + 1) / 2;

		glClearColor(0.39f * clampedCurrY + 0.15f * cos((clampedCurrY + 0.1) * PI) - 0.05f, 0.43f * sin(clampedCurrY * PI) + 0.075f * cos(clampedCurrY * PI) - 0.05f, 0.93f * sin(clampedCurrY * PI), 1.0f); 
	}
	else { // cand e noapte/intuneric
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// reperul de vizualizare + proiectie
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz);
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz);
	view = glm::lookAt(Obs, PctRef, Vert);
	projection = glm::infinitePerspective(fov, GLfloat(winWidth) / GLfloat(winHeight), znear);
	
	// randam terenul cu shaderele corespunzatoare
	UseTerrainShader();
	glDrawArrays(GL_PATCHES, 0, 4 * nr_patches * nr_patches);

	// randam sfera cu shaderele corespunzatoare
	UseSphShader();
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0)
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}

	// verificare erori in timpul randarii
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL Error: " << error << std::endl;
	}

	glutSwapBuffers();
	glFlush();
}

void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100); // pozitia initiala a ferestrei
	glutInitWindowSize(winWidth, winHeight); //dimensiunile ferestrei
	glutCreateWindow("Procedural Terrain Generation"); // titlul ferestrei
	glewInit(); // nu uitati de initializare glew; trebuie initializat inainte de a a initializa desenarea
	Initialize();
	glutIdleFunc(RenderFunction);
	glutDisplayFunc(RenderFunction);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutTimerFunc(frameInterval, UpdateFunc, 0);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

