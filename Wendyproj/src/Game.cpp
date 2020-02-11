#pragma once
#include "Game.h"
#include "Logging.h"
#include <functional>
#include "ObjectLoader2.h"
#include <stdexcept>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "imgui_impl_opengl3.cpp"
#include "imgui_impl_glfw.cpp"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include "SceneManager.h"
#include "MeshRenderer.h"
#include "Texture2D.h"



/*
	Handles debug messages from OpenGL
	https://www.khronos.org/opengl/wiki/Debug_Output#Message_Components
	@param source    Which part of OpenGL dispatched the message
	@param type      The type of message (ex: error, performance issues, deprecated behavior)
	@param id        The ID of the error or message (to distinguish between different types of errors, like nullref or index out of range)
	@param severity  The severity of the message (from High to Notification)
	@param length    The length of the message
	@param message   The human readable message from OpenGL
	@param userParam The pointer we set with glDebugMessageCallback (should be the game pointer)
*/

void lerp(glm::vec3& goal, glm::vec3& startPoint, glm::vec3& currentPos, float duration) {
	static float totalTime = duration;
	static float amountOfTimes = totalTime / (0.009025);
	static float percentagePerIntervals = (0.009025) / totalTime;
	static float currentInterval = 0;
	if (currentInterval <= amountOfTimes && currentInterval >= 0) {
		float t = startPoint.x != goal.x ? (((percentagePerIntervals * currentInterval) * (goal.x - startPoint.x) + startPoint.x) - startPoint.x) / (goal.x - startPoint.x) : 1;
		currentPos.x = (1 - t) * startPoint.x + t * goal.x;
		float t2 = startPoint.y != goal.y ? (((percentagePerIntervals * currentInterval) * (goal.y - startPoint.y) + startPoint.y) - startPoint.y) / (goal.y - startPoint.y) : 1;
		currentPos.y = (1 - t2) * startPoint.y + t2 * goal.y;
		float t3 = startPoint.z != goal.z ? (((percentagePerIntervals * currentInterval) * (goal.z - startPoint.z) + startPoint.z) - startPoint.z) / (goal.z - startPoint.z) : 1;
		currentPos.z = (1 - t3) * startPoint.z + t3 * goal.z;

		currentInterval += 1;

		//All the 0.095s are because delta time was giving us varying results based on the computers, so this constant works better with smaller numbers
	}
}

struct UpdateBehaviour {
	std::function<void(entt::entity e, float dt)> Function;
};

struct TempTransform {
	glm::vec3 Position;
	glm::vec3 EulerRotation;
	glm::vec3 Scale;
	glm::mat4 GetWorldTransform() const {
		return
			glm::translate(glm::mat4(1.0f), Position) * glm::mat4_cast(glm::quat(glm::radians(EulerRotation))) * glm::scale(glm::mat4(1.0f), Scale);
	}
};

void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:          LOG_INFO(message); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN(message); break;
	case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR(message); break;
#ifdef LOG_GL_NOTIFICATIONS
	case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO(message); break;
#endif
	default: break;
	}
}

void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	Game* game = (Game*)glfwGetWindowUserPointer(window);
	if (game) {
		game->Resize(width, height);
	}
}

void Game::Resize(int newWidth, int newHeight) {
	myCamera->Projection = glm::perspective(glm::radians(60.0f), newWidth / (float)newHeight, 0.01f, 1000.0f);
}

Game::Game() :
	myWindow(nullptr),
	myWindowTitle("Mallory"),
	myClearColor(glm::vec4(0, 0, 0, 1))
	//genTransform()
{ }

Game::~Game() { }

void Game::Run()
{
	Initialize();
	InitImGui();

	LoadContent();

	static float prevFrame = glfwGetTime();

	// Run as long as the window is open
	while (!glfwWindowShouldClose(myWindow)) {
		// Poll for events from windows (clicks, keypressed, closing, all that)
		glfwPollEvents();

		float thisFrame = glfwGetTime();
		float deltaTime = thisFrame - prevFrame;

		Update(deltaTime);
		Draw(deltaTime);

		ImGuiNewFrame();
		DrawGui(deltaTime);
		ImGuiEndFrame();

		// Store this frames time for the next go around
		prevFrame = thisFrame;

		// Present our image to windows
		glfwSwapBuffers(myWindow);
	}

	LOG_INFO("Shutting down...");

	UnloadContent();

	ShutdownImGui();
	Shutdown();
}

void Game::Initialize() {
	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		throw std::runtime_error("Failed to initialize GLFW");
	}

	// Enable transparent backbuffers for our windows (note that Windows expects our colors to be pre-multiplied with alpha)
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

	// Create a new GLFW window
	myWindow = glfwCreateWindow(600, 600, myWindowTitle, nullptr, nullptr);

	// Tie our game to our window, so we can access it via callbacks
	glfwSetWindowUserPointer(myWindow, this);

	// Set our window resized callback
	glfwSetWindowSizeCallback(myWindow, GlfwWindowResizedCallback);

	// We want GL commands to be executed for our window, so we make our window's context the current one
	glfwMakeContextCurrent(myWindow);

	// Let glad know what function loader we are using (will call gl commands via glfw)
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		throw std::runtime_error("Failed to initialize GLAD");
	}

	// Log our renderer and OpenGL version
	LOG_INFO(glGetString(GL_RENDERER));
	LOG_INFO(glGetString(GL_VERSION));

	// Enable debugging, and route messages to our callback
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GlDebugMessage, this);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

}

