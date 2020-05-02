/*
Semestre 2020-2
Proyecto final
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <thread>
#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include"Model.h"
#include "Skybox.h"
#include"SpotLight.h"
#include "Sound.h"
#include "Keyframe.h"
const float toRadians = 3.14159265f / 180.0f;
float movCoche;
float movOffset;
bool avanza;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture dadoTexture;
Texture pisoTexture;
Texture Tagave;
//materiales
Material Material_brillante;
Material Material_opaco;
//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
Sound *music = new Sound(0.0f, 0.0f, 0.0f);

Skybox skybox;
Skybox skybox_noche;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLint dia_flag = 0;
// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
//c�lculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = unsigned int(i) * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.48f,	0.64f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.27f,	0.64f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}

void CrearCupula() {
	
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void loadModelArray(Model objeto, GLfloat* posiciones, glm::mat4 model, GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess, int num_posiciones) {
	glm::vec3 posicion;
	int i_aux = 0;
	posicion = glm::vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < num_posiciones; i++) {
		if (i_aux > 2)
			i_aux = 0;
		if (i_aux == 0) {
			posicion.x = posiciones[i];
		}
		if (i_aux == 1) {
			posicion.y = posiciones[i];
		}
		if (i_aux == 2) {
			posicion.z = posiciones[i];
			model = glm::mat4(1.0);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, posicion);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			objeto.RenderModel();
			posicion = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		i_aux++;

	}
}

void loadModelArrayFaro(Model objeto, GLfloat* posiciones, glm::mat4 model, GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess, GLint* ind_point_lights, int num_posiciones) {
	glm::vec3 posicion;
	int i_aux = 0;
	int iL = 0;
	int iL2 = 0;
	posicion = glm::vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < num_posiciones; i++) {
		if (i_aux > 2)
			i_aux = 0;
		if (i_aux == 0) {
			posicion.x = posiciones[i];
		}
		if (i_aux == 1) {
			posicion.y = posiciones[i];
		}
		if (i_aux == 2) {
			iL = ind_point_lights[iL2];
			posicion.z = posiciones[i];
			model = glm::mat4(1.0);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, posicion);
			pointLights[iL].SetPosition(posicion.x, 3.4f, posicion.z);
			if (dia_flag == 0)
				pointLights[iL].SetColor(1.0f, 1.0f, 1.0f);
			else
				pointLights[iL].SetColor(0.0f, 0.0f, 0.0f);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			objeto.RenderModel();
			posicion = glm::vec3(0.0f, 0.0f, 0.0f);
			iL2++;
		}
		i_aux++;
	}
}

void loadModelArbustoArray(std::vector<Mesh*> meslist, GLfloat* posiciones, glm::mat4 model, GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess, int num_posiciones) {
	glm::vec3 posicion;
	int i_aux = 0;
	posicion = glm::vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < num_posiciones; i++) {
		if (i_aux > 2)
			i_aux = 0;
		if (i_aux == 0) {
			posicion.x = posiciones[i];
		}
		if (i_aux == 1) {
			posicion.y = posiciones[i];
		}
		if (i_aux == 2) {
			posicion.z = posiciones[i];
			model = glm::mat4(1.0);
			model = glm::translate(model, posicion);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			//blending: transparencia o traslucidez
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meslist[3]->RenderMesh();
			glDisable(GL_BLEND);
			posicion = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		i_aux++;
	}
}

void rotacion_compleja_anim(GLfloat *anim_rot, bool *sentido, GLfloat min_rot, GLfloat max_rot, GLfloat offset) {
	if (sentido != NULL) {
		if (*anim_rot <= max_rot && *sentido) {
			*anim_rot += offset * deltaTime;
		}
		else if (*anim_rot >= min_rot && !*sentido) {
			*anim_rot -= offset * deltaTime;
		}
		else {
			if (*sentido) {
				*sentido = false;
				*anim_rot = max_rot;
			}
			else {
				*sentido = true;
				*anim_rot = min_rot;
			}
		}
	}
	else {
		if (max_rot > min_rot) {
			if (*anim_rot <= max_rot) {
				*anim_rot += offset * deltaTime;
			}
			else {
				*anim_rot = min_rot;
			}
		}
		else {
			if (*anim_rot <= max_rot) {
				*anim_rot -= offset * deltaTime;
			}
			else {
				*anim_rot = max_rot;
			}
		}

	}
}

void animacion_simple(GLfloat *pos, GLfloat pos_final, GLfloat offset, const char* FX, bool *status_FX) {
	if (!*status_FX) {
		music->playFX(FX);
		*status_FX = true;
	}
	if (pos_final < *pos) {
		if (*pos > pos_final)
			*pos -= offset * deltaTime;
		else
			*pos = pos_final;
	}
	else {
		if (*pos < pos_final)
			*pos += offset * deltaTime;
		else
			*pos = pos_final;
	}
}

int main()
{
	mainWindow = Window(1920, 1080); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearCubo();
	CreateShaders();
	camera = Camera(glm::vec3(26.0f, 3.0f, 1.2f), glm::vec3(0.0f, 1.0f, 0.0f), 180.0f, 0.0f, 5.0f, 0.5f);
	
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	Tagave = Texture("Textures/arbusto.png");
	Tagave.LoadTextureA();
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);
	std::vector<std::thread> threads;
	Model *Alameda = new Model(); 
	Alameda->LoadModel("Models/alameda.assbin");
	Model *Monumento = new Model();
	Monumento->LoadModel("Models/monumento.assbin");
	Model *Kiosko = new Model();
	Kiosko->LoadModel("Models/quiosco.assbin");
	Model *CuerpoPajaro = new Model();
	CuerpoPajaro->LoadModel("Models/pajaro_cuerpo.obj");
	Model *AlaDer = new Model();
	AlaDer->LoadModel("Models/ala_der.obj");
	Model *AlaIzq = new Model();
	AlaIzq->LoadModel("Models/ala_izq.obj");
	Model *Arbol = new Model();
	Arbol->LoadModel("Models/arbol.assbin");
	Model *Cabeza_avatar = new Model();
	Cabeza_avatar->LoadModel("Models/avatar_cabeza.obj");
	Model *Cuerpo_avatar = new Model();
	Cuerpo_avatar->LoadModel("Models/avatar_cuerpo.obj");
	Model *PiernaD_avatar = new Model();
	PiernaD_avatar->LoadModel("Models/avatar_pierna_d.obj");
	Model *PiernaI_avatar = new Model();
	PiernaI_avatar->LoadModel("Models/avatar_pierna_i.obj");
	Model *BrazoI_avatar = new Model();
	BrazoI_avatar->LoadModel("Models/avatar_brazo_i.obj");
	Model *BrazoD_avatar = new Model();
	BrazoD_avatar->LoadModel("Models/avatar_brazo_d.obj");
	Model *Columna = new Model();
	Columna->LoadModel("Models/columna.obj");
	Model Bote_basura = Model();
	Bote_basura.LoadModel("Models/bote_basura.assbin");
	Model Faro = Model();
	Faro.LoadModel("Models/faro.assbin");
	Model Banca = Model();
	Banca.LoadModel("Models/banca.assbin");
	Model Fuente = Model();
	Fuente.LoadModel("Models/fuente.assbin");
	Model Bano = Model();
	Bano.LoadModel("Models/baño.assbin");
	Model Puerta = Model();
	Puerta.LoadModel("Models/puerta.assbin");
	Model Helipuerto = Model();
	Helipuerto.LoadModel("Models/helipuerto.assbin");
	Model Helicoptero = Model();
	Helicoptero.LoadModel("Models/helicopter.assbin");
	Model Helice_lateral = Model();
	Helice_lateral.LoadModel("Models/helice_lateral.assbin");
	Model Helice = Model();
	Helice.LoadModel("Models/helice.assbin");
	
	GLfloat posiciones_arboles[] = {
		4.5f, 0.8f, 6.3f,
		8.5f, 0.8f,6.3f,
		14.5f, 0.8f,6.3f,

		4.5f, 0.8f, 11.0f,
		8.5f, 0.8f,11.0f,
		14.5f, 0.8f,11.0f,

		4.5f, 0.8f, 18.3f,
		8.5f, 0.8f,18.3f,
		14.5f, 0.8f,18.3f,

		4.5f, 0.8f, -4.1f,
		8.5f, 0.8f,-4.1f,
		14.5f, 0.8f,-4.1f,

		4.5f, 0.8f, -10.3f,
		8.5f, 0.8f,-10.3f,
		14.5f, 0.8f,-10.3f,

		4.5f, 0.8f, -14.3f,
		8.5f, 0.8f,-14.3f,
		14.5f, 0.8f,-14.3f,

		-7.1f, 0.8f, -14.3f,
		-12.5f, 0.8f,-14.3f,
		-18.5f, 0.8f,-14.3f,

		-7.1f, 0.8f, -10.3f,
		-12.5f, 0.8f,-10.3f,
		-18.5f, 0.8f,-10.3f,

		-7.1f, 0.8f, 6.3f,
		-12.5f, 0.8f,6.3f,
		-18.5f, 0.8f,6.3f,

		-7.1f, 0.8f, 11.0f,
		-12.5f, 0.8f,11.0f,
		-18.5f, 0.8f,11.0f,

		-7.1f, 0.8f, 18.3f,
		-12.5f, 0.8f,18.3f,
		-18.5f, 0.8f,18.3f,

		-7.1f, 0.8f, -4.1f,
		-12.5f, 0.8f,-4.1f,
		-18.5f, 0.8f,-4.1f,

	};
	int num_posiciones_arboles = sizeof(posiciones_arboles) / sizeof(posiciones_arboles[0]);
	GLfloat posiciones_arbustos[] = {
		-25.9f, 1.2f, 21.03f,
		-25.9f, 1.2f, 28.03f,
		-25.9f, 1.2f, 33.03f,

		-22.9f, 1.2f, 21.03f,
		-22.9f, 1.2f, 28.03f,
		-22.9f, 1.2f, 33.03f,

		-17.9f, 1.2f, 21.03f,
		-17.9f, 1.2f, 28.03f,
		-17.9f, 1.2f, 33.03f,

		-25.9f, 1.2f, 21.03f,
		-25.9f, 1.2f, 28.03f,
		-25.9f, 1.2f, 33.03f,

		-11.3f, 1.2f, 35.03f,
		-10.9f, 1.2f, 35.03f,
		-9.9f, 1.2f, 35.03f,

		7.8f, 1.2f, 30.03f,
		7.8f, 1.2f, 32.03f,
		7.8f, 1.2f, 34.03f,

		18.3f, 1.2f, 5.5f,
		19.3f, 1.2f, 5.5f,
		20.5f, 1.2f, 5.5f,
		22.5f, 1.2f, 5.5f,
		23.5f, 1.2f, 5.5f,
		24.5f, 1.2f, 5.5f,

		18.3f, 1.2f, -3.7f,
		19.3f, 1.2f, -3.7f,
		20.5f, 1.2f, -3.7f,
		22.5f, 1.2f, -3.7f,
		23.5f, 1.2f, -3.7f,
		24.5f, 1.2f, -3.7f,
	};
	int num_posiciones_arbustos = sizeof(posiciones_arbustos) / sizeof(posiciones_arbustos[0]);
	GLfloat posiciones_botes[] = {
		0.839863f, 0.0f, 21.764261f,
		-22.199312f, 0.0f, -0.895342f,
		1.636256f, 0.0f, 4.358578f,
		19.175541f, 0.0f, -1.209046f,

	};
	int num_posiciones_botes = sizeof(posiciones_botes) / sizeof(posiciones_botes[0]);
	GLfloat posiciones_faros[] = {
		-5.0f, 0.0f, -2.0f,
		-5.0f, 0.0f, 4.0f,
		1.0f, 0.0f, -2.0f,
		1.0f, 0.0f, 4.0f,
		19.0f, 0.0f, 4.5f,
		19.0f, 0.0f, -2.5f,
		-22.0f, 0.0f, 4.5f,
		-22.0f, 0.0f, -2.5f,
	};
	int num_posiciones_faros = sizeof(posiciones_faros) / sizeof(posiciones_faros[0]);
	GLint inds_luz_faro[] = {
		1,2,3,4,5,6,7,8
	};
	GLfloat posiciones_bancas[] = {
		3.5f,0.0f,2.5f,
		14.5f,0.0f,2.5f,
		-7.0f,0.0f,2.5f,
		-18.0f,0.0f,2.5f,
	};
	int num_posiciones_bancas = sizeof(posiciones_bancas) / sizeof(posiciones_bancas[0]);
	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
		0.0f, 1.0f,
		-0.1f, 3.0f, 28.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	pointLights[1] = PointLight(0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.03f, 0.3f, 0.01f);
	pointLightCount++;
	pointLights[2] = PointLight(0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.03f, 0.3f, 0.01f);
	pointLightCount++;
	pointLights[3] = PointLight(0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		2.0f, 1.5f, 1.5f,
		0.03f, 0.3f, 0.01f);
	pointLightCount++;
	pointLights[4] = PointLight(0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.03f, 0.3f, 0.01f);
	pointLightCount++;
	pointLights[5] = PointLight(0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.03f, 0.3f, 0.01f);
	pointLightCount++;
	pointLights[6] = PointLight(0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.03f, 0.3f, 0.01f);
	pointLightCount++;
	pointLights[7] = PointLight(0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.03f, 0.3f, 0.01f);
	pointLightCount++;
	pointLights[8] = PointLight(0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.03f, 0.3f, 0.01f);
	pointLightCount++;
	pointLights[9] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-2.3f, 3.0f, 28.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	// Luz R
	spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f,
		0.0f, 2.0f,
		-1.0f, 5.0f, -24.0f,
		0.00f, -1.0f, -0.8f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	// Luz G
	spotLights[2] = SpotLight(0.0f, 1.0f, 0.0f,
		0.0f, 2.0f,
		1.0f, 4.3f, -29.5f,
		0.0f, -0.450f, 0.5f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	// Luz B
	spotLights[3] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 2.0f,
		-4.0f, 4.3f, -29.5f,
		0.63f, -0.45f, 0.52f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;


	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/hill_lf.png");
	skyboxFaces.push_back("Textures/Skybox/hill_rt.png");
	skyboxFaces.push_back("Textures/Skybox/hill_dn.png");
	skyboxFaces.push_back("Textures/Skybox/hill_up.png");
	skyboxFaces.push_back("Textures/Skybox/hill_bk.png");
	skyboxFaces.push_back("Textures/Skybox/hill_ft.png");

	std::vector<std::string> skyboxFaces_noche;
	skyboxFaces_noche.push_back("Textures/Skybox/hill_noche_lf.png");
	skyboxFaces_noche.push_back("Textures/Skybox/hill_noche_rt.png");
	skyboxFaces_noche.push_back("Textures/Skybox/hill_noche_dn.png");
	skyboxFaces_noche.push_back("Textures/Skybox/hill_noche_up.png");
	skyboxFaces_noche.push_back("Textures/Skybox/hill_noche_bk.png");
	skyboxFaces_noche.push_back("Textures/Skybox/hill_noche_ft.png");

	skybox = Skybox(skyboxFaces);
	skybox_noche = Skybox(skyboxFaces_noche);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);

	float giro_offset = 45.0f;
	GLfloat rot_helice = 0.0f;
	GLfloat mov_alas = 0.0f;
	movOffset = 1.0f;
	int sentido_carro = 1;
	GLint ciclos = 0;
	GLfloat pos_x_helicopter = 0.0f;
	GLfloat pos_y_helicopter = 0.0f;
	GLfloat pos_z_helicopter = 0.0f;
	GLfloat func_helicoptero = 0.0f;
	GLfloat puerta_offset = 45.0f;
	GLfloat func_sin = 0.0f;
	GLfloat rot_y_helicoptero = 0.0f;
	GLfloat rotPuerta1 = 0.0f;
	GLfloat rotPuerta2 = 0.0f;
	GLfloat avatar_rot = 0.0f;
	GLfloat rot_brazo = 0.0f;
	GLfloat rot_pierna = 0.0f;
	GLint flag_helicoptero = 1;
	Keyframe *keyframes_pajaro = new Keyframe("Keyframes/keyFramesPajaro.txt", 60, "Pajaro");
	Keyframe *keyframes_helicoptero = new Keyframe("Keyframes/keyFramesHelicoptero.txt", 60, "Helicoptero");
	music->playMusic("sound/gorillaz.mp3");
	bool isSoundHelicopterPlay = false;
	bool isSoundPuertaPlay = true;
	bool puerta1_anim_ant = false;
	bool puerta2_anim_ant = false;
	bool *keys_avatar;
	bool alas_sentido = false;
	bool brazo_sentido = false;
	bool pierna_sentido = false;
	glm::vec3 posAvatar(24.0f, 1.88f, 1.0f);
	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose()){
		GLfloat now = float(glfwGetTime());
		deltaTime = 1.0f * (now - lastTime);
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		if (mainWindow.getCamara() == 0) {
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		if (mainWindow.getCamara() == 1) {
			camera.keyControlAerea(mainWindow.getsKeys(), deltaTime);
			camera.mouseControlAerea();
		}
		if (mainWindow.getCamara() == 2) {
			camera.setCameraPosition(glm::vec3(posAvatar.x, 15.0f, posAvatar.z));
			camera.keyControlAvatar(mainWindow.getsKeys(), deltaTime);
			camera.mouseControlAerea();
			posAvatar.x = camera.getCameraPosition().x;
			posAvatar.z = camera.getCameraPosition().z;
			keys_avatar = mainWindow.getsKeys();
			if (keys_avatar[GLFW_KEY_A] || keys_avatar[GLFW_KEY_W] || keys_avatar[GLFW_KEY_D] || keys_avatar[GLFW_KEY_S]) {
				rotacion_compleja_anim(&rot_brazo, &brazo_sentido, 0.0f, 45.0f, 50.0f);
				rotacion_compleja_anim(&rot_pierna, &pierna_sentido, -45.0f, 45.0f, 90.0f);
				if (keys_avatar[GLFW_KEY_D] && keys_avatar[GLFW_KEY_W])
					avatar_rot = 45.0f;
				else
					if (keys_avatar[GLFW_KEY_D] && keys_avatar[GLFW_KEY_S])
						avatar_rot = -45.0f;
					else
						if (keys_avatar[GLFW_KEY_S] && keys_avatar[GLFW_KEY_A])
							avatar_rot = -135.0f;
						else
							if (keys_avatar[GLFW_KEY_A] && keys_avatar[GLFW_KEY_W])
								avatar_rot = 135.0f;
							else
								if (keys_avatar[GLFW_KEY_W])
									avatar_rot = 90.0f;
								else
									if (keys_avatar[GLFW_KEY_S])
										avatar_rot = -90.0f;
									else
										if (keys_avatar[GLFW_KEY_D])
											avatar_rot = 0.0f;
										else
											if (keys_avatar[GLFW_KEY_A])
												avatar_rot = 180.0f;
			}
		}
		
		if (mainWindow.getCamara() == 3) {
			camera.keyControlQuiosco(mainWindow.getsKeys(), deltaTime);
			camera.mouseControlQuiosco();

		}
		// Camara en pausa
		if (mainWindow.getCamara() == 4) {
			
		}
		// Animacion de keyframes
		keyframes_pajaro->inputKeyframes(mainWindow.getAnimKeyPajaro());
		keyframes_pajaro->animate();
		keyframes_helicoptero->inputKeyframes(mainWindow.getAnimKeyHelicoptero());
		keyframes_helicoptero->animate();
		mainWindow.setAnimKeyHelicoptero(false);
		mainWindow.setAnimKeyPajaro(false);
		// Animacion simple puertas
		if(mainWindow.getAnimPuerta1() != puerta1_anim_ant)
			isSoundPuertaPlay = false;
		puerta1_anim_ant = mainWindow.getAnimPuerta1();
		if(mainWindow.getAnimPuerta1()) {
			animacion_simple(&rotPuerta1, 90.0f, puerta_offset / 4, "sound/puerta_abre.wav", &isSoundPuertaPlay);
		}
		else {
			animacion_simple(&rotPuerta1, 0.0f, puerta_offset, "sound/puerta_cierra.wav", &isSoundPuertaPlay);
		}
		if (mainWindow.getAnimPuerta2() != puerta2_anim_ant)
			isSoundPuertaPlay = false;
		puerta2_anim_ant = mainWindow.getAnimPuerta2();
		if(mainWindow.getAnimPuerta2()) {
			animacion_simple(&rotPuerta2, 90.0f, puerta_offset / 4, "sound/puerta_abre.wav", &isSoundPuertaPlay);
		}
		else {
			animacion_simple(&rotPuerta2, 0.0f, puerta_offset, "sound/puerta_cierra.wav", &isSoundPuertaPlay);
		}
		rotacion_compleja_anim(&rot_helice, NULL, 0, 360, 180.0f);

		// Animacion alas de pajaro
		rotacion_compleja_anim(&mov_alas, &alas_sentido, 0.0f, 45.0f, 30.0f);

		if (mainWindow.getAnimHelicoptero()) {
			// Efecto de sonido helicoptero
			if (!isSoundHelicopterPlay) {
				music->playFX("sound/helicoptero.wav");
				isSoundHelicopterPlay = true;
			}
			// Altura
			if (pos_y_helicopter <= 15.0f) {
				
				pos_y_helicopter += movOffset * deltaTime;
			}
			else {
				if (func_sin <= 360.0f) {
					func_sin += 27.5f * deltaTime;
				}
				else 
					func_sin = 0.0f;	
			}
			// Recorrido helicoptero
			if (flag_helicoptero == 1) {
				if (pos_x_helicopter <= 30.0f) {
					pos_x_helicopter += movOffset * deltaTime;
					if(rot_y_helicoptero <= 90.0f && rot_y_helicoptero >= 0.0f)
						rot_y_helicoptero -= giro_offset * deltaTime;
					else 
						rot_y_helicoptero = 360.0f;
				}
				else
					flag_helicoptero++;
			}
			if (flag_helicoptero == 2) {
				if (pos_z_helicopter <= 50.0f) {
					pos_z_helicopter += movOffset * deltaTime;
					if (rot_y_helicoptero >= 270.0f)
						rot_y_helicoptero -= giro_offset * deltaTime;
				}
				else
					flag_helicoptero++;
			}
			if (flag_helicoptero == 3) {
				if (pos_x_helicopter >= 0.0f) {
					pos_x_helicopter -= movOffset * deltaTime;
					if (rot_y_helicoptero >= 180.0f)
						rot_y_helicoptero -= giro_offset * deltaTime;
				}else
					flag_helicoptero++;
			}
			if (flag_helicoptero == 4) {
				if (pos_z_helicopter >= 0.0f) {
					pos_z_helicopter -= movOffset * deltaTime;
					if (rot_y_helicoptero >= 90.0f)
						rot_y_helicoptero -= giro_offset * deltaTime;
				}else
					flag_helicoptero = 1;
			}
			// Animacion senoidal helicoptero
			func_helicoptero = 1.0f *(glm::sin(glm::radians(func_sin)));
		}
		else {
			isSoundHelicopterPlay = false;
			rot_y_helicoptero = 0.0f;
			func_helicoptero = 0.0f;
			flag_helicoptero = 1;
			pos_x_helicopter = 0.0f;
			pos_y_helicopter = 0.0f;
			pos_z_helicopter = 0.0f;
		}
		//printf("posx:%f posy:%f posz:%f\n",pos_x_helicopter, pos_y_helicopter, pos_z_helicopter);
		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (int(now) % 10 == 0) {
			if (int(now) != ciclos) {
				dia_flag = !dia_flag;
				ciclos = int(now);
			}
		}
		if (dia_flag == 1) {
			mainLight.setIntensity(0.3f, 0.3f);
			skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		else {
			mainLight.setIntensity(0.0, 0.0);
			skybox_noche.DrawSkybox(camera.calculateViewMatrix(), projection);
		}

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		if(mainWindow.OnOffLight())
			spotLights[0].SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
		else
			spotLights[0].SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
		if (mainWindow.statusluzR())
			spotLights[1].SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
		else
			spotLights[1].SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
		if (mainWindow.statusluzG())
			spotLights[2].SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
		else
			spotLights[2].SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
		if (mainWindow.statusluzB())
			spotLights[3].SetColor(glm::vec3(0.0f, 0.0f, 1.0f));
		else
			spotLights[3].SetColor(glm::vec3(0.0f, 0.0f, 0.0f));

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		printf("%f %f %f\n", camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Alameda->RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Monumento->RenderModel();

		// Pajaro Compleja y keyframes
		modelaux = glm::mat4(1.0);
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-1.7f, 5.5f, 31.0f));
		model = glm::translate(model, glm::vec3(keyframes_pajaro->getVal("movX"), keyframes_pajaro->getVal("movY"), keyframes_pajaro->getVal("movZ")));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(keyframes_pajaro->getVal("giroX")), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(keyframes_pajaro->getVal("giroY")), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(keyframes_pajaro->getVal("giroZ")), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		CuerpoPajaro->RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.40f, 0.0f));
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.230f));
		model = glm::rotate(model, glm::radians(mov_alas), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		AlaDer->RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.230f));
		model = glm::rotate(model, glm::radians(mov_alas*(-1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		AlaIzq->RenderModel();


		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Fuente.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Bano.RenderModel();

		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.90f, 0.700f, 30.50f));
		model = glm::rotate(model, glm::radians(rotPuerta1), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = modelaux;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.50f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Puerta.RenderModel();


		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(17.9f, 0.700f, 30.50f));
		model = glm::rotate(model, glm::radians(rotPuerta2), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = modelaux;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.50f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Puerta.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Helipuerto.RenderModel();

		// Helicoptero Animacion compleja
		modelaux = glm::mat4(1.0);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-21.5f + pos_x_helicopter,0.0f + pos_y_helicopter + func_helicoptero, -24.5f + pos_z_helicopter));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(rot_y_helicoptero), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Helicoptero.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.4f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		if (pos_x_helicopter == 0.0f && pos_y_helicopter == 0.0f && pos_z_helicopter == 0.0f)
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		else
			model = glm::rotate(model, glm::radians(rot_helice), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Helice.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.05f,4.55f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		if (pos_x_helicopter == 0.0f && pos_y_helicopter == 0.0f && pos_z_helicopter == 0.0f)
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		else
			model = glm::rotate(model, glm::radians(rot_helice), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Helice_lateral.RenderModel();

		// Helicoptero Animacion Keyframes
		modelaux = glm::mat4(1.0);
		model = glm::mat4(1.0);
		// printf("\n%f %f %f\n", keyframes_helicoptero.getVal("movX"), keyframes_helicoptero.getVal("movY"), keyframes_helicoptero.getVal("movZ"));
		model = glm::translate(model, glm::vec3(-21.5f, 0.0f, -20.5f));
		model = glm::translate(model, glm::vec3(keyframes_helicoptero->getVal("movX"), keyframes_helicoptero->getVal("movY"), keyframes_helicoptero->getVal("movZ")));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(keyframes_helicoptero->getVal("giroX")), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(keyframes_helicoptero->getVal("giroY")), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(keyframes_helicoptero->getVal("giroZ")), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Helicoptero.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.4f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		if (!keyframes_helicoptero->getPlay())
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		else
			model = glm::rotate(model, glm::radians(rot_helice), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Helice.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.05f, 4.55f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		if(!keyframes_helicoptero->getPlay())
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		else
			model = glm::rotate(model, glm::radians(rot_helice), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Helice_lateral.RenderModel();

		//Kiosko 
		modelaux = glm::mat4(1.0);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosko->RenderModel();

		// Avatar
		//Cabeza
		modelaux = glm::mat4(1.0);
		model = glm::mat4(1.0);
		model = glm::translate(model, posAvatar);
		model = glm::rotate(model, glm::radians(-90+avatar_rot), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cabeza_avatar->RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.62f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cuerpo_avatar->RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.3f, 0.35f, 0.0f));
		model = glm::rotate(model, glm::radians(rot_brazo - 45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		BrazoI_avatar->RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.3f, 0.35f, 0.0f));
		model = glm::rotate(model, glm::radians(-rot_brazo), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		BrazoD_avatar->RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.13f, -0.16f, 0.0f));
		model = glm::rotate(model, glm::radians(-rot_pierna), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		PiernaI_avatar->RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.13f, -0.16f, 0.0f));
		model = glm::rotate(model, glm::radians(rot_pierna), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		PiernaD_avatar->RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, -2.0f, 32.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Columna->RenderModel();
		loadModelArray(Bote_basura, posiciones_botes, model, uniformModel, uniformSpecularIntensity, uniformShininess, num_posiciones_botes);
		loadModelArray(*Arbol, posiciones_arboles, model, uniformModel, uniformSpecularIntensity, uniformShininess, num_posiciones_arboles);
		loadModelArray(Banca, posiciones_bancas, model, uniformModel, uniformSpecularIntensity, uniformShininess, num_posiciones_bancas);
		loadModelArrayFaro(Faro, posiciones_faros, model, uniformModel, uniformSpecularIntensity, uniformShininess, inds_luz_faro, num_posiciones_faros);

		Tagave.UseTexture();
		loadModelArbustoArray(meshList, posiciones_arbustos, model, uniformModel, uniformSpecularIntensity, uniformShininess, num_posiciones_arbustos);
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}