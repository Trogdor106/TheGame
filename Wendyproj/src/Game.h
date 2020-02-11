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

	//struct objCreate = filename, interactable, texture, transformations
	void CreateObjects(int objectNameID, int typeOfObject, int textureNameID, glm::mat4 transformation = glm::mat4(1.0f));
	void InitImGui();
	void ShutdownImGui();

	void ImGuiNewFrame();
	void ImGuiEndFrame();

	void Update(float deltaTime);
	void Draw(float deltaTime);
	void DrawGui(float deltaTime);

	// Interaction
	bool interactionIsPossible(glm::vec3, glm::vec3);

	bool interact(int objectID, glm::vec3 cameraPos, bool isPressed);
private:

	const char* filename[100] = { "",
		//1                //2          //3            //4             //5          //6                //7
  "1stFloor.obj",	"f_Floor3.obj",	 "Bed.obj",	 "BigVase.obj",	 "SmallVase.obj", "Book.obj",	"BookShelf.obj",
		//8                //9								 //10								 //11
	  "Door.obj", "f_Door(Reverse_open_1_3).obj", "f_Door(Reverse_open_2_3).obj", "f_Door(Reverse_open_Max).obj",
		//12					//13					   //14                //15				   //16                
	"f_Door(open_1_3).obj", "f_Door(open_2_3).obj", "f_Door(open_Max).obj", "LargeDrawer.obj", "Dresser.obj",
		//17					//18                 //19          //20         //21
	"Dresser.obj", "f_DresserWithMorror.obj",  "Key1.obj",   "Key2.obj",   "Key3.obj",
		//22           //23				 //24              //25            //26            //27					//28
	"f_Key4.obj", "Portrait.obj", "SmallFrame.obj", "SmallWindow.obj", "Stairs.obj", "f_LargeWindow.obj", "WideWindow.obj" ,
		//29                    //30                //31                    //32					//33
	"BookShelfFull.obj", "f_rotatedDoor.obj", "f_shortFlatWall.obj", "f_rotatedShortWall.obj", "FrontDoor.obj",
		//34					//35				//36		//37		  //38				  //39			//40
	"SafeRoomStairs.obj", "KitchenStairs.obj", "Piano.obj", "Toilet.obj" ,"PaintBucket.obj" , "OilCask.obj", "note.obj",
		//41	   //42
	"Sink.obj", "Mirror.obj" 
	 };

	const char* texturename[100] = { "",
		//1			//2				//3				//4					//5						//6
	"f_Bed.png", "f_Door.png", "f_doorFrame.png", "f_Drawer.png", "f_DresserNoDrawer.png", "F_fatWall.png",
		//7			//8
	"f_Key1.png", "Note.png"

	};


	// Stores the main window that the game is running in
	GLFWwindow* myWindow;
	// Stores the clear color of the game's window
	glm::vec4   myClearColor;
	// Stores the title of the game's window
	char        myWindowTitle[32];

	Camera::Sptr myCamera;
	Camera::Sptr interactCamera;

	// A shared pointer to our shader
	Shader::Sptr myShader;
	// Shader for viewing normal maps
	Shader::Sptr myNormalShader;



	//float lerp(float p1, float p2, float want, glm::mat4 objTransform, int index);

	glm::vec3 cameraViewAngle = glm::vec3(0, 0, 1);
	glm::vec3 cameraViewTarget = glm::vec3(0);

	glm::vec3 cameraPos = glm::vec3(1, 1, 10);
	glm::vec3 interactPos = glm::vec3(1, 1, 5);
	Material::Sptr testMat2; //Wut?
	//Material::Sptr testMat; //Exacly

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
	//std::vector <Mesh::Sptr> genMesh;
	std::vector <MeshData> genObjects;
	std::vector <Texture2D::Sptr> genMats;
	//std::vector <Material::Sptr> genMats;
	//std::vector <int> genAlbedo;

	//Texture2D::Sptr Albedo  = Texture2D::LoadFromFile(texturename[0]);

	std::vector <int> amountOfObjects;

	bool isPickedUp = false;
	bool isDoneReading = false;


	//int amountOfObjects = 0;
	//int amountOfMorphObjects = 0;
	//int amountOfCastleObjects = 0;

	//Mighty morphin' objects
	////It's gone :(

	glm::vec3 lanternAngle = { 0.0f, 0.0f, 0.0f };

	float angleForX = 0;
	float angleForY = 0;
	float angleForZ = 0;


	double mousePosX = 0;
	double mousePosY = 0;
	double mousePrevPosX = 0;
	double mousePrevPosY = 0;
	int windowSizeWidth = 0;
	int windowSizeHeight = 0;
	int windowPosX = 0;
	int windowPosY = 0;

	//Other
	//MorphObject morphObjectManager;
	//deCasteJau deCasteJauManager;

	float halfOfPI = 1.57079633;
};
