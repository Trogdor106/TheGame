#pragma once

#include "FrameState.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>

#include "GLM/glm.hpp"
#include <GLM/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "ObjectLoader2.h"
#include "Material.h"
#include "HitBoxes.h"
#include "GeneralMath.h"
#include "SceneManager.h"
#include "MeshRenderer.h"
#include "Texture2D.h"
#include "florp/graphics/BufferLayout.h"
#include "FrameBuffer.h"

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

	void pauseScreen();
	void gBufferInit();
	void gBuffer();
	void preRender();
	void saveHitAndMesh(MeshAndHitBox toSave);

	void UnloadGBuffer();
	struct TempTransform {
		glm::vec3 Position;
		glm::vec3 EulerRotation;
		glm::vec3 Scale;
		glm::mat4 GetWorldTransform() const {
			return
				glm::translate(glm::mat4(1.0f), Position) * glm::mat4_cast(glm::quat(glm::radians(EulerRotation))) * glm::scale(glm::mat4(1.0f), Scale);
		}
	};
	struct UpdateBehaviour {
		std::function<void(entt::entity e, float dt)> Function;
	};
	struct PointLight {
		
		glm::vec3 Color;
		float     Attenuation;
	};
private:
	int gameState = 0;
	int firstLoop = 0;

	const char* filename[100] = { "",
		//1                //2          //3            //4             //5          //6                //7
	"1stFloor.obj",	"2ndFloor.obj",	 "Bed.obj",	 "BigVaseUV.obj",	 "SmallVase.obj", "Book.obj",	"BookShelf.obj",
		//8                //9								 //10								 //11
	  "Door.obj", "f_Door(Reverse_open_1_3).obj", "f_Door(Reverse_open_2_3).obj", "DoorOpen.obj",
		//12					//13					   //14                //15				   //16                
	"f_Door(open_1_3).obj", "f_Door(open_2_3).obj", "f_Door(open_Max).obj", "LargeDrawer.obj", "Dresser.obj",
		//17					//18                 //19          //20         //21
	"Dresser.obj", "f_DresserWithMorror.obj",  "Key1.obj",   "Key2.obj",   "Key3.obj",
		//22           //23				 //24              //25            //26            //27					//28
	"f_Key4.obj", "Portrait.obj", "SmallFrame.obj", "SmallWindow.obj", "Stairs.obj", "f_LargeWindow.obj", "WideWindow.obj" ,
		//29                    //30                //31                    //32					//33
	"BookShelfFull.obj", "f_rotatedDoor.obj", "f_shortFlatWall.obj", "f_rotatedShortWall.obj", "FrontDoor.obj",
		//34					//35				//36		//37		  //38				  //39			//40
	"SafeRoomStairs.obj", "KitchenStairs.obj", "Piano.obj", "Toilet.obj" ,"PaintBucket.obj" , "NewCask.obj", "note.obj",
		//41	     //42			  //43				//44			  //45				//46						//47
	"sink UV.obj", "Mirror.obj", "Fireplace.obj", "KitchenTable.obj", "Cupboards.obj", "HallwayWindow.obj", "FireplaceSmallWindow.obj",
		//48
	"Ceiling.obj"
	};

	const char* texturename[100] = { "",
		//1			//2				//3				//4					//5						//6				//7
	"f_Bed.png", "f_Door.png", "f_doorFrame.png", "f_Drawer.png", "f_DresserNoDrawer.png", "F_fatWall.png", "NewCask.png",
		//8			//9				//10			  //11			 //12			//13		   //14				//15
	"f_Key1.png", "Note.png", "KitchenTable.png", "Cupboards.png", "Door.png", "DoorOpen.png", "1stFloor.png", "WideWindow.png",
		//16		//17		//18			  //19					//20				//21			  //22
	"Piano.png", "Sink.png", "BigVase.png", "BookShelfFull.png", "KitchenStairs.png", "SmallWindow.png", "FrontDoor.png",
		//23		   //24			 //25			 //26		  //27			  //28
	"Toilet.png", "Fireplace.png", "Ceiling.png", "Key1.png", "Portrait.png", "Portrait2.png"
	};

	int objectUpdate(int ID);

	// Stores the main window that the game is running in
	GLFWwindow* myWindow;
	// Stores the clear color of the game's window
	glm::vec4   myClearColor;
	// Stores the title of the game's window
	char        myWindowTitle[32];

	Camera::Sptr myCamera;
	Camera::Sptr interactCamera;

	// A shared pointer to our shader
	//Shader::Sptr myShader;
	// Shader for viewing normal maps
	//Shader::Sptr myNormalShader;
	// A test



	//float lerp(float p1, float p2, float want, glm::mat4 objTransform, int index);

	glm::vec3 cameraViewAngle = glm::vec3(0, 0, 1);
	glm::vec3 cameraViewTarget = glm::vec3(0);

	glm::vec3 cameraPos = glm::vec3(1, 1, 10);
	glm::vec3 interactPos = glm::vec3(1, 1, 5);
	Material::Sptr testMat2; //Wut?
	//Material::Sptr testMat; //Exacly

	HitBoxes hitBoxManager;

	//Lantern stuff prob want to seperate into another file later
	float lanternFuel = 1.0f;
	float lightShyninessModifyer = 1.0f;
	float lightAttenuationModifyer = 1.0f;
	glm::mat4 myLanternTransform = glm::mat4(1.0f);
	glm::mat4 myLanternTransform2 = glm::mat4(1.0f);

	
	//
	
	
	
	//Normal objects
	std::vector <glm::mat4> genTransform;
	std::vector <MeshData> genObjects;
	std::vector <Texture2D::Sptr> genMats;

	std::vector <int> amountOfObjects;
	std::vector <int> objectsToUpdate;

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


	float halfOfPI = 1.57079633;



	Shader::Sptr gBufferShader;
	Mesh::Sptr myFullscreenQuad;
	FrameBuffer::Sptr myAccumulationBuffer;
	
};
