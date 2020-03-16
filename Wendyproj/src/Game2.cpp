#pragma once
#include "Game.h"

//To make it so that the game.cpp is not too long, I put the loading of objects and the update function into it's own file as those are the two
//functions that are the most modified (Should be re-organised properly later by logical groupment like graphics stuff, math stuff or other similar ideas)


void Game::LoadSimpleContent()
{
	// Create our 4 vertices
	Vertexmini vertices[4] = {
		//       Position                   Color
		//    x      y     z         r    g     b     a
		{{ -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
		{{  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f }},
		{{ -0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 1.0f, 1.0f }},
		{{  0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }},
	};

	// Create our 6 indices
	uint32_t indices[6] = {
		0, 1, 2,
		2, 1, 3
	};

	// Create a new mesh from the data
	mySimpleMesh = std::make_shared<Meshmini>(vertices, 4, indices, 6);

	// Create and compile shader
	simpleShader = std::make_shared<Shader>();
	simpleShader->LoadPart(ShaderStageType::VertexShader, "Shaders/passthrough.vs.glsl");
	simpleShader->LoadPart(ShaderStageType::FragmentShader, "Shaders/passthrough.fs.glsl");
	simpleShader->Link();
}

void Game::LoadContent() {
	myCamera = std::make_shared<Camera>();
	myCamera->SetPosition(cameraPos);
	myCamera->LookAt(cameraViewTarget, cameraViewAngle);
	myCamera->Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 1000.0f);
	
	interactCamera = std::make_shared<Camera>();
	interactCamera->SetPosition(glm::vec3(1, 1, 10));
	interactCamera->LookAt(glm::vec3(0.0f, 0.0f, 4), glm::vec3(0, 0, 1));
	interactCamera->Projection = glm::ortho(-22.0f, 22.0f, -10.0f, 10.0f, 0.0f, 1000.0f);

	glfwGetWindowSize(myWindow, &windowSizeWidth, &windowSizeHeight);
	GlfwWindowResizedCallback(myWindow, windowSizeWidth, windowSizeHeight);



	//Texture2D::Sptr albedo = Texture2D::LoadFromFile("f_Door.png");
	Texture2D::Sptr albedo2 = Texture2D::LoadFromFile("textures/F_fatWall.png");

	

	//std::vector <Vertex> temp;
	//Replaced with
	MeshData MeshTemp = ObjLoader::LoadObj("objects/f_Door.obj", glm::vec4(1.0f));

	
	lanternFuelFullPowerBuffer = 1000; //How long before the light starts to go away

	objectIDToHaveHitBox = { 3 }; //Put the list of objects you want to have hitboxes in here
	//Type 0 is nothing, type 1 is unlocked door, type 2 is locked door, type 3 is drawer, type 4 is a note, type 5 is the oil cask (refueling)
	///////////////Load objects and textures under here
	Game::CreateObjects(1, 0, 14); // object 0 floor 1
	Game::CreateObjects(11, 3, 13, 1); // object 1 is door topside of saferoom
	Game::CreateObjects(8, 3, 12); // object 2 is door right side in safe room
	Game::CreateObjects(11, 3, 13); // object 3 is left bathroom door
	Game::CreateObjects(8, 3, 12); // object 4 is top door across of saferoom
	Game::CreateObjects(11, 3, 13); // object 5 is ?
	Game::CreateObjects(16, 0, 5); // object 6 is the dresser in the batrhoom(?)
	Game::CreateObjects(15, 10, 4); // object 7 is the top drawer for object 6
	Game::CreateObjects(15, 0, 4); // object 8 is the middle drawer for object 6
	Game::CreateObjects(15, 0, 4); // object 9 is the bottom drawer for object 6
	Game::CreateObjects(28, 0, 7); // object 10 is the boarded up window in the safe room (left wall)
	Game::CreateObjects(47, 0, 21); // object 11 is a second boarded up window
	Game::CreateObjects(4, 5, 18); // object 12 is a big vase
	Game::CreateObjects(4, 0, 18); // object 13 is a big vase
	Game::CreateObjects(29, 0, 19); // object 14 is a full bookshelf
	Game::CreateObjects(29, 0, 19); //Object 15 is a full bookshelf, look at all those vertices
	Game::CreateObjects(11, 3, 13); //Object 16 is the study door
	Game::CreateObjects(11, 3, 13); //Object 17 is the right door of the dining room
	Game::CreateObjects(11, 3, 13); //Object 18 is the left door of the dining room
	Game::CreateObjects(11, 3, 13); //Object 19 is the door to the pantry
	Game::CreateObjects(33, 4, 22); //Object 20 is the front door
	Game::CreateObjects(34, 20, 20); //Object 21 is the safe room staircase
	Game::CreateObjects(35, 22, 20); //Object 22 is the kitchen staircase
	Game::CreateObjects(36, 0, 16); //Object 23 is the piano
	Game::CreateObjects(37, 0, 23); //Object 24 is the 1st floor toilet
	Game::CreateObjects(41, 0, 17); //Object 25 is the bathroom sink
	Game::CreateObjects(42, 0, 6); //Object 26 is the mirror above the sink
	Game::CreateObjects(11, 3, 13); //Object 27 is the door to the basement
	Game::CreateObjects(39, 0, 7); //Object 28 is the oil cask
	Game::CreateObjects(43, 0, 24); //Object 29 is the fireplace
	Game::CreateObjects(44, 0, 10); //Object 30 is the kitchen table
	Game::CreateObjects(45, 0, 11); //Object 31 is the kitchen cupboards
	Game::CreateObjects(2, 0, 5); //Object 32 is the second floor
	Game::CreateObjects(3, 0, 5); //Object 33 is the second floor bed
	Game::CreateObjects(19, 0, 26); //Object 34 is the bathroom key
	Game::CreateObjects(20, 0, 26); //Object 35 is the study key
	Game::CreateObjects(46, 0, 15); //Object 36 is the left window in the top hallway
	Game::CreateObjects(46, 0, 15); //Object 37 is the right window in the top hallway
	Game::CreateObjects(28, 0, 15); //Object 38 is the right window in the right hallway
	Game::CreateObjects(25, 0, 21); //Object 39 is the window in the kitchen stairwell
	Game::CreateObjects(47, 0, 21); //Object 40 is the fireplace window
	Game::CreateObjects(23, 0, 27); //Object 41 is the portrait across from the first hallway window
	Game::CreateObjects(23, 0, 28); //Object 42 is the portrait across from the first hallway window
	Game::CreateObjects(48, 0, 25); //Object 42 is the portrait across from the first hallway window

	//int hello = 0;
	Shader::Sptr phong = std::make_shared<Shader>();
	phong->Load("Shaders/lighting.vs.glsl", "Shaders/blinn-phong.fs.glsl");

	//Shader::Sptr phong2 = std::make_shared<Shader>();
	//phong2->Load("lighting.vs.glsl", "blinn-phong.fs.glsl");

	//vec3  Pos;
	//vec3  Color;
	//float Attenuation;
	
	testMat2 = std::make_shared<Material>(phong);
	testMat2->Set("s_Albedo", albedo2);
	//testMat2->Set("a_LightPos", { 0, 1, 10 });
	testMat2->Set("a_LightPos", { 1.0f, 1.0f, 0.0f });
	testMat2->Set("a_LightColor", { 1.0f, 0.0f, 1.0f });  
	testMat2->Set("a_LightAttenuation", 1.0f / 100.0f);
	//testMat2->Set("a_Lights[1].Pos", { 1.0f, 1.0f, 0.0f });
	//testMat2->Set("a_Lights[1].Color", { 1.0f, 1.0f, 1.0f });
	//testMat2->Set("a_Lights[1].Attenuation", 1.0f);

	//testMat2->Set("a_LightColor", { 1.0f, 1.0f, 0.0f });
	testMat2->Set("a_AmbientColor", { 1.0f, 1.0f, 1.0f });
	testMat2->Set("a_AmbientPower", 1.2f);
	testMat2->Set("a_LightSpecPower", 1.0f);
	testMat2->Set("a_EnabledLights", 2);

	SceneManager::RegisterScene("Test2");
	SceneManager::SetCurrentScene("Test2");

	{

		auto& ecs2 = GetRegistry("Test2");

		std::vector <entt::entity> genEntt;

		//for (int ix = 0; ix < 6; ix++) {
		//	entt::entity entity = ecs2.create();
		//	PointLight& light = ecs2.assign<PointLight&>(entity);
		//	light.Color = glm::vec3(glm::sin(-ix * ix) + 1.0f, glm::cos(-ix) + 1.0f, glm::sin((-ix) + glm::pi<float>()) + 1.0f) / 2.0f * 0.7f;
		//	light.Attenuation = 1.0f / 40.0f;
		//	ecs2.assign<TempTransform>(entity).Position = glm::vec3(glm::cos(ix) * 20.0f, 2.0f, glm::sin(ix) * 20.0f);
		//}

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
			case 0: //Object 0 The first floor
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(44.2f, 49.9f, 0.0f));
				break;
			case 1: //Object 1 Door right saferoom
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(50.5f, 11.5f, 0.0f));
				break;
			case 2:// object 2 is door bottom of saferoom
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(3.85f, -11.1f, 0.0f));
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
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(-11.85f, 16.5f, 117.5f));
				break;
			case 11:// object 11 is a broken window in the stairwell
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(-10.0f, -17.5f, 6.5f));
				break;
			case 12:// object 12 is a big vase in the safe room
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(-5.85f, 1.75f, 0.0f));
				break;
			case 13:// object 13 is a big vase in the safe room
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(-4.00f, -3.25f, 0.0f));
				break;
			case 14:// object 14 is the bookshelf with way, way too many faces
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(24.0f, -6.55f, 0.0f));
				break;
			case 15:// object 15 is the bookshelf in the study
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(122.0f, -38.8f, 0.0f));
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
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(121.0f, -11.0f, 0.0f));
				break;

			case 24:// object 24 is the 1st floor toilet
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(96.55f, -9.0f, -0.1f));
				break;

			case 25:// object 25 is the bathroom sink
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(90.55f, 24.0f, 0.0f));
				break;

			case 26:// object 26 is the mirror above the sink
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(90.55f, 26.0f, 700.0f));
				break;

			case 27:// object 27 is the basement door
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(27.55f, 68.0f, 0.0f));
				break;

			case 28:// object 28 is the oil cask
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(49.0f, 29.0f, 1.0f));
				break;

			case 29:// object 29 is the fireplace
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(-8.0f, 20.0f, -0.0f));
				break;

			case 30:// object 30 is the kitchen table
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(135.0f, 45.0f, 0.0f));
				break;

			case 31:// object 31 is the kitchen cupboards
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(161.5f, 56.0f, 0.0f));
				break;

			case 32:// object 32 is the 2nd floor
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(44.2f, 49.9f, -32.0f));
				break;

			case 33:// object 33 is the 2nd floor bed
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(-4.0f, 19.0f, 48.0f));
				break;

			case 34:// object 34 is the bathroom key
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(90.55f, 24.0f, 0.0f));
				break;

			case 35:// object 35 is the piano key
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(118.0f, -25.0f, 0.0f));
				break;

			case 36:// object 36 is the left window in the top hallway
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(105.0f, 94.5f, 8.0f));
				break;

			case 37:// object 37 is the right window in the top hallway
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(137.0f, 94.5f, 8.0f));
				break;

			case 38:// object 38 is the window in the right hallway
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(198.26f, 60.0f, 8.0f));
				break;

			case 39:// object 39 is the window in the kitchen stairwell
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(198.26f, 12.0f, 6.0f));
				break;

			case 40:// object 40 is the window on the fireplace wall
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(-10.0f, 0.0f, 9.0f));
				break;

			case 41:// object 41 is the portrait across from left window in the top hallway
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(97.5f, 79.35f, 14.5f));
				break;

			case 42:// object 42 is the portrait past the first one
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(117.5f, 79.35f, 14.5f));
				break;

			case 43:// object 43 is the ceiling
				genTransform[i] = glm::translate(genTransform[i], glm::vec3(44.2f, 49.9f, -6.0f));
				break;
			}
		}
	}
}

