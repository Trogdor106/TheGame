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
#include "simpleMeshAndVertexFor.h"
#include "AudioEngine.h"

void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height);

class Game {
public:
	Game();
	~Game();

	void Run();
	void Resize(int newWidth, int newHeight);

protected:
	void Initialize();
	void Shutdown();

	void LoadSimpleContent();
	void LoadContent();
	void UnloadContent();

	//struct objCreate = filename, interactable, texture, transformations
	void CreateObjects(int objectNameID, int typeOfObject, int textureNameID, int floor = 0, glm::mat4 transformation = glm::mat4(1.0f));
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
	void saveHitAndMesh(MeshAndHitBox toSave, int floor);
	void alwaysActiveKeys();

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

	void loadMusic();
	void updateMusic();
	void playSound();
	void shutDownSound();
	void createSoundParam(std::string name, int timerLength, glm::vec3 position, int toPlay = 0);

private:
	int gameState = 0;
	int currentFloor = 0;


	int firstLoop = 0;


	const char* filename[100] = { "",
		//1							 //2					//3						 //4						//5						 //6               
	"Objects/1stFloor.obj",	"Objects/2ndFloor.obj",	 "Objects/Bed.obj",	 "Objects/BigVaseUV.obj",	 "Objects/SmallVase.obj", "Objects/Book.obj",
				//7
		"Objects/BookShelf.obj",
		//8						    //9								 //10										 //11
	  "Objects/Door.obj", "Objects/f_Door(Reverse_open_1_3).obj", "Objects/f_Door(Reverse_open_2_3).obj", "Objects/DoorOpen.obj",
		//12								//13							  //14								//15				   //16                
	"Objects/f_Door(open_1_3).obj", "Objects/f_Door(open_2_3).obj", "Objects/f_Door(open_Max).obj", "Objects/LargeDrawer.obj", "Objects/Dresser.obj",
		//17							//18						   //19					 //20				 //21
	"Objects/Dresser.obj", "Objects/f_DresserWithMorror.obj",  "Objects/Key1.obj",   "Objects/Key2.obj",   "Objects/Key3.obj",
		//22					    //23					 //24					 //25							//26          
	"Objects/f_Key4.obj", "Objects/Portrait.obj", "Objects/SmallFrame.obj", "Objects/SmallWindow.obj", "Objects/Stairs.obj",
					//27					//28
		"Objects/f_LargeWindow.obj", "Objects/WideWindow.obj" ,
				//29					  //30							//31									 //32					//33
	"Objects/BookShelfFull.obj", "Objects/f_rotatedDoor.obj", "Objects/f_shortFlatWall.obj", "Objects/f_rotatedShortWall.obj", "Objects/FrontDoor.obj",
				//34							//35					//36				//37				 //38							//39			
	"Objects/SafeRoomStairs.obj", "Objects/KitchenStairs.obj", "Objects/Piano.obj", "Objects/Toilet.obj" ,"Objects/PaintBucket.obj" , "Objects/NewCask.obj", 
				//40
		"Objects/note.obj",
			//41					  //42				  //43							//44					 //45					//46					
	"Objects/sink UV.obj", "Objects/Mirror.obj", "Objects/Fireplace.obj", "Objects/KitchenTable.obj", "Objects/Cupboards.obj", "Objects/HallwayWindow.obj", 
						//47
		"Objects/FireplaceSmallWindow.obj",
			//48
	"Objects/Ceiling.obj"
	};

	const char* texturename[100] = { "",
			//1						//2					//3							//4							//5								//6				
	"textures/f_Bed.png", "textures/f_Door.png", "textures/f_doorFrame.png", "textures/f_Drawer.png", "textures/f_DresserNoDrawer.png", "textures/F_fatWall.png",
		//7
	"textures/NewCask.png",
			//8						//9					//10						 //11					 //12					//13		   
	"textures/f_Key1.png", "textures/Note.png", "textures/KitchenTable.png", "textures/Cupboards.png", "textures/Door.png", "textures/DoorOpen.png",
			//14					//15
	"textures/1stFloor.png", "textures/WideWindow.png",
			//16				//17					//18					  //19							//20					
	"textures/Piano.png", "textures/Sink.png", "textures/BigVase.png", "textures/BookShelfFull.png", "textures/KitchenStairs.png",
			//21						  //22
	"textures/SmallWindow.png", "textures/FrontDoor.png",
			//23				   //24						 //25					 //26				  //27					  //28
	"textures/Toilet.png", "textures/Fireplace.png", "textures/Ceiling.png", "textures/Key1.png", "textures/Portrait.png", "textures/Portrait2.png"
	};

	int objectUpdate(int ID);
	std::vector <int> objectIDToHaveHitBox;


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
	const float LANTERNFUELDEFAULT = 2000;
	int lanternFuelFullPowerBuffer = 1000;
	int deathTimer = 100;
	const int DEATHTIMERMAX = 100;

	float lightShyninessModifyer = 1.0f;
	float lightAttenuationModifyer = 1.0f;
	glm::mat4 myLanternTransform = glm::mat4(1.0f);
	glm::mat4 myLanternTransform2 = glm::mat4(1.0f);


	//std::vector <AudioEngine> audioEngineManager;
	std::vector <int> soundsToPlay;
	std::vector <int> timerForSoundsdefault;
	std::vector <int> timerForSounds;
	std::vector <std::string> soundsName;
	std::vector <glm::vec3> soundPosition;

	
	//
	Shader::Sptr	simpleShader;
	Meshmini_sptr   mySimpleMesh;
	

	
	//Normal objects
	std::vector <glm::mat4> genTransform;
	std::vector <MeshData> genObjects;
	std::vector <Texture2D::Sptr> genMats;

	std::vector <int> amountOfObjects;
	std::vector <int> objectsToUpdate;
	std::vector <int> objFloor;

	bool isPickedUp = false;
	bool isDoneReading = false;
	bool interactBuffer = true;

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
	int windowSizeWidth = 600;
	int windowSizeHeight = 600;
	int windowPosX = 0;
	int windowPosY = 0;


	float halfOfPI = 1.57079633;



	Shader::Sptr gBufferShader;
	Mesh::Sptr myFullscreenQuad;
	FrameBuffer::Sptr myAccumulationBuffer;
	
};
