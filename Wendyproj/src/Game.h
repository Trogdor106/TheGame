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
#include "GeneralMath.h"


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

	const char* filename[100] = { "",
		//1                //2                //3            //4                    //5                //6                //7
	  "f_Floor2.obj",	"f_Floor3.obj",	 "f_Bed.obj",	 "f_BigVase.obj",	 "f_SmallVase.obj",		"f_Book.obj",	"f_BookShelf.obj",
		//8                //9								 //10								 //11
		"f_Door.obj", "f_Door(Reverse_open_1_3).obj", "f_Door(Reverse_open_2_3).obj", "f_Door(Reverse_open_Max).obj",
		//12					//13                    //14                //15                //16                //17
		"f_Door(open_1_3)", "f_Door(open_2_3)", "f_Door(open_Max)", "f_Drawer.obj", "f_DresserNoDrawer.obj",
		//17					//18                //18            //19          //20        //21
	  "f_Dresser.obj", "f_DresserWithMorror.obj", "f_Key1.obj", "f_Key2.obj", "f_Key3.obj",
		//21			  //22              //23				 //24              //25                //26
	  "f_Key4.obj", "f_Portrait.obj", "f_SmallFrame.obj", "f_SmallWindow.obj", "f_Stairs.obj", "f_LargeWindow.obj" };

	// Stores the main window that the game is running in
	GLFWwindow* myWindow;
	// Stores the clear color of the game's window
	glm::vec4   myClearColor;
	// Stores the title of the game's window
	char        myWindowTitle[32];

	Camera::Sptr myCamera;
	Camera::Sptr interactCamera;
	
	
	// A shared pointer to our mesh
	Mesh::Sptr   myMesh;
	Mesh::Sptr   myMesh2;
	Mesh::Sptr   myMesh3;
	Mesh::Sptr   myMesh4;
	Mesh::Sptr   myMesh5;

	Mesh::Sptr	 dresser[6];
	Mesh::Sptr	 door[40];

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
	glm::vec3 interactPos = glm::vec3(1, 1, 5);
	Material::Sptr testMat2; //Wut?

	HitBoxes hitBoxManager;

	float backupDeltatime;

	//Lantern stuff prob want to seperate into another file later
	float lanternFuel = 1.0f;
	float lightShyninessModifyer = 1.0f;
	float lightAttenuationModifyer = 1.0f;
	glm::mat4 myLanternTransform = glm::mat4(1.0f);
	glm::mat4 myLanternTransform2 = glm::mat4(1.0f);

	//Normal objects
	std::vector <glm::mat4> genTransform;

	//Mighty morphin' objects
	////

	glm::vec3 lanternAngle = {0.0f, 0.0f, 0.0f};
	float angleForX = 0;
	float angleForY = 0;
	float angleForZ = 0;

	glm::vec3 dresserAngle[6];


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
	std::vector <Vertex> chairVertices;

	std::vector <Vertex> dresserVertices;
	std::vector <Vertex> doorVertices;

	//Other
	MorphObject morphObjectManager;
	deCasteJau deCasteJauManager;
};
