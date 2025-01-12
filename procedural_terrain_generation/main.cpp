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
matrUmbraLocation,
viewLocation,
projLocation, 
viewPosLocation,
heightMapLocation,
texture;

float PI = 3.141592;
const int height = 1024, width = 1024;
unsigned nr_patches = 100;
double noiseValue[height][width];

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

// matrice
glm::mat4 myMatrix, view, projection, matrUmbra;

FastNoiseLite gen;
double noise(double nx, double ny) { // if using fastnoiselite
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
	gen.SetFrequency(0.01f);
	gen.SetFractalGain(0.1f);
	gen.SetFractalLacunarity(3.0f);
	gen.SetFractalOctaves(3);

	//for (int x = 0; x < 1000; x++)
	//	for (int y = 0; y < 1000; y++)
	//		std::cout << noise(x, y);
		

	//gen.SetSeed(42);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float nx = x * width - 0.5;
			float ny = y * height - 0.5;

			double noiseVal = 1.0 * noise(nx, ny)//;
					 + 0.5 * noise(2 * nx, 2 * ny)//;
					 + 0.25 * noise(4 * nx, 4 * ny);

			noiseValue[y][x] = noiseVal / 1.75 * 255;
			
			//std::cout << noiseValue[y][x] << " ";
		}
	}

	//int imageWidth = width, imageHeight = height;
	//unsigned char* image = SOIL_load_image("iceland_heightmap.png", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, &noiseValue);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	//std::cout << "width" << width << " height" << height << "\n";
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
	//glm::vec3 Colors[(NR_PARR + 1) * NR_MERID];
	//glm::vec3 Normals[(NR_PARR + 1) * NR_MERID];
	GLushort Indices[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN + parr * step_u; // valori pentru u si v
			float v = V_MIN + merid * step_v;
			float x_vf = radius * cosf(u) * cosf(v); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius * cosf(u) * sinf(v);
			float z_vf = radius * sinf(u);

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			//Colors[index] = glm::vec3(0.96 + 0.1f * sinf(u) * cosf(v), 0.71 + 0.1f * cosf(u) * sinf(v), 0.1);
			//Normals[index] = glm::vec3(x_vf, y_vf, z_vf);
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
				Indices[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices[AUX + 4 * index + 1] = index2;
				Indices[AUX + 4 * index + 2] = index3;
				Indices[AUX + 4 * index + 3] = index4;
			}
		}
	};

	glGenBuffers(1, &SphereVboId);
	glBindBuffer(GL_ARRAY_BUFFER, SphereVboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), NULL, GL_STATIC_DRAW);

	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors), sizeof(Normals), Normals);

	glGenBuffers(1, &SphereEboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &SphereVaoId);
	glBindVertexArray(SphereVaoId);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	//glEnableVertexAttribArray(1); // atributul 1 = culoare
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices));
	//glEnableVertexAttribArray(2); // atributul 2 = normala
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices) + sizeof(Colors)));
}

void CreateTerrainVBO(void)
{
	std::vector<GLfloat> Vertices;

	for (unsigned i = 0; i <= nr_patches - 1; i++)
	{
		for (unsigned j = 0; j <= nr_patches - 1; j++)
		{
			//std::cout << -width / 2.0f + width * i / (float)nr_patches << " " << -height / 2.0f + height * j / (float)nr_patches << "\n";
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
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);//(4 * sizeof(GLfloat)));

	// se activeaza lucrul cu atribute; atributul 0 = pozitie
	glEnableVertexAttribArray(1);
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
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
	ProgramId = LoadShaders("sphere.vert", "sphere.frag");
	glUseProgram(SphProgramId);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
	glDeleteProgram(SphProgramId);
}

void Initialize(void)
{
	glClearColor(0.5f, 0.3f, 0.9f, 1.0f); // culoarea de fond a ecranului
	CreateSphereVBO();
	//CreateTerrainVBO();

	noiseToHeightMap();

	/*
	CreateShaders();
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	//matrUmbraLocation = glGetUniformLocation(ProgramId, "matrUmbra");
	viewPosLocation = glGetUniformLocation(ProgramId, "viewPos");
	heightMapLocation = glGetUniformLocation(ProgramId, "heightMap");
	*/
	
	CreateSphShaders();
	myMatrixLocation = glGetUniformLocation(SphProgramId, "myMatrix");
	viewLocation = glGetUniformLocation(SphProgramId, "view");
	projLocation = glGetUniformLocation(SphProgramId, "projection");
	//matrUmbraLocation = glGetUniformLocation(ProgramId, "matrUmbra");
	viewPosLocation = glGetUniformLocation(SphProgramId, "viewPos");
	
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glEnable(GL_DEPTH_TEST);
}
void RenderFunction(void)
{
	//glUseProgram(ProgramId);

	glUseProgram(SphProgramId);
	glBindVertexArray(SphereVaoId);
	glBindBuffer(GL_ARRAY_BUFFER, SphereVboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEboId);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//glBindVertexArray(TerrainVaoId);
	//glBindBuffer(GL_ARRAY_BUFFER, TerrainVboId);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT_AND_BACK);

	// reperul de vizualizare + proiectie
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	//std::cout << "Observator: " << Obsx << " " << Obsy << " " << Obsz << "\n";
	//std::cout << "Ref: " << Refx << " " << Refy << " " << Refz << "\n";

	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz);
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz);
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	projection = glm::infinitePerspective(fov, GLfloat(winWidth) / GLfloat(winHeight), znear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);
	glUniform3f(viewPosLocation, Obsx, Obsy, Obsz);

	//glUniform1i(heightMapLocation, 0);

	// Functiile de desenare
	//glDrawArrays(GL_PATCHES, 0, 4 * nr_patches * nr_patches);
	// 
	//glDrawArrays(GL_TRIANGLE_FAN, 4 * nr_patches * nr_patches, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	
	//glUseProgram(SphProgramId);
	//glBindVertexArray(SphereVaoId);
	//glBindBuffer(GL_ARRAY_BUFFER, SphereVboId);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEboId);

	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0)
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}
	
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
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