void Game::Shutdown() {
	glfwTerminate();
}


glm::vec4 testColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
void Game::LoadContent() {
	myCamera = std::make_shared<Camera>();
	myCamera->SetPosition(cameraPos);
	myCamera->LookAt(cameraViewTarget, cameraViewAngle);
	myCamera->Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 1000.0f);

	interactCamera = std::make_shared<Camera>();
	interactCamera->SetPosition(glm::vec3(1, 1, 10));
	interactCamera->LookAt(glm::vec3(0.0f, 0.0f, 4), glm::vec3(0, 0, 1));
	interactCamera->Projection = glm::ortho(-22.0f, 22.0f, -10.0f, 10.0f, 0.0f, 1000.0f);

	//Texture2D::Sptr albedo = Texture2D::LoadFromFile("f_Door.png");
	Texture2D::Sptr albedo2 = Texture2D::LoadFromFile("F_fatWall.png");



	//std::vector <Vertex> temp;
	//Replaced with
	MeshData MeshTemp = ObjLoader::LoadObj("f_Door.obj", glm::vec4(1.0f));
	
	//Type 0 is nothing, type 1 is unlocked door, type 2 is locked door, type 3 is drawer, type 4 is a note, type 5 is the oil cask (refueling)
	///////////////Load objects and textures under here
	Game::CreateObjects(1, 0, 5); // object 0 floor 2
	//texture 5 is dresser no drawers
	Game::CreateObjects(11, 1, 2); // object 1 is door topside of saferoom
	Game::CreateObjects(8, 1, 2); // object 2 is door right side in safe room
	Game::CreateObjects(11, 1, 2); // object 3 is door bottomside of saferoom
	Game::CreateObjects(8, 2, 2); // object 4 is top door across of saferoom
	Game::CreateObjects(8, 1, 2); // object 5 is left bathroom door
	Game::CreateObjects(16, 0, 5); // object 6 is the dresser in the batrhoom(?)
	Game::CreateObjects(15, 3, 4); // object 7 is the top drawer for object 6
	Game::CreateObjects(15, 0, 4); // object 8 is the middle drawer for object 6
	Game::CreateObjects(15, 0, 4); // object 9 is the bottom drawer for object 6
	Game::CreateObjects(28, 0, 7); // object 10 is the boarded up window in the safe room (left wall)
	Game::CreateObjects(25, 0, 6); // object 11 is a second boarded up window
	Game::CreateObjects(4, 0, 6); // object 12 is a big vase
	Game::CreateObjects(4, 0, 6); // object 13 is a big vase
	Game::CreateObjects(29, 0, 6); // object 14 is a full bookshelf
	Game::CreateObjects(29, 0, 6); // object 15 is a full bookshelf, look at all those vertices
	Game::CreateObjects(11, 0, 6); // object 16 is the study door
	Game::CreateObjects(11, 0, 6); // object 17 is the right door of the dining room
	Game::CreateObjects(11, 0, 6); // object 18 is the left door of the dining room
	Game::CreateObjects(11, 0, 6); //Object 19 is the door to the pantry
	Game::CreateObjects(33, 0, 6); //Object 20 is the front door
	Game::CreateObjects(34, 0, 6); //Object 21 is the safe room staircase
	Game::CreateObjects(35, 0, 6); //Object 22 is the kitchen staircase
	Game::CreateObjects(36, 0, 6); //Object 23 is the piano
	Game::CreateObjects(37, 0, 6); //Object 24 is the 1st floor toilet
	Game::CreateObjects(41, 0, 6); //Object 25 is the bathroom sink
	Game::CreateObjects(42, 0, 6); //Object 25 is the mirror above the sink
	//Game::CreateObjects(8, 1, 2);
	//Game::CreateObjects(34, 4, 8);

	int hello = 0;
	Shader::Sptr phong = std::make_shared<Shader>();
	phong->Load("lighting.vs.glsl", "blinn-phong.fs.glsl");
	
	Shader::Sptr phong2 = std::make_shared<Shader>();
	phong2->Load("lighting.vs.glsl", "blinn-phong.fs.glsl");

	//lerpVS->Load("lerp.vs.glsl", "blinn-phong.fs.glsl");

	//for (int i = 0; i < amountOfObjects.size(); i++)
	//{
	//	Mesh::Sptr temp;
	//	if (amountOfObjects[i] == 3)
	//		temp = std::make_shared<Mesh>(genObjects[i].data(), genObjects[i].size(), nullptr, 0);// , genObjects[2].data(), genObjects[2].size(), nullptr, 0);
	//	else 
	//		temp = std::make_shared<Mesh>(genObjects[i].data(), genObjects[i].size(), nullptr, 0);// , genObjects[i].data(), genObjects[i].size(), nullptr, 0);
	//	//MeshBuilder::Bake(data2);
	//	//temp =
	//	Bake(genObjects[0])
	//	genMesh.push_back(temp);
	//}

	//testMat = std::make_shared<Material>(phong2);
	//
	//testMat->Set("s_Albedo", albedo2);
	//testMat->Set("a_LightPos", { 0, 1, 10 });
	//testMat->Set("a_LightColor", { 1.0f, 1.0f, 0.0f });
	//testMat->Set("a_AmbientColor", { 1.0f, 1.0f, 1.0f });
	//testMat->Set("a_AmbientPower", 0.1f);
	//testMat->Set("a_LightSpecPower", 1.0f);

	testMat2 = std::make_shared<Material>(phong);
	testMat2->Set("s_Albedo", albedo2);
	testMat2->Set("a_LightPos", { 0, 1, 10 });
	testMat2->Set("a_LightColor", { 1.0f, 1.0f, 0.0f });
	testMat2->Set("a_AmbientColor", { 1.0f, 1.0f, 1.0f });
	testMat2->Set("a_AmbientPower", 0.8f);
	testMat2->Set("a_LightSpecPower", 1.0f);

	SceneManager::RegisterScene("Test2");
	SceneManager::SetCurrentScene("Test2");

	{

		auto& ecs2 = GetRegistry("Test2");
		
		std::vector <entt::entity> genEntt;

		for (int i = 0; i < amountOfObjects.size(); i++) {
			entt::entity temp = ecs2.create();
			genEntt.push_back(temp);
			ecs2.assign<TempTransform>(genEntt[i]).Scale = glm::vec3(1.0f);
			MeshRenderer& genMesh2 = ecs2.assign<MeshRenderer>(genEntt[i]);

			//Texture2D::Sptr tempText = Texture2D::LoadFromFile(genMats[i]);
			//testMat2->Set("s_Albedo", tempText);
			genMesh2.Material = testMat2;
			genMesh2.Mesh = Bake(genObjects[i]);
			

			switch (i) {
			case 0: //Object 0 The saferoom
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(44.2f, 49.9f, 0.0f));
				break;
			case 1: //Object 1 Door right saferoom
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(50.5f, 11.5f, -0.1f));
				break;
			case 2:// object 2 is door bottom of saferoom
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(3.85f, -11.1f, -0.1f));
				genTransform[i] = glm::rotate(genTransform[i], 2 * halfOfPI, glm::vec3(0, 0, 1));
				break;
			case 3:// object 3 door right across saferoom
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(61.6f, 11.5f, -0.1f));
				break;
			case 4:// object 4 is door top across saferoom
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(12.7f, 49.0f, -0.1));
				break;
			case 5:// object 5 is door top safe room
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(12.7f, 36.5f, -0.1));
				genTransform[i] = glm::rotate(genTransform[i], 2 * halfOfPI, glm::vec3(0, 0, 1));
				break;
			case 6:// object 6 is the dresser in the room where you find the red key
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(96.55f, -9.0f, -110.1f));
				genTransform[i] = glm::rotate(genTransform[i], 2 * halfOfPI, glm::vec3(0, 0, 1));
				break;
			case 7:// object 7 is the top drawer in the room where you find the red key 
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(96.55f, -7.92f, 161.1f));
				genTransform[i] = glm::rotate(genTransform[i], -halfOfPI, glm::vec3(0, 0, 1));
				break;
			case 8:// object 8 is the middle drawer in the room where you find the red key
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(96.55f, -7.92f, 113.8f));
				genTransform[i] = glm::rotate(genTransform[i], -halfOfPI, glm::vec3(0, 0, 1));
				break;
			case 9:// object 9 is the bottom drawer in the room where you find the red key
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(96.55f, -7.92f, 111.8f));
				genTransform[i] = glm::rotate(genTransform[i], -halfOfPI, glm::vec3(0, 0, 1));
				break;
			case 10:// object 10 is a long broken window in the safe room
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(-11.85f, 16.5f, 7.5f));
				break;
			case 11:// object 11 is a broken window in the stairwell
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(-11.85f, -17.5f, 7.5f));
				break;
			case 12:// object 12 is a big vase
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(-8.85f, 32.5f, 0.0f));
				break;
			case 13:// object 13 is a big vase
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(-7.00f, 27.5f, 0.0f));
				break;
			case 14:// object 14 is the bookshelf with way, way too many faces
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(24.0f, -8.1f, 0.0f));
				break;
			case 15:// object 15 is the bookshelf in the study
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(122.0f, -39.0f, 0.0f));
				break;
			case 16:// object 16 is the study door
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(164.75f, -9.5f, 0.0f));
				break;
			case 17:// object 17 is the right kitchen door
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(164.75f, 21.05f, 0.0f));
				break;

			case 18:// object 18 is the left kitchen door
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(102.0f, 11.5f, 0.0f));
				break;

			case 19:// object 19 is the pantry door
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(102.0f, 42.0f, 0.0f));
				break;

			case 20:// object 20 is the front door
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(79.75f, 97.5f, 0));
				break;

			case 21:// object 21 is the safe room staircase
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(15.85f, -15.85f, 0.0f));
				break;

			case 22:// object 22 is the kitchen staircase
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(192.0f, -0.9f, 0.0f));
				break;

			case 23:// object 23 is the piano
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(118.0f, -25.0f, 0.0f));
				break;

			case 24:// object 24 is the 1st floor toilet
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(96.55f, -9.0f, -0.1f));
				break;

			case 25:// object 25 is the bathroom sink
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(90.55f, 24.0f, 0.0f));
				break;

			case 26:// object 25 is the mirror above the sink
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(90.55f, 26.0f, 7.0f));
				break;
			}
		}
	}


	//for (int i = 0; i < genObjects.size(); i++) {
	//	if (amountOfObjects[i] == 0) {
	//		hitBoxManager.saveHitBoxes(genObjects[i], 0);
	//		hitBoxManager.updateHitBoxes(genTransform[i][3], i);
	//	}
	//	else if (amountOfObjects[i] == 1) {
	//		hitBoxManager.saveHitBoxes(genObjects[i], 1);
	//		hitBoxManager.updateHitBoxes(genTransform[i][3], i);
	//	}
	//	else {
	//		hitBoxManager.saveHitBoxes(genObjects[i], 2);
	//		hitBoxManager.updateHitBoxes(genTransform[i][3], i);
	//	}
	//}
    /////////////////////////////////////////////////////////////////////////HitBoxes
}