void Game::Update(float deltaTime) {
	hitBoxManager.saveFloor(currentFloor); 

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

	myCamera->LookAt({ myCamera->GetPosition().x + cos(-angleForX), myCamera->GetPosition().y + sin(-angleForY), myCamera->GetPosition().z + tan(angleForZ) }, cameraViewAngle);



	if (!hitBoxManager.isInHitBox(myCamera->GetPosition(), { myCamera->GetPosition().x + movement.z * cos(angleForX) + movement.x * cos(angleForX + 1.57078145) + finalHeadBobSide * cos(angleForX + 1.57078145),
		myCamera->GetPosition().y + movement.z * sin(angleForX) + movement.x * sin(angleForX + 1.57078145) + finalHeadBobSide * sin(angleForX + 1.57078145),
		myCamera->GetPosition().z + finalHeadBob })) {
		
		myCamera->SetPosition({
		myCamera->GetPosition().x + movement.z * cos(angleForX) + movement.x * cos(angleForX + 1.57078145) + finalHeadBobSide * cos(angleForX + 1.57078145),
		myCamera->GetPosition().y + movement.z * sin(angleForX) + movement.x * sin(angleForX + 1.57078145) + finalHeadBobSide * sin(angleForX + 1.57078145),
		myCamera->GetPosition().z + finalHeadBob });

	}
	


	//myCamera->Move(movement);

	glm::mat4 temp = myCamera->GetView();

	cameraPos = myCamera->GetPosition();

	//myLanternTransform = glm::translate(myLanternTransform, glm::vec3(cameraPos + glm::vec3(-6, -2, 0)));
	float extraDist = 2;
	float offSet = 0.5;

	testMat2->Set("a_LightPos", { (myCamera->GetPosition().x + cos(-angleForX) + extraDist * cos(angleForX - offSet)) - 40.0f, //+ offSet * cos(angleForX), 
									  (myCamera->GetPosition().y + sin(-angleForY) + extraDist * sin(-angleForY - offSet)) - 40.0f, //+ offSet * sin(-angleForY),
									  myCamera->GetPosition().z + (angleForZ > -1 ? (angleForZ < 1 ? tan(angleForZ) : 1) : -1) });

	if (glfwGetKey(myWindow, GLFW_KEY_R) == GLFW_PRESS) {
		lanternFuel = LANTERNFUELDEFAULT; //How long will the lantern last
		lightAttenuationModifyer = 1.0f;
		lightShyninessModifyer = 1.0f;
	}


	//Lantern degrade numbers here
	if (lanternFuel > lanternFuelFullPowerBuffer) {
		deathTimer = DEATHTIMERMAX;
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
		if (!(cameraPos.x > -11.7 && cameraPos.x < 52.5 && cameraPos.y > -8.15 && cameraPos.y < 35.2 && currentFloor == 0)) { //Safe room location
			deathTimer--;
		}
		else if ((cameraPos.x > -11.7 && cameraPos.x < 52.5 && cameraPos.y > -8.15 && cameraPos.y < 35.2 && currentFloor == 0)) { //Safe room location
			deathTimer = DEATHTIMERMAX;
		}
	}


	if (deathTimer == 0) {
		//When shading is working add a death screen and delay before respawn here

		currentFloor = 0;
		myCamera->SetPosition({ 1, 1, 10 });
	}

	lanternFuel -= 1; //Just makes the lantern fuel drain

	static bool isE = false;
	if (glfwGetKey(myWindow, GLFW_KEY_E) == GLFW_PRESS) {
		if (!isE) { //Interactables work like this -> if e was pressed and the object to interact with is in range, add it to the list of object that need to update
					//Add any special things to the objects (like doors get converted to an ID of 2 instead of 3 because after opening it will no longer be interactable)
					//Interact buffer is there to make sure only one object is interacted at a time (prevents trying to open a drawer and then the key is picked up)
			isE = true;
			for (int i = 0; i < genObjects.size(); i++) {
				if (objFloor[i] == currentFloor) { //Only interact with what is on your floor
					if (amountOfObjects[i] == 3 && interactBuffer == true) { //3 is for Doors
						if (interactionIsPossible(cameraPos, glm::vec3(genTransform[i][3].x, genTransform[i][3].y, genTransform[i][3].z))) {
							objectsToUpdate.push_back(i);
							amountOfObjects[i]--;
							interactBuffer = false;
						}
					}
					else if (amountOfObjects[i] == 10 && interactBuffer == true) { //10 is for Drawer
						if (interactionIsPossible(cameraPos, glm::vec3(genTransform[i][3].x, genTransform[i][3].y, genTransform[i][3].z))) {
							objectsToUpdate.push_back(i);
							amountOfObjects[i]--;
							interactBuffer = false;
						}
					}
					else if (amountOfObjects[i] == 5 && interactBuffer == true) { //5 is for keys
						if (interactionIsPossible(cameraPos, glm::vec3(genTransform[i][3].x, genTransform[i][3].y, genTransform[i][3].z))) {
							objectsToUpdate.push_back(i);
							interactBuffer = false;
						}
					}
					else if (amountOfObjects[i] == 19 && interactBuffer == true) { //20 is for stairs going down
						if (interactionIsPossible(cameraPos, glm::vec3(genTransform[i][3].x + 7, genTransform[i][3].y, genTransform[i][3].z))) {
							objectsToUpdate.push_back(i);
							amountOfObjects[i]++; //It's 20 because here it's getting incremented by one
							interactBuffer = false;
						}
					}
					else if (amountOfObjects[i] == 20 && interactBuffer == true) { //19 is for stairs going up
						if (interactionIsPossible(cameraPos, glm::vec3(genTransform[i][3].x - 7, genTransform[i][3].y, genTransform[i][3].z))) {
							objectsToUpdate.push_back(i);
							amountOfObjects[i]--; //It's 19 because here it's getting incremented by one
							interactBuffer = false;
						}
					} //192.0f, -0.9f, 0.0f
					else if (amountOfObjects[i] == 21 && interactBuffer == true) { //22 is for stairs going down
						if (interactionIsPossible(cameraPos, glm::vec3(genTransform[i][3].x, genTransform[i][3].y + 20, genTransform[i][3].z))) {
							objectsToUpdate.push_back(i);
							amountOfObjects[i]++; //It's 22 because here it's getting incremented by one
							interactBuffer = false;
						}
					}
					else if (amountOfObjects[i] == 22 && interactBuffer == true) { //21 is for stairs going up
						if (interactionIsPossible(cameraPos, glm::vec3(genTransform[i][3].x, genTransform[i][3].y + 20, genTransform[i][3].z))) {
							objectsToUpdate.push_back(i);
							amountOfObjects[i]--; //It's 21 because here it's getting incremented by one
							interactBuffer = false;
						}
					}
				}
			}
		}
	}
	else {
		isE = false;
	}


	if (objectsToUpdate.size() != 0) { //Now it calls the update for objects that need that special attention ;)
		for (int i = 0; i < objectsToUpdate.size(); i++) {
			objectUpdate(i);
		}
	}
	if (interactBuffer == false) { //Keep this one last because yes
		if (objectsToUpdate.size() == 0) {
			interactBuffer = true;
		}
	}

}

