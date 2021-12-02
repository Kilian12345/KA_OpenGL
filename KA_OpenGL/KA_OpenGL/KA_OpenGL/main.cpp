#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"
#include "SkyBox.h"

#include <assimp\Importer.hpp>

using namespace std;

const float toRadians = 3.14159265f / 180.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0,
uniformDirectionalLightTransform = 0, uniformOmniLightPos = 0, uniformFarPlane = 0,
uniformBaseColor = 0;

Window mainWindow;
vector<Mesh*> meshList;

vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;
Shader unlitShader;

Camera camera;

Texture brickTex;
Texture dirtTex;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

unsigned int pointLightsCount = 0;
unsigned int spotLightsCount = 0;

Material shineMaterial;
Material dullMaterial;

Model Shape1;

SkyBox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";
static const char* unlitVShader = "Shaders/unlit_light.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";		
static const char* unlitFShader = "Shaders/unlit_light.frag";		

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices,
						unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i+=3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i+1] * vLength;
		unsigned int in2 = indices[i+2] * vLength;

		// Setup Cross Product
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0+1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);

		// Cross product
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0+1] += normal.y; vertices[in0+2] += normal.z;
		vertices[in1] += normal.x; vertices[in1+1] += normal.y; vertices[in1+2] += normal.z;
		vertices[in2] += normal.x; vertices[in2+1] += normal.y; vertices[in2+2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount/vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	///// Triangles
	unsigned int indices[] =
	{
		0,3,1,
		1,3,2,
		2,3,0,
		0,1,2
	};

	GLfloat vertices[] =
	{
		// x	y	 z		| u	  v			| normal x,y,z
		-1.0f,-1.0f,-0.6f,	0.0f,0.0f,		0.0f, 0.0f, 0.0f,
		0.0f,-1.0f,1.0f,	0.5f,0.0f,		0.0f, 0.0f, 0.0f,
		1.0f,-1.0f,-0.6f,	1.0f,0.0f,		0.0f, 0.0f, 0.0f,
		0.0f,1.0f,0.0f,		0.5f,1.0f,		0.0f, 0.0f, 0.0f
	};

	///// Plane
	unsigned int floorIndices[] =
	{
		0,2,1,
		1,2,3
	};

	GLfloat floorVertices[] =
	{
		// x	y	 z				| u	   v			| normal x,y,z
		-10.0f, 0.0f, -10.0f,		0.0f, 0.0f,	 		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,		10.0f, 0.0f, 		0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,		0.0f, 10.0f, 		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,			10.0f, 10.0f,		0.0f, -1.0f, 0.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	// Create Mesh and setup
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	unlitShader.CreateFromFiles(unlitVShader, unlitFShader);

	//directionalShadowShader = Shader();
	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
	omniShadowShader.CreateFromFiles("Shaders/omni_shadow_map.vert","Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag");
}

void RenderScene()
{
	
	// Create Model Matrix
	glm::mat4 model(1.0f); // identity matrix

	/// FOR PLANE
	model = glm::mat4(1.0f); // identity matrix
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(7.0, 7.0, 7.0));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTex.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	/// FOR F22 MESH / PLANE
	model = glm::mat4(1.0f); // identity matrix
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 4.0f));
	model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Shape1.RenderModel();

}

void DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader.UseShader();

	// Draw a new Viewport to match the FrameBuffer
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	// Bind the FrameBuffer
	light->GetShadowMap()->Write();
	// Clear all depth buffer data if there is
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();

	glm::mat4 tmp = light->CalculateLightTransform();
	directionalShadowShader.SetDirectionalLightTransform(&tmp);

	directionalShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* light)
{
	omniShadowShader.UseShader();

	// Draw a new Viewport to match the FrameBuffer
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	// Clear all depth buffer data if there is
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader.SetOmniLightMatrices(light->CalculateLightTransform());

	omniShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glViewport(0, 0, 1920 / RESOLUTION, 1080 / RESOLUTION);
	// Clear window
	glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix)); // Moving the camera
	glUniform3f(uniformEyePosition, camera.GetCameraPosition().x, camera.GetCameraPosition().y, camera.GetCameraPosition().z);

	// Add Data ID to the shader
	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightsCount, 3, 0);
	shaderList[0].SetSpotLights(spotLights, spotLightsCount, 3 + pointLightsCount, pointLightsCount);

	//mainLight.direction = glm::vec3(glm::sin(lastTime), glm::cos(lastTime), 0.0f);
	glm::mat4 tmp = mainLight.CalculateLightTransform();
	shaderList[0].SetDirectionalLightTransform(&tmp);

	mainLight.GetShadowMap()->Read(GL_TEXTURE2);
	shaderList[0].SetTexture(1);
	shaderList[0].SetDirectionalShadowMap(2);

	spotLights[0].SetFlash(camera.GetCameraPosition(), camera.GetCameraDirection());

	shaderList[0].Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main()
{
	// Initialise Window
	mainWindow = Window(1920 / RESOLUTION, 1080 / RESOLUTION);
	mainWindow.Intialise();

	// Create all the customs meshes and all the shaders
	CreateObjects();
	CreateShaders();

	// Initialise the Camera
	camera = Camera
	(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		-90.0f,
		0.0f,
		5.0f,
		0.2f
	);

	// Load every custom texture (custom because they do not cam with a .mtl)
	brickTex = Texture("Textures/brick.png");
	brickTex.LoadTextureA(); // Load Texture
	dirtTex = Texture("Textures/dirt.png");
	dirtTex.LoadTextureA(); // Load Texture

	// Custom Material (because no .mtl)
	// Now it is just tweaking specular and shininess parameters
	shineMaterial = Material(1.0f, 32);
	dullMaterial = Material(0.3f, 4);

	// Load Imported Mesh
	Shape1 = Model();
	Shape1.LoadModel("Models/nanosuit.obj");

	// Setup light for the shader
	mainLight = DirectionalLight(2048, 2048,
								1.0f, 1.0f, 1.0f,
								0.1f, 0.0f,
								0.0f, -15.0f, -10.0f);

	pointLights[0] = PointLight(1024,1024,
								0.01f, 100.0f,
								1.0f, 1.0f, 1.0f,
								0.2f, 1.0f,
								0.0f, 3.7f, 4.5f,
								0.3f, 0.01f, 0.01f,
								1.0f);
	pointLightsCount++;

	pointLights[1] = PointLight(1024, 1024,
								0.01f, 100.0f,
								0.0f, 0.0f, 1.0f,
								0.2f, 1.0f,
								1.0f, 4.0f, 0.0f,
								0.3f, 0.01f, 0.01f,
								1.0f);
	//pointLightsCount++;

	spotLights[0] = SpotLight(1024, 1024,
							0.01f, 100.0f,
							1.0f, 1.0f, 1.0f,
							0.0f, 10.0f,
							-4.0f, 0.0f, 2.0f,
							0.0f, -1.0f, 0.0f,
							0.3f, 0.1f, 0.1f,
							1.0f, 20.0f);
	spotLightsCount++;

	spotLights[1] = SpotLight(1024, 1024,
							0.01f, 100.0f,
							0.0f, 1.0f, 0.0f,
							0.1f, 1.0f,
							4.0f, 0.0f, -2.0f,
							-0.4f, -1.0f, 0.0f,
							0.3f, 0.2f, 0.1f,
							1.0f, 45.0f);
	//spotLightsCount++;

	// Load every texture for the skybBox and then create it
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/skybox-pack/corona_rt.png");
	skyboxFaces.push_back("Textures/Skybox/skybox-pack/corona_lf.png");
	skyboxFaces.push_back("Textures/Skybox/skybox-pack/corona_up.png");
	skyboxFaces.push_back("Textures/Skybox/skybox-pack/corona_dn.png");
	skyboxFaces.push_back("Textures/Skybox/skybox-pack/corona_bk.png");
	skyboxFaces.push_back("Textures/Skybox/skybox-pack/corona_ft.png");

	skybox = SkyBox(skyboxFaces);

	// Setup Camera (fov, aspect ratio, near plane, far plane)
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	// Update Function
	while (!mainWindow.getShouldClose())
	{
		// Get Delta Time
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;


		// Get + Handle user inpunt events
		glfwPollEvents();

		// Get Input & Keyboard & Mouse
		// Change the Camera Position, this will be useful for changing the view matrix later in the code
		// By changing the view matrix, we actually move the camera
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getxChange(), mainWindow.getyChange());

		// Event when the key 'L' is pressed / used
		if (mainWindow.getsKeys()[GLFW_KEY_L])
		{
			spotLights[0].Toggle();
			mainWindow.getsKeys()[GLFW_KEY_L] = false;
		}

		// Add the directional Light Shadow map
		DirectionalShadowMapPass(&mainLight);
		
		// Add every Omni Light Shadow Map
		for (size_t i = 0; i < pointLightsCount; i++)
		{
			OmniShadowMapPass(&pointLights[i]);
		}
		for (size_t i = 0; i < spotLightsCount; i++)
		{
			OmniShadowMapPass(&spotLights[i]);
		}


		// Render the all scene
		RenderPass(camera.calculateViewMatrix(), projection);

		// Setup Moving Lights
		unlitShader.UseShader();
		uniformModel = unlitShader.GetModelLocation();
		uniformProjection = unlitShader.GetProjectionLocation();
		uniformView = unlitShader.GetViewLocation();
		uniformBaseColor = unlitShader.GetBaseColor();
		unlitShader.Validate();

		glm::mat4 model(1.0f); // identity matrix
		pointLights[0].position.x = glm::sin(lastTime * 0.5f + 3.14f) * 10;
		pointLights[0].position.z = glm::cos(lastTime * 0.5f + 3.14f) * 10;
		//pointLights[0].color.r = glm::sin(lastTime * 0.5f + 3.14f) * 0.5f + 0.5f;
		//pointLights[0].color.g = glm::sin(lastTime) * 0.5f + 0.5f;
		//pointLights[1].color.b = glm::sin(lastTime * 0.25f) * 0.5f + 0.5f;
		model = glm::mat4(1.0f); // identity matrix
		model = glm::translate(model, pointLights[0].position);
		//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix())); // Moving the camera
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform4f
		(
			uniformBaseColor,
			pointLights[0].GetColor().r,
			pointLights[0].GetColor().g,
			pointLights[0].GetColor().b,
			1.0f
		);
		meshList[0]->RenderMesh();

		// Working like the SwapChain (the used FrameBuffer is Swaped
		// with the previous who was already getting filled. Without vSync, the Swap 
		// can go further than your screen Hz frequency.
		mainWindow.swapBuffers();
	}

	return 0;
}