void Game::CreateObjects(int objectNameID, int typeOfObject, int textureNameID, glm::mat4 transformation)
{
	genObjects.push_back(ObjLoader::LoadObj(filename[objectNameID], glm::vec4(1.0f)));
	amountOfObjects.push_back(typeOfObject);  
	genMats.push_back(Texture2D::LoadFromFile(texturename[textureNameID]));
	genTransform.push_back(transformation);
}

void Game::UnloadContent() {

}


void Game::InitImGui() {
	// Creates a new ImGUI context
	ImGui::CreateContext();
	// Gets our ImGUI input/output 
	ImGuiIO& io = ImGui::GetIO();
	// Enable keyboard navigation
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	// Allow docking to our window
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// Allow multiple viewports (so we can drag ImGui off our window)
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// Allow our viewports to use transparent backbuffers
	io.ConfigFlags |= ImGuiConfigFlags_TransparentBackbuffers;

	// Set up the ImGui implementation for OpenGL
	ImGui_ImplGlfw_InitForOpenGL(myWindow, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	// Dark mode FTW
	ImGui::StyleColorsDark();

	// Get our imgui style
	ImGuiStyle& style = ImGui::GetStyle();
	//style.Alpha = 1.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.8f;
	}
}

void Game::ShutdownImGui() {
	// Cleanup the ImGui implementation
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	// Destroy our ImGui context
	ImGui::DestroyContext();
}