int Game::objectUpdate(int ID) //Why did I put this as an int?
{

	float time = 0.001;

	switch (amountOfObjects[objectsToUpdate[ID]]) {
	case 1: //When the door is done opening
		objectsToUpdate.clear();//We want to clear whenever the update is done to free up space for the next object
		break;
	case 2: //Door opening Really needs tweaking LIKE REALLY NEEDS FIXING!!!!
		if (genTransform[objectsToUpdate[ID]][0].y >= 0.99) {
			amountOfObjects[objectsToUpdate[ID]] = 1;
			return 1;
		}
		else {
			genTransform[objectsToUpdate[ID]] = glm::rotate(genTransform[objectsToUpdate[ID]], time, glm::vec3(0, 0, 0.3));
			genTransform[objectsToUpdate[ID]] = glm::translate(genTransform[objectsToUpdate[ID]], glm::vec3(-acos(genTransform[objectsToUpdate[ID]][0].x) / 500, -asin(genTransform[objectsToUpdate[ID]][1].y) / 500, 0));
		}
		return 0;
	case 5: //Red Key //Is that a thing?
		if (genTransform[objectsToUpdate[ID]][3].y <= 100099) {
			genTransform[objectsToUpdate[ID]] = glm::translate(genTransform[objectsToUpdate[ID]], glm::vec3(1000000000, 10000000000, 1000000000));
			for (int i = 0; i < amountOfObjects.size(); i++) {
				if (amountOfObjects[i] == 4) { //Unlock the doors that are associated with this key
					amountOfObjects[i] = 3;
				}
			}

			return 0;
		}
		else {
			objectsToUpdate.clear();
			return 1;
		}
	case 10: //Drawer //Just guessing because I have no idea where it is
		if (time < 10) {
			genTransform[objectsToUpdate[ID]] = glm::translate(genTransform[objectsToUpdate[ID]], glm::vec3(0.5, 0.0, 0.0));
		}
		else {
			amountOfObjects[objectsToUpdate[ID]] = 1;
			objectsToUpdate.clear();
		}
		break;
	case 20:
		objFloor[objectsToUpdate[ID]] = 0;
		myCamera->SetPosition({ 3.21, -7.373, myCamera->GetPosition().z - 50}); //Do the stuff here idiot.
		currentFloor--;
		objectsToUpdate.clear();
		break;
	case 19:
		objFloor[objectsToUpdate[ID]] = 1;
		myCamera->SetPosition( { 33.0, -17.0, myCamera->GetPosition().z + 50 }); //Do the stuff here idiot. 33 -17
		currentFloor++;
		objectsToUpdate.clear();
		break;
	case 22:
		objFloor[objectsToUpdate[ID]] = 0;
		myCamera->SetPosition({ myCamera->GetPosition().x, myCamera->GetPosition().y, myCamera->GetPosition().z - 50 }); //Do the stuff here idiot.
		currentFloor--;
		objectsToUpdate.clear();
		break;
	case 21:
		objFloor[objectsToUpdate[ID]] = 1;
		myCamera->SetPosition({ myCamera->GetPosition().x, myCamera->GetPosition().y, myCamera->GetPosition().z + 50 }); //Do the stuff here idiot. 33 -17
		currentFloor++;
		objectsToUpdate.clear();
		break;

	}


	return 0;

}