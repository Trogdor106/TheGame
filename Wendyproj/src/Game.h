#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>

#include "GLM/glm.hpp"

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "OBJLoader.h"
#include "Material.h"
#include "HitBoxes.h"
#include "MorphObject.h"


class Game {
public:
	Game();
	~Game();

	void Run();
	void Resize(int newWidth, int newHeight);

protected:
	void Initialize();
	void Shutdown();

	void LoadContent();
	void UnloadContent();

	void InitImGui();
	void ShutdownImGui();

	void ImGuiNewFrame();
	void ImGuiEndFrame();

	void Update(float deltaTime);
	void Draw(float deltaTime);
	void DrawGui(float deltaTime);

	// Interaction
	bool interactionIsPossible(glm::vec3, glm::vec3);
private:
	// Stores the main window that the game is running in
	GLFWwindow* myWindow;
	// Stores the clear color of the game's window
	glm::vec4   myClearColor;
	// Stores the title of the game's window
	char        myWindowTitle[32];

	Camera::Sptr myCamera;
	Camera::Sptr OrthoCameraForInteraction;

	// A shared pointer to our mesh
	Mesh::Sptr   myMesh;
	Mesh::Sptr   myMesh2;
	Mesh::Sptr   myMesh3;
	Mesh::Sptr   myMesh4;
	Mesh::Sptr   myMesh5;

	// A shared pointer to our shader
	Shader::Sptr myShader;
	// Shader for viewing normal maps
	Shader::Sptr myNormalShader;

	//ObjLoader hello;

	// Our models transformation matrix
	glm::mat4   myModelTransform;

	//float lerp(float p1, float p2, float want, glm::mat4 objTransform, int index);

	glm::vec3 cameraViewAngle = glm::vec3(0, 0, 1);
	glm::vec3 cameraViewTarget = glm::vec3(0);

	glm::vec3 cameraPos = glm::vec3(1, 1, 5);
	Material::Sptr testMat2; //Wut?

	HitBoxes hitBoxManager;

	float backupDeltatime;

	//Lantern stuff prob want to seperate into another file later
	float lanternFuel = 1.0f;
	float lightShyninessModifyer = 1.0f;
	float lightAttenuationModifyer = 1.0f;
	glm::mat4 myLanternTransform = glm::mat4(1.0f);
	glm::vec3 lanternAngle = {0.0f, 0.0f, 0.0f};
	float angleForX = 0;
	float angleForY = 0;
	float angleForZ = 0;


	//That's it for now I guess XD

	double mousePosX = 0;
	double mousePosY = 0;
	double mousePrevPosX = 0;
	double mousePrevPosY = 0;
	int windowSizeWidth = 0;
	int windowSizeHeight = 0;
	int windowPosX = 0;
	int windowPosY = 0;

	//Objects
	std::vector <Vertex> lanternVertices;

	//Morph objects
	MorphObject morphObjectManager;
};