void Game::ImGuiNewFrame() {
	// Implementation new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	// ImGui context new frame
	ImGui::NewFrame();
}

void Game::ImGuiEndFrame() {
	// Make sure ImGui knows how big our window is
	ImGuiIO& io = ImGui::GetIO();
	int width{ 0 }, height{ 0 };
	glfwGetWindowSize(myWindow, &width, &height);
	io.DisplaySize = ImVec2(width, height);

	// Render all of our ImGui elements
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// If we have multiple viewports enabled (can drag into a new window)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		// Update the windows that ImGui is using
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		// Restore our gl context
		glfwMakeContextCurrent(myWindow);
	}
}

void Game::Update(float deltaTime) {
	backupDeltatime = deltaTime;

	

	glm::vec3 movement = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);

	mousePrevPosX = windowPosX + windowSizeWidth / 4;
	mousePrevPosY = windowPosY + windowSizeHeight / 4;

	static bool isMouse = false;
	static bool isM = false;

	//glfwSetCursorPosCallback
	//glfwGetCursorPos
	//glfwSetCursorPos(myWindow, , 1);
	if (glfwGetKey(myWindow, GLFW_KEY_M) == GLFW_PRESS) {
		if (!isM) {
			isMouse = isMouse == true ? false : true;
			glfwSetCursorPos(myWindow, windowPosX + windowSizeWidth / 4, windowPosY + windowSizeHeight / 4);
			isM = true;
		}
	}
	else {
		isM = false;
	}
	if (isMouse) {
		glfwSetInputMode(myWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwGetCursorPos(myWindow, &mousePosX, &mousePosY);
		glfwGetWindowPos(myWindow, &windowPosX, &windowPosY);
		glfwGetWindowSize(myWindow, &windowSizeWidth, &windowSizeHeight);
		glfwSetCursorPos(myWindow, windowPosX + windowSizeWidth / 4, windowPosY + windowSizeHeight / 4);
		//rotation.x = (mousePosX - mousePrevPosX) * deltaTime;


		rotation.y = (mousePosX - mousePrevPosX) * deltaTime;
		rotation.x = (mousePosY - mousePrevPosY) * deltaTime;

		//Proper way to do it, but not enough time for tomorrow so will be left to the me of a later day
		//cameraViewAngle.x += (mousePosY - mousePrevPosY) * deltaTime;
		//myCamera->LookAt(cameraViewTarget, cameraViewAngle);
	}

	float speed = 30.0f;
	float speed2 = 15.0f;
	float rotSpeed = 1.0f;
	bool isheadBob = false;
	//myCamera->LookAt(cameraViewTarget, cameraViewAngle);

	if (glfwGetKey(myWindow, GLFW_KEY_S) == GLFW_PRESS) {
		movement.z = -speed2 * deltaTime;
		isheadBob = true;
	}
	else if (glfwGetKey(myWindow, GLFW_KEY_W) == GLFW_PRESS) {
		movement.z = speed2 * deltaTime;
		isheadBob = true;
	}
	else {
		movement.z = 0 * deltaTime;
	}
	if (glfwGetKey(myWindow, GLFW_KEY_D) == GLFW_PRESS) {
		movement.x = -speed2 * deltaTime;
		isheadBob = true;
	}
	else if (glfwGetKey(myWindow, GLFW_KEY_A) == GLFW_PRESS) {
		movement.x = speed2 * deltaTime;
		isheadBob = true;
	}
	else {
		movement.x = 0;
	}

	if (glfwGetKey(myWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
		movement.y += speed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		movement.y -= speed * deltaTime;

	if (glfwGetKey(myWindow, GLFW_KEY_ESCAPE)) {
		exit(1);
	}

	if (glfwGetKey(myWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
		angleForZ -= 0.01;
		if (angleForZ < -1.4) {
			angleForZ = -1.4;
		}
	}
	if (glfwGetKey(myWindow, GLFW_KEY_UP) == GLFW_PRESS) {
		angleForZ += 0.01;
		if (angleForZ > 1.4) {
			angleForZ = 1.4;
		}
	}
	if (glfwGetKey(myWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		angleForY += 0.01;
		angleForX -= 0.01;
	}
	if (glfwGetKey(myWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
		angleForY -= 0.01;
		angleForX += 0.01;
	}
	
	static bool isEPressed = false;
	if (glfwGetKey(myWindow, GLFW_KEY_E) == GLFW_PRESS) {
		if (isEPressed == false) {
			isEPressed = true;
		}
	}
	else {
		isEPressed = false;
	}

	// Rotate and move our camera based on input
	
	//myCamera->Rotate(rotation);

	static float headBob = 0.0f;
	float finalHeadBob = sin(headBob) * 0.03;
	float finalHeadBobSide = sin(headBob * 0.5) * 0.01;
	if (isheadBob == true) {
		headBob += 0.10;
	}
	else {
		finalHeadBob = 0;
		finalHeadBobSide = 0;
	}

	myCamera->LookAt({ myCamera->GetPosition().x + cos(-angleForX), myCamera->GetPosition().y + sin(-angleForY), myCamera->GetPosition().z + tan(angleForZ)}, cameraViewAngle);

	

	myCamera->SetPosition({ 
		myCamera->GetPosition().x + movement.z * cos(angleForX) + movement.x * cos(angleForX + 1.57078145) + finalHeadBobSide * cos(angleForX + 1.57078145), 
		myCamera->GetPosition().y + movement.z * sin(angleForX) + movement.x * sin(angleForX + 1.57078145) + finalHeadBobSide * sin(angleForX + 1.57078145),
		myCamera->GetPosition().z + finalHeadBob});

	

	//myCamera->Move(movement);

	glm::mat4 temp = myCamera->GetView();

	cameraPos = myCamera->GetPosition();
	
	//hitBoxManager.updateHitBoxes(glm::vec3(myLanternTransform[3][0], myLanternTransform[3][1], myLanternTransform[3][2]), 0);
	//Will add once the camera follows the correct format
	
	//testMat2->Set("a_LightPos", { cameraPos + glm::vec3(-6, -2, 0) + glm::vec3(cos(lanternAngle.x), sin(lanternAngle.y), tan(lanternAngle.z)) });
	
	//HitBoxing, will be a for loop going through objects once we have things in place
	//for (int i = 0; i < genObjects.size(); i++)
	//{
	//	if (i == 0)
	//		;
	//
	//	else if (hitBoxManager.testHitBoxes(cameraPos, 0)) {
	//		myCamera->SetPosition(cameraPos);
	//	}
	//}


	//myLanternTransform = glm::translate(myLanternTransform, glm::vec3(cameraPos + glm::vec3(-6, -2, 0)));
	float extraDist = 10;
	float offSet = 0.5;

	testMat2->Set("a_LightPos", { (myCamera->GetPosition().x + cos(-angleForX) + extraDist * cos(angleForX - offSet)), //+ offSet * cos(angleForX), 
									  (myCamera->GetPosition().y + sin(-angleForY) + extraDist * sin(-angleForY - offSet)), //+ offSet * sin(-angleForY),
									  myCamera->GetPosition().z + (angleForZ > -1 ? (angleForZ < 1 ? tan(angleForZ) : 1) : -1) });

		if (glfwGetKey(myWindow, GLFW_KEY_R) == GLFW_PRESS) {
			lanternFuel = 2000; //How long will the lantern last
			lightAttenuationModifyer = 1.0f;
			lightShyninessModifyer = 1.0f;
		}
		

		if (lanternFuel > 1000) {
			testMat2->Set("a_LightColor", { 1.0f, 1.0f, 1.0f });

			//Brightness Smaller the more shinny the surface is and the better it will show light
			testMat2->Set("a_LightShininess", 85.0f);

			//Radius of glow Bigger the wider range and brighter it becomes
			testMat2->Set("a_LightAttenuation", 0.00125f);
		}
		else if (lanternFuel > 100) {
			//lightShyninessModifyer += 0.001;
			lightAttenuationModifyer += 0.05;
			//Brightness Smaller the more shinny the surface is and the better it will show light
			testMat2->Set("a_LightShininess", 56.0f * lightShyninessModifyer);

			//Radius of glow Bigger the wider range and brighter it becomes
			testMat2->Set("a_LightAttenuation", 0.00125f * lightAttenuationModifyer);
		}
		else {
			testMat2->Set("a_LightColor", { 0.0f, 0.0f, 0.0f });
		}
	
	lanternFuel -= 1; //Just makes the lantern fuel drain
	
	//morphObjectManager.updateMorphObject(deltaTime, 0, myLanternTransform);
	///lanternVertices = morphObjectManager.getCurrentModel(0);
	//myMesh5 = std::make_shared<Mesh>(lanternVertices.data(), lanternVertices.size(), nullptr, 0);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////mighty morphin' door rangers
	//	myMesh5 = nullptr;
	//
	//	morphObjectManager.updateMorphObject();
	//	lanternVertices = morphObjectManager.getCurrentModel(0);
	//
	//	myMesh5 = std::make_shared<Mesh>(lanternVertices.data(), lanternVertices.size(), nullptr, 0);

	//myMesh4 = nullptr;

	//deCasteJauManager.calculatedeCasteJau();
	//morphObjectManager.updateMorphObject();


	//int deCasteObj = 0;
	//int morphObject = 0;
	//int lerpObject = 0;
	//static glm::vec3 start = genTransform[7][3];
	//static bool isLerpActive = false;
	//for (int i = 0; i < amountOfObjects.size(); i++) {
	//	if (amountOfObjects[i] == 0) {
	//		;
	//	}
	//	else if (amountOfObjects[i] == 2) {
	//		genMesh[i] = nullptr;
	//		genObjects[i] = deCasteJauManager.getCurrentCasteJau(deCasteObj);
	//		genMesh[i] = std::make_shared<Mesh>(genObjects[i].data(), genObjects[i].size(), nullptr, 0);// , genObjects[i].data(), genObjects[i].size(), nullptr, 0);
	//		if (Game::interact(i, myCamera->GetPosition(), isEPressed)) {
	//				deCasteJauManager.switchToTrue(deCasteObj);
	//		}
	//		deCasteObj++;
	//	}
	//	else if (amountOfObjects[i] == 1) {
	//		genMesh[i] = nullptr;
	//		genObjects[i] = morphObjectManager.getCurrentModel(morphObject);
	//		genMesh[i] = std::make_shared<Mesh>(genObjects[i].data(), genObjects[i].size(), nullptr, 0);// , genObjects[i].data(), genObjects[i].size(), nullptr, 0);
	//		if (Game::interact(i, myCamera->GetPosition(), isEPressed)) {
	//			morphObjectManager.switchToTrue(morphObject);
	//		}
	//		morphObject++;
	//	}
	//	else if (amountOfObjects[i] == 3 && isLerpActive == false) {
	//		if (Game::interact(i, myCamera->GetPosition(), isEPressed)) {
	//			isLerpActive = true;
	//		}
	//	}
	//}
	//
	//if (isLerpActive) {
	//	glm::vec3 temp = genTransform[7][3];
	//	lerp(start + glm::vec3(1.5f, 0.0f, 0.0f), start, temp, 3);
	//	genTransform[7][3].x = temp.x;
	//	genTransform[7][3].y = temp.y;
	//	genTransform[7][3].z = temp.z;
	//}
	//
	//
	//
	//static bool bufferP = false;
	//static int buffer1 = 0;
	//static int buffer2 = 0;
	//static float buffer3 = 0;
	//if (glfwGetKey(myWindow, GLFW_KEY_P) == GLFW_PRESS) {
	//	if (interactionIsPossible(cameraPos, { genTransform[27][3].x, genTransform[27][3].y, genTransform[27][3].z })) {
	//		if (!bufferP) {
	//			if (isPickedUp && isDoneReading == false) {
	//				isDoneReading = true;
	//			}
	//			isPickedUp = true;
	//		}
	//	}
	//	bufferP = true;
	//}
	//else {
	//	bufferP = false;
	//}
	//if (isPickedUp && isDoneReading == false) {
	//	if (buffer1 < 100) {
	//		genTransform[27] = glm::rotate(genTransform[27], deltaTime, glm::vec3(0.0f, 0.0f, 1.0f));
	//		buffer1++;
	//	}
	//	if (buffer2 < 72) {
	//		genTransform[27] = glm::rotate(genTransform[27], deltaTime, glm::vec3(0.0f, -1.0f, 0.0f));
	//		buffer2++;
	//	}
	//	if (buffer3 < 180) {
	//		interactCamera->Projection = glm::ortho(-20.0f + buffer3 / 10, 20.0f - buffer3 / 10, -10.5f + buffer3 / 20, 9.5f - buffer3 / 20, 0.0f, 1000.0f);
	//		buffer3++;
	//	}
	//	int i = 0;
	//	//for (genObjects[27])
	//	//if (buffer3 < 72) {
	//	//	genTransform[27] = glm::rotate(genTransform[27], deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
	//	//	buffer3++;
	//	//}
	//}


	//chairVertices = deCasteJauManager.getCurrentCasteJau(0);

	//doorVertices = deCasteJauManager.getCurrentCasteJau(0);

	//myMesh4 = std::make_shared<Mesh>(chairVertices.data(), chairVertices.size(), nullptr, 0);

}

void Game::Draw(float deltaTime) {
	static float time = 0.0;
	// Clear our screen every frame
	glClearColor(myClearColor.x, myClearColor.y, myClearColor.z, myClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//myShader->Bind();
	//myShader->SetUniform("a_ModelViewProjection", interactCamera->GetViewProjection());
	//myMesh->Draw();

	// We'll grab a reference to the ecs to make things easier   
	auto& ecs = CurrentRegistry();

	// We sort our mesh renderers based on material properties   
	// This will group all of our meshes based on shader first, then material second   
	ecs.sort<MeshRenderer>([](const MeshRenderer& lhs, const MeshRenderer& rhs) {
		if (rhs.Material == nullptr || rhs.Mesh == nullptr)
			return false;
		else if (lhs.Material == nullptr || lhs.Mesh == nullptr)
			return true;
		else if (lhs.Material->GetShader() != rhs.Material->GetShader())
			return lhs.Material->GetShader() < rhs.Material->GetShader();
		else
			return lhs.Material < rhs.Material;
		});

	// These will keep track of the current shader and material that we have bound    
	Material::Sptr mat = nullptr;
	Shader::Sptr   boundShader = nullptr;
	// A view will let us iterate over all of our entities that have the given component types    
	auto view = ecs.view<MeshRenderer>();
	int hello = genObjects.size() - 1;


	for (const auto& entity : view) {
		// Get our shader    
		const MeshRenderer& renderer = ecs.get<MeshRenderer>(entity);
		// Early bail if mesh is invalid 
		if (renderer.Mesh == nullptr || renderer.Material == nullptr)
			continue;
		
		//renderer.Mesh = Bake(genObjects[hello]);
		
		// If our shader has changed, we need to bind it and update our frame-level uniforms    
		if (renderer.Material->GetShader() != boundShader) {
			boundShader = renderer.Material->GetShader();
			boundShader->Bind();
			//boundShader->SetUniform("a_CameraPos", interactCamera->GetPosition());
			boundShader->SetUniform("a_CameraPos", myCamera->GetPosition());
		}
		//if (amountOfObjects[hello] == 3) {
		//	testMat2->Set("inGoalPosition", genMats[hello]);
		//	
		//		//genMesh[hello];
		//}
		// If our material has changed, we need to apply it to the shader    
			
			
		
		//testMat->Set("s_Albedo", genMats[hello]);
		testMat2->Set("s_Albedo", genMats[hello]);
		//if (renderer.Material != mat) {
			mat = renderer.Material;
			mat->Apply();
		//}


		//}
		// We'll need some info about the entities position in the world 
		const TempTransform& transform = ecs.get_or_assign<TempTransform>(entity);
		// Our normal matrix is the inverse-transpose of our object's world rotation
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform.GetWorldTransform())));
		// Update the MVP using the item's transform  
		
		if (amountOfObjects[hello] == 3) {
			//testMat->Set("time", time);
			mat->GetShader()->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() * genTransform[hello]);
			//mat->GetShader()->SetUniform("a_ModelViewProjection", interactCamera->GetViewProjection() * genTransform[hello]);
			// Update the model matrix to the item's world transform
			mat->GetShader()->SetUniform("a_Model", transform.GetWorldTransform());
			// Update the model matrix to the item's world transform 
			mat->GetShader()->SetUniform("a_NormalMatrix", normalMatrix);
			// Draw the item   
			renderer.Mesh->Draw();
		}
		else if (amountOfObjects[hello] == 4) {
			if (isPickedUp && isDoneReading == false) {
				//mat->GetShader()->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() * genTransform[hello]);
				mat->GetShader()->SetUniform("a_ModelViewProjection", interactCamera->GetViewProjection() * genTransform[hello]);
		
		
				// Update the model matrix to the item's world transform
				mat->GetShader()->SetUniform("a_Model", transform.GetWorldTransform());
				// Update the model matrix to the item's world transform 
				mat->GetShader()->SetUniform("a_NormalMatrix", normalMatrix);
				// Draw the item   
				renderer.Mesh->Draw();
				
			}
			else if (isDoneReading == false) {
				mat->GetShader()->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() * genTransform[hello]);
				//mat->GetShader()->SetUniform("a_ModelViewProjection", interactCamera->GetViewProjection() * genTransform[hello]);
		
		
				// Update the model matrix to the item's world transform
				mat->GetShader()->SetUniform("a_Model", transform.GetWorldTransform());
				// Update the model matrix to the item's world transform 
				mat->GetShader()->SetUniform("a_NormalMatrix", normalMatrix);
				// Draw the item   
				renderer.Mesh->Draw();
			}
		}
		else {
			boundShader->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() * genTransform[hello]);
			//mat->GetShader()->SetUniform("a_ModelViewProjection", interactCamera->GetViewProjection() * genTransform[hello]);


			// Update the model matrix to the item's world transform
			boundShader->SetUniform("a_Model", transform.GetWorldTransform());
			// Update the model matrix to the item's world transform 
			boundShader->SetUniform("a_NormalMatrix", normalMatrix);
			// Draw the item   
			renderer.Mesh->Draw();
		}
		
		hello--;
	}
	time += 0.02;
}

