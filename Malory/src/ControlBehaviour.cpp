#include "ControlBehaviour.h"
#include "florp/app/Window.h"
#include "florp/app/Application.h"
#include "florp/game/Transform.h"
#include "florp/game/SceneManager.h"
#include "florp/app/Timing.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/wrap.hpp>

bool interactionIsPossible(glm::vec3 playerPos, glm::vec3 objectPos)
{
	float distanceX = abs((playerPos.x) - (objectPos.x));

	float distanceY = abs((playerPos.y) - (objectPos.y));
	if (distanceX < 10 && distanceY < 10) {
		return true;
	}
	return false;
}

template <typename T>
T wrap(const T& value, const T& min, const T& max) {
	//(((x - x_min) % (x_max - x_min)) + (x_max - x_min)) % (x_max - x_min) + x_min;
	T range = max - min;
	return glm::mod(glm::mod(value - min, range) + range, range )+ min;
}

static glm::vec3 cameraPos = {1, 1, 10};
std::vector <int> keys; //Use 0 for locked doors and 1 for unlocked


void ControlBehaviour::Update(entt::entity entity) {
	using namespace florp::app;
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	Window::Sptr window = Application::Get()->GetWindow();

	if (transform.GetLocalPosition() != cameraPos) {
		transform.SetPosition(cameraPos);
	}
	else {
		glm::vec3 movement = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);

		// Rotate and move our camera based on input
		float speed = 30.0f;
		float speed2 = 15.0f;
		float rotSpeed = 1.0f;
		bool isheadBob = false;
		//myCamera->LookAt(cameraViewTarget, cameraViewAngle);

		if (GetKeyState(0x53) & 0x8000) { //S key
			movement.z = -speed2 * Timing::DeltaTime;
			isheadBob = true;
		}
		else if (GetKeyState(0x57) & 0x8000) {
			movement.z = speed2 * Timing::DeltaTime;
			isheadBob = true;
		}
		else {
			movement.z = 0 * Timing::DeltaTime;
		}
		if (GetKeyState(0x44) & 0x8000) {
			movement.x = -speed2 * Timing::DeltaTime;
			isheadBob = true;
		}
		else if (GetKeyState(0x41) & 0x8000) {
			movement.x = speed2 * Timing::DeltaTime;
			isheadBob = true;
		}
		else {
			movement.x = 0;
		}

		if (GetKeyState(0x28) & 0x8000) {
			angleForZ -= 0.01;
			if (angleForZ < -1.4) {
				angleForZ = -1.4;
			}
		}
		if (GetKeyState(0x26) & 0x8000) {
			angleForZ += 0.01;
			if (angleForZ > 1.4) {
				angleForZ = 1.4;
			}
		}
		if (GetKeyState(0x27) & 0x8000) {
			angleForY += 0.01;
			angleForX -= 0.01;
		}
		if (GetKeyState(0x25) & 0x8000) {
			angleForY -= 0.01;
			angleForX += 0.01;
		}


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


		transform.LookAt({ transform.GetLocalPosition().x + cos(-angleForX), transform.GetLocalPosition().y + sin(-angleForY), transform.GetLocalPosition().z + tan(angleForZ) }, glm::vec3(0, 0, 1));

		if (glm::length(movement) > 0) {
			if (!HitBoxes::GetInstance().isInHitBox(CurrentRegistry().get<florp::game::Transform>(entity).GetLocalPosition(),
				{
				transform.GetLocalPosition().x + movement.z * cos(angleForX) + movement.x * cos(angleForX + 1.57078145) + finalHeadBobSide * cos(angleForX + 1.57078145),
				transform.GetLocalPosition().y + movement.z * sin(angleForX) + movement.x * sin(angleForX + 1.57078145) + finalHeadBobSide * sin(angleForX + 1.57078145),
				transform.GetLocalPosition().z + finalHeadBob
				})) {



				transform.SetPosition({
					transform.GetLocalPosition().x + movement.z * cos(angleForX) + movement.x * cos(angleForX + 1.57078145) + finalHeadBobSide * cos(angleForX + 1.57078145),
					transform.GetLocalPosition().y + movement.z * sin(angleForX) + movement.x * sin(angleForX + 1.57078145) + finalHeadBobSide * sin(angleForX + 1.57078145),
					transform.GetLocalPosition().z + finalHeadBob });
				cameraPos = transform.GetLocalPosition();
			}
		}
	}
}

void InputBehaviour::Update(entt::entity entity)
{
	using namespace florp::app;

	//auto& transform = florp::game::SceneManager::Current()->Get<florp::game::Transform>(entity);
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	
	
	//Window::Sptr window = Application::Get()->GetWindow();
	
	glm::vec3 translate = glm::vec3(0.0f);
	
	if (GetKeyState(0x25) & 0x8000)
		translate.x -= 10.0f;
	if (GetKeyState(0x27) & 0x8000)
		translate.x += 10.0f;
	if (GetKeyState(0x28) & 0x8000)
		translate.z -= 10.0f;
	if (GetKeyState(0x26) & 0x8000)
		translate.z += 10.0f;
	
	translate *= Timing::DeltaTime * mySpeed;
	if (glm::length(translate) > 0) {
		
		translate += transform.GetLocalPosition();
		transform.SetPosition(translate);
	}
}

void Hit::Update(entt::entity entity)
{
	static std::vector <glm::vec3> box;
	if (positionSaved != glm::vec3(-1, -10000000000000, -1)) {
		box.push_back(positionSaved);
		positionSaved = glm::vec3(-1, -10000000000000, -1);
	}
	//do nothing
	for (int i = box.size() - 1; i > 0; i--) {
		//Do hitBoxing
	}
}

