#pragma once
#include "Game.h"
#include "Logging.h"
#include <functional>

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
	static float amountOfTimes = totalTime / (0.095);
	static float percentagePerIntervals = (0.095) / totalTime;
	static float currentInterval = 0;
	if (currentInterval <= amountOfTimes && currentInterval >= 0) {
		float t = startPoint.x != goal.x ? (((percentagePerIntervals * currentInterval) * (goal.x - startPoint.x) + startPoint.x) - startPoint.x) / (goal.x - startPoint.x) : 1;
		currentPos.x = (1 - t) * startPoint.x + t * goal.x;
		float t2 = startPoint.y != goal.y ? (((percentagePerIntervals * currentInterval) * (goal.y - startPoint.y) + startPoint.y) - startPoint.y) / (goal.y - startPoint.y) : 1;
		currentPos.y = (1 - t2) * startPoint.y + t2 * goal.y;
		float t3 = startPoint.z != goal.z ? (((percentagePerIntervals * currentInterval) * (goal.z - startPoint.z) + startPoint.z) - startPoint.z) / (goal.z - startPoint.z) : 1;
		currentPos.z = (1 - t3) * startPoint.z + t3 * goal.z;

		currentInterval += 0.095;

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
	myWindowTitle("Game"),
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
	interactCamera->SetPosition(cameraPos);
	interactCamera->LookAt(cameraViewTarget, cameraViewAngle);
	interactCamera->Projection = glm::ortho(-4, 4, -4, 4, 1, 1000);

	Texture2D::Sptr albedo = Texture2D::LoadFromFile("untitled.png");
	Texture2D::Sptr albedo2 = Texture2D::LoadFromFile("f_fatWall.png");
	
	//Halp
	//std::vector <Vertex> objVertices = loadOBJ("Chair.obj");
	//deCasteJauManager.saveDeCasteJauObject("Chair.obj", "Chair2.obj", "Chair3.obj", "Chair4.obj");

	std::vector <Vertex> temp;
	///////////////Load objects under here
	temp = loadOBJ(filename[1]);
	genObjects.push_back(temp);
	amountOfObjects++; // object 0 is floor 2

	temp = loadOBJ(filename[8]);
	genObjects.push_back(temp);
	amountOfObjects++; // object 1 is door topside of saferoom

	genObjects.push_back(temp);
	amountOfObjects++; // object 2 is door leftside of saferoom

	genObjects.push_back(temp);
	amountOfObjects++; // object 3 is door bottomside of saferoom

	genObjects.push_back(temp);
	amountOfObjects++; // object 4 is locked red door in front of door bottomside of saferoom

	genObjects.push_back(temp);
	amountOfObjects++; // object 5 is door in behind of door leftside of saferoom

	temp = loadOBJ(filename[16]);
	genObjects.push_back(temp);
	amountOfObjects++; // object 6 is the drawer in the room where you find the red key

	temp = loadOBJ(filename[15]);
	genObjects.push_back(temp);
	amountOfObjects++; // object 7 is the drawer things in the room where you find the red key

	genObjects.push_back(temp);
	amountOfObjects++; // object 8 is the drawer things in the room where you find the red key

	genObjects.push_back(temp);
	amountOfObjects++; // object 9 is the drawer things in the room where you find the red key

	temp = loadOBJ(filename[28]);
	genObjects.push_back(temp);
	amountOfObjects++; // object 4 is locked red door in front of door bottomside of saferoom


	//deCasteJauManager.saveDeCasteJauObject("Door.obj", "DoorHuge.obj", "DoorAss.obj", "DoorImplode.obj");
	//temp = deCasteJauManager.getCurrentCasteJau(0);
	//genObjects.push_back(temp);
	//amountOfObjects++;



//  morphObjectManager.saveMorphObject("Door.obj", "Dresser.obj");
//	lanternVertices = loadOBJ("DoorImplode.obj");
//	lanternVertices = morphObjectManager.getCurrentModel(0);

	


	//MeshData lanternVertices = ObjLoader::LoadObj("Dresser.obj");
	
	
	for (int i = 0; i < genObjects.size(); i++) {
		if (i != 9999) {
			hitBoxManager.saveHitBoxes(genObjects[i]);
		}
		else {
			;
		}
	}


	// Create a new mesh from the data
	
	//myMesh = std::make_shared<Mesh>(vertices, 4, indices, 6);
	//myMesh2 = std::make_shared<Mesh>(vertices2, 4, indices2, 6);
	//myMesh3 = std::make_shared<Mesh>(vertices3, 4, indices, 6);
	//myMesh4 = std::make_shared<Mesh>(chairVertices.data(), chairVertices.size(), nullptr, 0);
	//myMesh5 = std::make_shared<Mesh>(lanternVertices.data(), lanternVertices.size(), nullptr, 0);
	//Mesh::Sptr myMesh5 = ObjLoader::LoadObjToMesh("Dresser.obj");

	
	for (int i = 0; i < amountOfObjects; i++)
	{
		Mesh::Sptr temp = std::make_shared<Mesh>(genObjects[i].data(), genObjects[i].size(), nullptr, 0);
		genMesh.push_back(temp);
	}

	Shader::Sptr phong = std::make_shared<Shader>();
	phong->Load("lighting.vs.glsl", "blinn-phong.fs.glsl");

	float dist = 10.0f;
	Material::Sptr testMat = std::make_shared<Material>(phong);
	testMat->Set("s_Albedo", albedo);
	testMat->Set("a_LightPos", { 0, 0, 1 });
	testMat->Set("a_LightColor", glm::vec3(1.0f, 1.0f, 0) / dist);
	testMat->Set("a_AmbientColor", { 1.0f, 1.0f, 1.0f });
	testMat->Set("a_AmbientPower", 1.1f);
	testMat->Set("a_LightSpecPower", 0.5f);
	testMat->Set("a_LightShininess", 256);
	testMat->Set("a_LightAttenuation", 1.0f / (dist * dist));

	testMat2 = std::make_shared<Material>(phong);
	testMat2->Set("s_Albedo", albedo2);
	testMat2->Set("a_LightPos", { 0, 1, 10 });
	testMat2->Set("a_LightColor", { 0.0f, 1.0f, 0.0f });
	testMat2->Set("a_AmbientColor", { 1.0f, 1.0f, 1.0f });
	testMat2->Set("a_AmbientPower", 0.4f);
	testMat2->Set("a_LightSpecPower", 1.0f);
	
	//Brightness
	testMat2->Set("a_LightShininess", 1);

	//Radius of glow
	testMat2->Set("a_LightAttenuation", 0.1f);



	// Create and compile shader
	//myShader = std::make_shared<Shader>();
	//myShader->Load("passthrough.vs", "passthrough.fs");

	//myNormalShader = std::make_shared<Shader>();
	//myNormalShader->Load("passthrough.vs", "normalView.fs");

	SceneManager::RegisterScene("Test2");
	SceneManager::SetCurrentScene("Test2");

	{

		auto& ecs2 = GetRegistry("Test2");
		//entt::entity e2 = ecs2.create();
		//ecs2.assign<TempTransform>(e2).Scale = glm::vec3(1.0f);
		//MeshRenderer& m2 = ecs2.assign<MeshRenderer>(e2);
		//m2.Material = testMat2;
		//m2.Mesh = myMesh2;
		//
		//auto rotate = [](entt::entity e, float dt) {
		//	CurrentRegistry().get<TempTransform>(e).EulerRotation += glm::vec3(0, 0, 90 * dt);
		//};
		

		//entt::entity e4 = ecs2.create();
		//ecs2.assign<TempTransform>(e4).Scale = glm::vec3(1.0f);
		//MeshRenderer& m3 = ecs2.assign<MeshRenderer>(e4);
		//m3.Material = testMat2;
		//m3.Mesh = myMesh4;
		
		//entt::entity doorEntity[40];
		//
		//for (int i = 0; i < 40; i++)
		//{
		//	doorEntity[i] = ecs2.create();
		//	ecs2.assign<TempTransform>(doorEntity[i]).Scale = glm::vec3(1.0f);
		//	MeshRenderer& doorMesh = ecs2.assign<MeshRenderer>(doorEntity[i]);
		//	doorMesh.Material = testMat2;
		//	doorMesh.Mesh = door[i];
		//}
		
		//morphObjectManager.updateMorphObject(0.000000003, 0, myLanternTransform);
		
		//if (backupDeltatime != -431602080.) {
		//	morphObjectManager.updateMorphObject();
		//	lanternVertices = morphObjectManager.getCurrentModel(0);
		//	myMesh5 = std::make_shared<Mesh>(lanternVertices.data(), lanternVertices.size(), nullptr, 0);
		//	int hello = 0;
		//}

		//entt::entity e5 = ecs2.create();
		//ecs2.assign<TempTransform>(e5).Scale = glm::vec3(1.0f);
		//MeshRenderer& m4 = ecs2.assign<MeshRenderer>(e5);
		//m4.Material = testMat2;
		//m4.Mesh = myMesh5;

		//entt::entity dresserEntity[6];

		std::vector <entt::entity> genEntt;

		for (int i = 0; i < amountOfObjects; i++) {
			entt::entity temp = ecs2.create();
			genEntt.push_back(temp);
			ecs2.assign<TempTransform>(genEntt[i]).Scale = glm::vec3(1.0f);
			MeshRenderer& genMesh2 = ecs2.assign<MeshRenderer>(genEntt[i]);
			genMesh2.Material = testMat2;
			genMesh2.Mesh = genMesh[i];

		}
		
		//for (int i = 0; i < 6; i++)
		//{
		//	dresserEntity[i] = ecs2.create();
		//	ecs2.assign<TempTransform>(dresserEntity[i]).Scale = glm::vec3(1.0f);
		//	MeshRenderer& dresserMesh = ecs2.assign<MeshRenderer>(dresserEntity[i]);
		//	dresserMesh.Material = testMat2;
		//	dresserMesh.Mesh = dresser[i];
		//}


	//auto rotate2 = [](entt::entity e, float dt) {
	//	CurrentRegistry().get<TempTransform>(e).EulerRotation += glm::vec3(0, 30 * dt, 90 * dt);
	//};
	//auto rotate3 = [](entt::entity e, float dt) {
	//	CurrentRegistry().get<TempTransform>(e).EulerRotation = glm::vec3(0, 90, 30);
	//};
	//auto& up = ecs2.get_or_assign<UpdateBehaviour>(e2);
	//up.Function = rotate;
	//	auto& up2 = ecs2.get_or_assign<UpdateBehaviour>(e5);
	//	up2.Function = rotate3;


	}
	myLanternTransform = glm::translate(myLanternTransform, glm::vec3(0, 0, 1));
	myLanternTransform2 = glm::translate(myLanternTransform2, glm::vec3(25, 0, 1));

	for (int i = 0; i < genObjects.size(); i++)
	{
		glm::mat4 temp = glm::mat4(1.0f);
		genTransform.push_back(temp);
		//dresserAngle[i] = glm::vec3(0, 0, 0);
	}

	for (int i = 0; i < genObjects.size(); i++) {
		switch (i) {
		case 1: //Door topside of the saferoom
			genTransform[i] = glm::translate(genTransform[i], glm::vec3(-27.5f, 20.0f, 0));
			genTransform[i] = glm::rotate(genTransform[i], halfOfPI, glm::vec3(0, 0, 1));
			break;
		case 2:// object 2 is door leftside of saferoom
			genTransform[i] = glm::translate(genTransform[i], glm::vec3(0.0f, -37.0f, 0));
			genTransform[i] = glm::rotate(genTransform[i], 2 * halfOfPI, glm::vec3(0, 0, 1));
			break;
		case 3:// object 2 is door bottomside of saferoom
			genTransform[i] = glm::translate(genTransform[i], glm::vec3(20.0f, 15.0f, 0));
			genTransform[i] = glm::rotate(genTransform[i], halfOfPI, glm::vec3(0, 0, 1));
			break;
		case 4:// object 4 is locked red door in front of door bottomside of saferoom
			genTransform[i] = glm::translate(genTransform[i], glm::vec3(34.0f, 15.0f, 0));
			genTransform[i] = glm::rotate(genTransform[i], halfOfPI, glm::vec3(0, 0, 1));
			break;
		case 5:// object 5 is door in behind of door leftside of saferoom
			genTransform[i] = glm::translate(genTransform[i], glm::vec3(0.0f, -49.0f, 0));
			genTransform[i] = glm::rotate(genTransform[i], 2 * halfOfPI, glm::vec3(0, 0, 1));
			break;
		case 6:// object 6 is the drawer in the room where you find the red key
			genTransform[i] = glm::translate(genTransform[i], glm::vec3(-25.0f, -80.0f, 0));
			genTransform[i] = glm::rotate(genTransform[i], 2 * halfOfPI, glm::vec3(0, 0, 1));
			break;
		case 7:// object 7 is the drawer things in the room where you find the red key
			genTransform[i] = glm::translate(genTransform[i], glm::vec3(-25.0f, -80.0f, 0));
			genTransform[i] = glm::rotate(genTransform[i], -halfOfPI, glm::vec3(0, 0, 1));
			break;
		case 8:// object 8 is the drawer things in the room where you find the red key
			genTransform[i] = glm::translate(genTransform[i], glm::vec3(-25.0f, -80.0f, -0.6));
			genTransform[i] = glm::rotate(genTransform[i], -halfOfPI, glm::vec3(0, 0, 1));
			break;
		case 9:// object 9 is the drawer things in the room where you find the red key
			genTransform[i] = glm::translate(genTransform[i], glm::vec3(-25.0f, -80.0f, -1.2));
			genTransform[i] = glm::rotate(genTransform[i], -halfOfPI, glm::vec3(0, 0, 1));
			break;
		case 10:
			genTransform[i] = glm::translate(genTransform[i], glm::vec3(0.0f, 0.0f, 4));
			break;


		}
	}

	//static int dummy = 1;
	//for (int i = 0; i < 7; i++)
	//{
	//	genTransform[i] = glm::translate(genTransform[i], glm::vec3(10 * dummy, 10 * dummy, 10 * dummy));
	//	dummy++;
	//}

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
	//myCamera->LookAt(cameraViewTarget, cameraViewAngle);

	if (glfwGetKey(myWindow, GLFW_KEY_S) == GLFW_PRESS)
		movement.z = -speed2 * deltaTime;
	else if (glfwGetKey(myWindow, GLFW_KEY_W) == GLFW_PRESS)
		movement.z = speed2 * deltaTime;
	else
		movement.z = 0 * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_D) == GLFW_PRESS)
		movement.x = -speed2 * deltaTime;
	else if (glfwGetKey(myWindow, GLFW_KEY_A) == GLFW_PRESS)
		movement.x = speed2 * deltaTime;
	else
		movement.x = 0;

	if (glfwGetKey(myWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
		movement.y += speed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		movement.y -= speed * deltaTime;

	if (glfwGetKey(myWindow, GLFW_KEY_ESCAPE)) {
		exit(1);
	}

	if (glfwGetKey(myWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
		//rotation.x -= rotSpeed * deltaTime;
		//angleForX -= 0.01;
		angleForZ -= 0.01;
		if (angleForZ < -1.4) {
			angleForZ = -1.4;
		}
	}
	if (glfwGetKey(myWindow, GLFW_KEY_UP) == GLFW_PRESS) {
		//rotation.x += rotSpeed * deltaTime;
		//angleForX += 0.01;
		angleForZ += 0.01;
		if (angleForZ > 1.4) {
			angleForZ = 1.4;
		}
	}
	if (glfwGetKey(myWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		//rotation.y -= rotSpeed * deltaTime;
		angleForY += 0.01;
		angleForX -= 0.01;
	}
	if (glfwGetKey(myWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
		//rotation.y += rotSpeed * deltaTime;
		angleForY -= 0.01;
		angleForX += 0.01;
	}

	bool inRange = false;
	glm::vec3 distance = {  abs(abs(myLanternTransform[3][0]) - abs(myCamera->GetPosition().x)),
							abs(abs(myLanternTransform[3][1]) - abs(myCamera->GetPosition().y)),
							abs(abs(myLanternTransform[3][2]) - abs(myCamera->GetPosition().z)) };

	if (distance.x < 5 && distance.y < 5 && distance.z < 5)
		inRange = true;

	static float currentTime = 0;
	currentTime = currentTime + deltaTime;
	static bool wasLPressed = false;
	static bool isLPressed = false;
	if (glfwGetKey(myWindow, GLFW_KEY_L) == GLFW_PRESS && wasLPressed == false && inRange == true)
	{
		wasLPressed = true;
	}

	static glm::vec3 start = myLanternTransform[3];
	glm::vec3 currentPos = myLanternTransform[3];
	static glm::vec3 end = glm::vec3(myLanternTransform[3][0], myLanternTransform[3][1], myLanternTransform[3][2] + 20);
	static float startTime = 0;

	if (wasLPressed)
		lerp(end, start, currentPos, (10));

	myLanternTransform[3][0] = currentPos.x;
	myLanternTransform[3][1] = currentPos.y;
	myLanternTransform[3][2] = currentPos.z;


	// Rotate and move our camera based on input
	
	//myCamera->Rotate(rotation);
	
	myCamera->LookAt({ myCamera->GetPosition().x + cos(-angleForX), myCamera->GetPosition().y + sin(-angleForY), myCamera->GetPosition().z + tan(angleForZ)}, cameraViewAngle);

	myCamera->SetPosition({ myCamera->GetPosition().x + movement.z * cos(angleForX) + movement.x * cos(angleForX + 1.57078145), myCamera->GetPosition().y + movement.z * sin(angleForX) + movement.x * sin(angleForX + 1.57078145), myCamera->GetPosition().z });

	float extraDist = 10;
	float offSet = 0.5;

	testMat2->Set("a_LightPos", { (myCamera->GetPosition().x + cos(-angleForX) + extraDist * cos (angleForX - offSet)), //+ offSet * cos(angleForX), 
								  (myCamera->GetPosition().y + sin(-angleForY) + extraDist * sin(-angleForY - offSet)), //+ offSet * sin(-angleForY),
								  myCamera->GetPosition().z + (angleForZ > -1 ? (angleForZ < 1 ? tan(angleForZ) : 1) : -1) });


	//myCamera->Move(movement);



	glm::mat4 temp = myCamera->GetView();

	// Rotate our transformation matrix a little bit each frame
	//myModelTransform = glm::rotate(myModelTransform, deltaTime, glm::vec3(0, 0, 1));


	//auto view = CurrentRegistry().view<UpdateBehaviour>();
	//for (const auto& e : view) {
	//	auto& func = CurrentRegistry().get<UpdateBehaviour>(e);
	//	if (func.Function) {
	//		func.Function(e, deltaTime);
	//	}
	//}

	


	cameraPos = myCamera->GetPosition();
	
	hitBoxManager.updateHitBoxes(glm::vec3(myLanternTransform[3][0], myLanternTransform[3][1], myLanternTransform[3][2]), 0);
	//Will add once the camera follows the correct format
	
	//testMat2->Set("a_LightPos", { cameraPos + glm::vec3(-6, -2, 0) + glm::vec3(cos(lanternAngle.x), sin(lanternAngle.y), tan(lanternAngle.z)) });
	

	
	//HitBoxing, will be a for loop going through objects once we have things in place
	//if (hitBoxManager.testHitBoxes(cameraPos, 0)) {
	//	myCamera->SetPosition(cameraPos);
	//}



	//myLanternTransform = glm::translate(myLanternTransform, glm::vec3(cameraPos + glm::vec3(-6, -2, 0)));

	if (glfwGetKey(myWindow, GLFW_KEY_U) == GLFW_PRESS) {
		lanternFuel = 2000; //How long will the lantern last
		lightAttenuationModifyer = 1.0f;
		lightShyninessModifyer = 1.0f;
	}
	lanternFuel -= 1; //Just makes the lantern fuel drain

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


	deCasteJauManager.calculatedeCasteJau();
	morphObjectManager.updateMorphObject();

	//chairVertices = deCasteJauManager.getCurrentCasteJau(0);

	//doorVertices = deCasteJauManager.getCurrentCasteJau(0);

	//myMesh4 = std::make_shared<Mesh>(chairVertices.data(), chairVertices.size(), nullptr, 0);

}

void Game::Draw(float deltaTime) {
	// Clear our screen every frame
	glClearColor(myClearColor.x, myClearColor.y, myClearColor.z, myClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//myScene.Render(deltaTime);

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
	static int hello = 0;
	
	
	


	for (const auto& entity : view) {
		// Get our shader    
		MeshRenderer& renderer = ecs.get<MeshRenderer>(entity);
		// Early bail if mesh is invalid 
		if (renderer.Mesh == nullptr || renderer.Material == nullptr)
			continue;
		if (hello == 0) {
		//	renderer.Mesh = myMesh5;
		//	for (int i = 0; i < 6; i++)
		//		renderer.Mesh = dresser[i];
		//	for (int i = 0; i < 40; i++)
		//		renderer.Mesh = door[i];
		}
		//	else if (hello == 1) {
		//		renderer.Mesh = myMesh4;
		//	}
		renderer.Mesh = genMesh[hello];
		// If our shader has changed, we need to bind it and update our frame-level uniforms    
		if (renderer.Material->GetShader() != boundShader) {
			
			boundShader = renderer.Material->GetShader();
			boundShader->Bind();
			boundShader->SetUniform("a_CameraPos", myCamera->GetPosition());
		}
		// If our material has changed, we need to apply it to the shader    
		if (renderer.Material != mat) {
			mat = renderer.Material;
			mat->Apply();
		}
		// We'll need some info about the entities position in the world 
		const TempTransform& transform = ecs.get_or_assign<TempTransform>(entity);
		// Our normal matrix is the inverse-transpose of our object's world rotation
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform.GetWorldTransform())));
		// Update the MVP using the item's transform  
	
		//
		//if (hello == 0) { //The lantern
		////	mat->GetShader()->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() * dresserTransform[6 - hello]);
		//	mat->GetShader()->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() /*transform.GetWorldTransform())*/ * myLanternTransform);
		//
		//}
		////else if (hello == 1) {
		////
		////	mat->GetShader()->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() /*transform.GetWorldTransform())*/ * dresserTransform[hello]);
		////}
		//else {
		//	mat->GetShader()->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() * transform.GetWorldTransform());
		//}


		mat->GetShader()->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() /*transform.GetWorldTransform())*/* genTransform[hello]);

		hello++;

		// Update the model matrix to the item's world transform
		mat->GetShader()->SetUniform("a_Model", transform.GetWorldTransform());
		// Update the model matrix to the item's world transform 
		mat->GetShader()->SetUniform("a_NormalMatrix", normalMatrix);
		// Draw the item   
		renderer.Mesh->Draw();
	}


	//ecs.sort<MeshRenderer>([](const MeshRenderer& lhs, const MeshRenderer& rhs) {
	
	hello = 0;


	
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
	if (playerPos.x - objectPos.x < 3 && playerPos.x - objectPos.x > 3) {
		return true;
	}
	return false;
}