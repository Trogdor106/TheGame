#pragma once
#include "Game.h"

//To make it so that the game.cpp is not too long, I put the loading of objects and the update function into it's own file as those are the two
//functions that are the most modified (Should be re-organised properly later by logical groupment like graphics stuff, math stuff or other similar ideas)


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
	Game::CreateObjects(11, 3, 2); // object 1 is door topside of saferoom
	Game::CreateObjects(8, 3, 2); // object 2 is door right side in safe room
	Game::CreateObjects(11, 3, 2); // object 3 is door bottomside of saferoom
	Game::CreateObjects(8, 3, 2); // object 4 is top door across of saferoom
	Game::CreateObjects(8, 3, 2); // object 5 is left bathroom door
	Game::CreateObjects(16, 0, 5); // object 6 is the dresser in the batrhoom(?)
	Game::CreateObjects(15, 10, 4); // object 7 is the top drawer for object 6
	Game::CreateObjects(15, 0, 4); // object 8 is the middle drawer for object 6
	Game::CreateObjects(15, 0, 4); // object 9 is the bottom drawer for object 6
	Game::CreateObjects(28, 0, 7); // object 10 is the boarded up window in the safe room (left wall)
	Game::CreateObjects(25, 0, 6); // object 11 is a second boarded up window
	Game::CreateObjects(4, 5, 6); // object 12 is a big vase
	Game::CreateObjects(4, 0, 6); // object 13 is a big vase
	Game::CreateObjects(29, 0, 6); // object 14 is a full bookshelf
	Game::CreateObjects(29, 0, 6); // object 15 is a full bookshelf, look at all those vertices
	Game::CreateObjects(11, 0, 6); // object 16 is the study door
	Game::CreateObjects(11, 3, 6); // object 17 is the right door of the dining room
	Game::CreateObjects(11, 3, 6); // object 18 is the left door of the dining room
	Game::CreateObjects(11, 3, 6); //Object 19 is the door to the pantry
	Game::CreateObjects(33, 4, 6); //Object 20 is the front door
	Game::CreateObjects(34, 0, 6); //Object 21 is the safe room staircase
	Game::CreateObjects(35, 0, 6); //Object 22 is the kitchen staircase
	Game::CreateObjects(36, 0, 6); //Object 23 is the piano
	Game::CreateObjects(37, 3, 6); //Object 24 is the 1st floor toilet
	Game::CreateObjects(41, 0, 6); //Object 25 is the bathroom sink
	Game::CreateObjects(42, 0, 6); //Object 25 is the mirror above the sink
	//Game::CreateObjects(8, 1, 2);
	//Game::CreateObjects(34, 4, 8);

	int hello = 0;
	Shader::Sptr phong = std::make_shared<Shader>();
	phong->Load("lighting.vs.glsl", "blinn-phong.fs.glsl");

	Shader::Sptr phong2 = std::make_shared<Shader>();
	phong2->Load("lighting.vs.glsl", "blinn-phong.fs.glsl");


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

	myCamera->LookAt({ myCamera->GetPosition().x + cos(-angleForX), myCamera->GetPosition().y + sin(-angleForY), myCamera->GetPosition().z + tan(angleForZ) }, cameraViewAngle);



	myCamera->SetPosition({
		myCamera->GetPosition().x + movement.z * cos(angleForX) + movement.x * cos(angleForX + 1.57078145) + finalHeadBobSide * cos(angleForX + 1.57078145),
		myCamera->GetPosition().y + movement.z * sin(angleForX) + movement.x * sin(angleForX + 1.57078145) + finalHeadBobSide * sin(angleForX + 1.57078145),
		myCamera->GetPosition().z + finalHeadBob });



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

	static bool isE = false;
	int hello = 0;
	if (glfwGetKey(myWindow, GLFW_KEY_E) == GLFW_PRESS) {
		if (!isE) {
			isE = true;
			for (int i = 0; i < genObjects.size(); i++) {
				if (amountOfObjects[i] == 3) {
					if (interactionIsPossible(cameraPos, glm::vec3(genTransform[i][3].x, genTransform[i][3].y, genTransform[i][3].z))) {
						objectsToUpdate.push_back(i);
						amountOfObjects[i]--;
					}
				}
				else if (amountOfObjects[i] == 5) {
					if (interactionIsPossible(cameraPos, glm::vec3(genTransform[i][3].x, genTransform[i][3].y, genTransform[i][3].z))) {
						objectsToUpdate.push_back(i);
					}
				}
				else if (amountOfObjects[i] == 10) {
					if (interactionIsPossible(cameraPos, glm::vec3(genTransform[i][3].x, genTransform[i][3].y, genTransform[i][3].z))) {
						objectsToUpdate.push_back(i);
					}
				}
			}
		}
	}
	else {
		isE = false;
	}


	if (objectsToUpdate.size() != 0) {
		for (int i = 0; i < objectsToUpdate.size(); i++) {
			objectUpdate(i);
		}
	}





}