void Car::Update(entt::entity entity)
{
	using namespace florp::app;
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	Window::Sptr window = Application::Get()->GetWindow();

	glm::vec3 translate = glm::vec3(1.0f, 0.0f, 0.0f);

	


	translate *= Timing::DeltaTime * mySpeed;
	timer--;
	if (timer < 0) {
		timer = 100;
		mySpeed *= -1;
	}

	if (glm::length(translate) > 0) {

		translate += transform.GetLocalPosition();
		transform.SetPosition(translate);
	}
}

void doorOpening::Update(entt::entity entity)
{
	
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	glm::tquat temp = glm::mat4(1.0f);
	temp = transform.GetLocalTransform();

	if (interacted == 2) {
		;
	}
	else if (GetKeyState(0x45) & 0x8000 && interacted == 0) {
		if (HitBoxes::GetInstance().getFloor() == floorObjectIsOn) {
			if (interactionIsPossible(cameraPos, transform.GetLocalPosition())) {
				interacted = 1;
			}
		}
	}
	else if (interacted = 1) {
		if (transform.GetLocalTransform()[0].y >= 0.99) { //Is interaction is done
			interacted = 2; 
		}
		else {
			temp = glm::rotate(temp, PI, glm::vec3(0, 0, 0.3));
			transform.SetRotation(temp);
		}
	}
}

void lockedDoor::Update(entt::entity entity)
{
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	glm::tquat temp = glm::mat4(1.0f);
	temp = transform.GetLocalTransform();

	if (interacted == 2) {
		;
	}
	else if (GetKeyState(0x45) & 0x8000 && interacted == 0) {
		if (keys[doorID] == 1) { //Door is unlocked
			if (HitBoxes::GetInstance().getFloor() == floorObjectIsOn) {
				if (interactionIsPossible(cameraPos, transform.GetLocalPosition())) {
					interacted = 1;
				}
			}
		}
	}
	else if (interacted = 1) {
		if (transform.GetLocalTransform()[0].y >= 0.99) { //Is interaction is done
			interacted = 2;
		}
		else {
			temp = glm::rotate(temp, PI, glm::vec3(0, 0, 0.3));
			transform.SetRotation(temp);
		}
	}
}

key::key(int floor, int keyNum) : IBehaviour(), interacted(0), floorObjectIsOn(floor), keyID(keyNum)
{
	keys.push_back(0);
}

void key::Update(entt::entity entity)
{

	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	glm::tquat temp = glm::mat4(1.0f);
	temp = transform.GetLocalTransform();

	if (interacted == 2) {
		;
	}
	else if (GetKeyState(0x45) & 0x8000 && interacted == 0) {
		if (HitBoxes::GetInstance().getFloor() == floorObjectIsOn) {
			if (interactionIsPossible(cameraPos, transform.GetLocalPosition())) {
				interacted = 1;
			}
		}
	}
	else if (interacted = 1) {
		transform.SetPosition(glm::vec3(100000000, 1000000, 1000000));
		interacted = 0;
		keys[keyID] = 1;
	}
}

void stairs1::Update(entt::entity entity)
{
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	glm::tquat temp = glm::mat4(1.0f);
	temp = transform.GetLocalTransform();

	if (GetKeyState(0x45) & 0x8000) { 
		if (HitBoxes::GetInstance().getFloor() == floorObjectIsOn) {
			if (interacted == 0) { //If 0 then stairs are leading to second floor
				if (interactionIsPossible(cameraPos, glm::vec3(transform.GetLocalPosition().x + 7, transform.GetLocalPosition().y, transform.GetLocalPosition().z))) {
					interacted = 1;
				}
			}
			else if (interacted == 2) { //If 2 then stairs are leading to first floor
				if (interactionIsPossible(cameraPos, glm::vec3(transform.GetLocalPosition().x - 7, transform.GetLocalPosition().y, transform.GetLocalPosition().z))) {
					interacted = 3;
				}
			}
		}
	}
	else if (interacted = 1) { //Do the interaction of going up the stairs
		HitBoxes::GetInstance().saveFloor(1);
		cameraPos = glm::vec3(33.0, -17.0, cameraPos.z + 50);
		interacted = 2;
	}
	else if (interacted = 3) { //Do the interaction of going down the stairs
		HitBoxes::GetInstance().saveFloor(0);
		cameraPos = glm::vec3(3.21, -7.373, cameraPos.z - 50);
		interacted = 0;
	}

}

void stairs2::Update(entt::entity entity)
{
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	glm::tquat temp = glm::mat4(1.0f);
	temp = transform.GetLocalTransform();

	if (GetKeyState(0x45) & 0x8000) {
		if (HitBoxes::GetInstance().getFloor() == floorObjectIsOn) {
			if (interacted == 0) { //If 0 then stairs are leading to second floor
				if (interactionIsPossible(cameraPos, glm::vec3(transform.GetLocalPosition().x, transform.GetLocalPosition().y + 20, transform.GetLocalPosition().z))) {
					interacted = 1;
				}
			}
			else if (interacted == 2) { //If 2 then stairs are leading to first floor
				if (interactionIsPossible(cameraPos, glm::vec3(transform.GetLocalPosition().x, transform.GetLocalPosition().y + 20, transform.GetLocalPosition().z))) {
					interacted = 3;
				}
			}
		}
	}
	else if (interacted = 1) { //Do the interaction of going up the stairs
		HitBoxes::GetInstance().saveFloor(1);
		cameraPos = glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z + 50);
		interacted = 2;
	}
	else if (interacted = 3) { //Do the interaction of going down the stairs
		HitBoxes::GetInstance().saveFloor(0);
		cameraPos = glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z - 50);
		interacted = 0;
	}
}

/*
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

*/


/*
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
*/