void Game::DrawGui(float deltaTime) {
	// Open a new ImGui window
	static bool testOpen = true;
	ImGui::Begin("Test", &testOpen, ImVec2(300, 200));
	// Draw a color editor
	auto it = SceneManager::Each();
	for (auto& kvp : it) {
		if (ImGui::Button(kvp.first.c_str())) {
			SceneManager::SetCurrentScene(kvp.first);
		}
	}
	
	
	ImGui::ColorEdit4("Clear Color", &myClearColor[0]);
	// Check if a textbox has changed, and update our window title if it has
	if (ImGui::InputText("Window Title", myWindowTitle, 32)) {
		glfwSetWindowTitle(myWindow, myWindowTitle);
	}
	// Our object's test color
	ImGui::ColorEdit4("Object Color", &testColor[0]);
	ImGui::End();
	
	// Open a second ImGui window
	ImGui::Begin("Debug");
	// Draw a formatted text line
	ImGui::Text("Time: %f", glfwGetTime());
	
	// Start a new ImGui header for our camera settings
	if (ImGui::CollapsingHeader("Camera Settings")) {
		// Draw our camera's normal
		glm::vec3 camNormal = myCamera->GetForward();
		ImGui::DragFloat3("Normal", &camNormal[0]);
	
		// Get the camera's position so we can edit it
		glm::vec3 position = myCamera->GetPosition();
		// Draw an editor control for the position, and update camera position
		if (ImGui::DragFloat3("Position", &position[0])) {
			myCamera->SetPosition(position);
		}
		if (ImGui::Button("Look at center")) {
			myCamera->LookAt(glm::vec3(0), glm::vec3(0, 0, 1));
		}
		// Get the camera pinning value
		static glm::vec3 camPin;
	
		// Get whether or not camera pinning is enabled
		bool camPlaneEnabled = myCamera->GetPinnedUp().has_value();
		// Draw a checkbox for camera pinning
		if (ImGui::Checkbox("Pinning Enabled", &camPlaneEnabled)) {
			// If we've disabled pinning, cache our pinning vector and remove it
			if (!camPlaneEnabled) {
				camPin = myCamera->GetPinnedUp().value();
				myCamera->SetPinnedUp(std::optional<glm::vec3>());
			}
			// Set our camera's pinning vector to our cached value
			else {
				myCamera->SetPinnedUp(camPin);
			}
		}
		// If we have enabled pinning
		if (camPlaneEnabled) {
			// Draw a slider for our camera pin direction
			if (ImGui::InputFloat3("Pin Direction", &camPin[0])) {
				myCamera->SetPinnedUp(camPin);
			}
		}
	}
	ImGui::End();
}

bool Game::interactionIsPossible(glm::vec3 playerPos, glm::vec3 objectPos)
{
	float distanceX = abs((playerPos.x) - (objectPos.x));

	float distanceY = abs((playerPos.y) - (objectPos.y));
	if (distanceX < 10 && distanceY < 10) {
		return true;
	}
	return false;
}

bool Game::interact(int objectID, glm::vec3 cameraPos, bool isPressed)
{
	if (isPressed) {
		if ((hitBoxManager.isInRangeToInteract(cameraPos, objectID))) {
			return true;
		}
	}
	return false;
}
