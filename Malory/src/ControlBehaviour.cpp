#include "ControlBehaviour.h"
#include "florp/app/Window.h"
#include "florp/app/Application.h"
#include "florp/game/Transform.h"
#include "florp/game/SceneManager.h"
#include "florp/app/Timing.h"
#include "layers/AudioLayer.h"

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

static glm::vec3 cameraPos = { 1.0f, 1.0f, 10.0f };
std::vector <int> keys; //Use 0 for locked doors and 1 for unlocked
float angleForX = 0;
float angleForY = 0;
float angleForZ = 0;
glm::vec3 lookAtPoint;
const float LANTERN_FULL = 2000;
float lanternFuel = LANTERN_FULL;
float playerState = 1; //0 is title screen, 1 is normal, 2 is paused, 3 is dead
std::vector <int> timers;
std::vector <std::string> names;

void playSound(std::string sound, float duration, glm::vec3 position = { 0,0,0 }) {
	AudioEngine& audioEngine = AudioEngine::GetInstance();
	audioEngine.LoadEvent(sound);
	audioEngine.PlayEvent(sound);
	audioEngine.SetEventPosition(sound, position);
	names.push_back(sound);
	timers.push_back(duration);
}
void updateSounds() {
	AudioEngine& audioEngine = AudioEngine::GetInstance();

	for (int i = 0; i < names.size(); i++) {
		if (timers[i] == 0) {
			audioEngine.StopEvent(names[i], true);
		}
		else {
			timers[i]--;
		}
	}
}



void ControlBehaviour::Update(entt::entity entity) {
	using namespace florp::app;
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	Window::Sptr window = Application::Get()->GetWindow();

	if (transform.GetLocalPosition() != cameraPos) {
		transform.SetPosition(cameraPos);
	}
	else if (playerState == 1) { //Make sure the player is in the playing state before letting them move
		glm::vec3 movement = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);

		// Rotate and move our camera based on input
		float speed = 30.0f;
		float speed2 = 15.0f;
		float rotSpeed = 0.05f;
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
			angleForY += rotSpeed;
			angleForX -= rotSpeed;
		}
		if (GetKeyState(0x25) & 0x8000) {
			angleForY -= rotSpeed;
			angleForX += rotSpeed;
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
		
		//if (finalHeadBob == 0) { //Do your conditions to make a sound
		//	playSound("Monkey", 30);
		//}

		lookAtPoint = { transform.GetLocalPosition().x + cos(-angleForX), transform.GetLocalPosition().y + sin(-angleForY), transform.GetLocalPosition().z + tan(angleForZ) };
		transform.LookAt(lookAtPoint, glm::vec3(0, 0, 1));

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
	updateSounds();
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

void doorManDoors::Update(entt::entity entity) {
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	glm::tquat temp = glm::mat4(1.0f);
	temp = transform.GetLocalTransform();
	

	if (interacted == 2) {
		;
	}
	else if (GetKeyState(0x45) & 0x8000 && interacted == 0) {
		if (HitBoxes::GetInstance().getFloor() == floorCurrent) { //Update this if it's working
			if (interactionIsPossible(cameraPos, transform.GetLocalPosition())) {
				interacted = 1;
			}
		}
	}
	else if (interacted == 1) {
		if (transform.GetLocalTransform()[0].y >= 0.99) { //Is interaction is done
			interacted = 2;
			HitBoxes::GetInstance().updateHitBox(idOfDoor, transform.GetLocalTransform());
		}
		else {
			transform.Rotate(glm::vec3(0, 0, 0.3));
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
	else if (interacted == 1) {
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
	else if (interacted == 1) {
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
	else if (interacted == 1) { //Do the interaction of going up the stairs
		HitBoxes::GetInstance().saveFloor(1);
		cameraPos = glm::vec3(33.0, -17.0, cameraPos.z + 50);
		interacted = 2;
	}
	else if (interacted == 3) { //Do the interaction of going down the stairs
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
	else if (interacted == 1) { //Do the interaction of going up the stairs
	HitBoxes::GetInstance().saveFloor(1);
	cameraPos = glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z + 50);
	interacted = 2;
	}
	else if (interacted == 3) { //Do the interaction of going down the stairs
	HitBoxes::GetInstance().saveFloor(0);
	cameraPos = glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z - 50);
	interacted = 0;
	}
}

void lantern::Update(entt::entity entity)
{
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);

	transform.SetPosition(glm::vec3(cameraPos.x + cos(-angleForX) + 10 * cos(angleForX - 0.5),
		cameraPos.y + sin(-angleForY) + 10 * sin(-angleForY - 0.5),
		cameraPos.z + (angleForZ > -1 ? (angleForZ < 1 ? tan(angleForZ) : 1) : -1)));
	transform.LookAt({ lookAtPoint.x, -lookAtPoint.y, -lookAtPoint.z }, glm::vec3(0.0f, 0.0f, 1.0f));

	auto& ecs = CurrentRegistry();
	if (ecs.has<ShadowLight>(entity)) {
		if (lanternFuel > 0) {
			lanternFuel--;
		}
		int aNumber = 100;
		ShadowLight& light = ecs.get<ShadowLight>(entity);

		int percentage = 100 - ((LANTERN_FULL - lanternFuel) / LANTERN_FULL) * 100;
		if (percentage > 60)
			;
		else if (percentage <= 60 && percentage != 0) {
			light.Attenuation = 0.01 * ((100 - percentage) * (100 - percentage) * 0.1);
		}
		else {
			light.Attenuation = 1000000;
		}
	}
}


void death::Update(entt::entity entity)
{
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	if (lanternFuel == 0 && !((transform.GetLocalPosition().x > -11.7 && transform.GetLocalPosition().x < 52.5 &&
		transform.GetLocalPosition().y > -8.15 && transform.GetLocalPosition().y < 35.2 && HitBoxes::GetInstance().getFloor() == 0))) {
		playerState = 3;
	}
}

void aStarAlgorithm::Update(entt::entity entity)
{
	//How do we want to do this?
	//First we get the estimated amount of intervals (since each interval will move the object by 5 it's equal to the floor of the x and y components from the hypotenus/5 (distance)
	//We do the math for all the squares (each possible movement will have to be +5 movement and see how far it's from the goal (estimated))
	//Take the path that contains the smallest value
	//If more then one path is equal, then take the first one until it's value get's increased above the other ones, then try the next
	//Once found make it move to it's location

	static int stateOfAlgorithm = 0; //Waiting for initiation
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);


	if (GetKeyState(0x52) & 0x8000 && stateOfAlgorithm == 0 && ((cameraPos.x > -11.7 && cameraPos.x < 52.5 &&
		cameraPos.y > -8.15 && cameraPos.y < 35.2 && HitBoxes::GetInstance().getFloor() == 0))) {
		stateOfAlgorithm = 1; //Calculation phase
		//Find distance is goal - current.x+y+z ^ 2 sqrt
		estimatedStepsX = abs(cameraPos.x - transform.GetLocalPosition().x) / 5;
		estimatedStepsY = abs(cameraPos.y - transform.GetLocalPosition().y) / 5;

	}
	else if (stateOfAlgorithm == 1) {
		//			0			
		//
		//
		//3			p			1
		//
		//
		//			2			

		glm::vec3 positionTest = transform.GetLocalPosition();
		glm::vec3 endposition;
		if (cameraPos.x < transform.GetLocalPosition().x) {
			if (cameraPos.y < transform.GetLocalPosition().y) {
				endposition = glm::vec3(positionTest.x - 5 * estimatedStepsX, positionTest.y - 5 * estimatedStepsY, positionTest.z);
			}
			else {
				endposition = glm::vec3(positionTest.x - 5 * estimatedStepsX, positionTest.y + 5 * estimatedStepsY, positionTest.z);
			}
		}
		else {
			if (cameraPos.y < transform.GetLocalPosition().y) {
				endposition = glm::vec3(positionTest.x + 5 * estimatedStepsX, positionTest.y - 5 * estimatedStepsY, positionTest.z);
			}
			else {
				endposition = glm::vec3(positionTest.x + 5 * estimatedStepsX, positionTest.y + 5 * estimatedStepsY, positionTest.z);
			}
		}
		
		if (positionTest == endposition || HitBoxes::GetInstance().isInHitBox(endposition, endposition)) {
			stateOfAlgorithm = 0;
		}
		else {
			std::vector <glm::vec3> positionsReached;
			std::vector <int> toNotUse;

			while (positionTest != endposition) {
				int square1Value = 999;
				int square2Value = 999;
				int square3Value = 999;
				int square4Value = 999;
				onStep++;
				bool wasPositionedReached = false;
				//check the 4 and see which is closer
				//add lowest to a table
				if (!(HitBoxes::GetInstance().isInHitBox(cameraPos, glm::vec3(positionTest.x, positionTest.y + 5, positionTest.z)))) {
					wasPositionedReached = false;
					for (int i = 0; i < positionsReached.size(); i++) {
						if (glm::vec3(positionTest.x, positionTest.y + 5, positionTest.z) == positionsReached[i]) {
							wasPositionedReached = true;
							i = positionsReached.size(); //Can exit the loop since we got our answer
						}
					}

					if (wasPositionedReached == false) {
						square1Value = onStep + abs(cameraPos.x - positionTest.x) / 5 + abs(cameraPos.y - (positionTest.y + 5)) / 5;
					}
				}
				if (!(HitBoxes::GetInstance().isInHitBox(cameraPos, glm::vec3(positionTest.x + 5, positionTest.y, positionTest.z)))) {
					wasPositionedReached = false;
					for (int i = 0; i < positionsReached.size(); i++) {
						if (glm::vec3(positionTest.x + 5, positionTest.y, positionTest.z) == positionsReached[i]) {
							wasPositionedReached = true;
							i = positionsReached.size(); //Can exit the loop since we got our answer
						}
					}

					if (wasPositionedReached == false) {
						square2Value = onStep + abs(cameraPos.x - (positionTest.x + 5)) / 5 + abs(cameraPos.y - positionTest.y) / 5;
					}
				}
				if (!(HitBoxes::GetInstance().isInHitBox(cameraPos, glm::vec3(positionTest.x, positionTest.y - 5, positionTest.z)))) {
					wasPositionedReached = false;
					for (int i = 0; i < positionsReached.size(); i++) {
						if (glm::vec3(positionTest.x, positionTest.y - 5, positionTest.z) == positionsReached[i]) {
							wasPositionedReached = true;
							i = positionsReached.size(); //Can exit the loop since we got our answer
						}
					}

					if (wasPositionedReached == false) {
						square3Value = onStep + abs(cameraPos.x - positionTest.x) / 5 + abs(cameraPos.y - (positionTest.y - 5)) / 5;
					}
				}
				if (!(HitBoxes::GetInstance().isInHitBox(cameraPos, glm::vec3(positionTest.x - 5, positionTest.y, positionTest.z)))) {
					wasPositionedReached = false;
					for (int i = 0; i < positionsReached.size(); i++) {
						if (glm::vec3(positionTest.x - 5, positionTest.y, positionTest.z) == positionsReached[i]) {
							wasPositionedReached = true;
							i = positionsReached.size(); //Can exit the loop since we got our answer
						}
					}

					if (wasPositionedReached == false) {
						square4Value = onStep + abs(cameraPos.x - (positionTest.x - 5)) / 5 + abs(cameraPos.y - positionTest.y) / 5;
					}
				}

				if (square1Value == 999 && square2Value == 999 && square3Value == 999 && square4Value == 999) {

					if (directions[onStep - 1] == 0) {
						positionTest = glm::vec3(positionTest.x, positionTest.y - 5, positionTest.z);
					}
					else if (directions[onStep - 1] == 1) {
						positionTest = glm::vec3(positionTest.x - 5, positionTest.y, positionTest.z);
					}
					else if (directions[onStep - 1] == 2) {
						positionTest = glm::vec3(positionTest.x, positionTest.y + 5, positionTest.z);
					}
					else if (directions[onStep - 1] == 3) {
						positionTest = glm::vec3(positionTest.x + 5, positionTest.y, positionTest.z);
					}
					onStep--;
					std::vector <glm::vec3> temp;
					std::vector <int> temp2;
					for (int i = 0; i < positionsReached.size() - 1; i++) {
						temp.push_back(positionsReached[i]);
						temp2.push_back(directions[i]);
					}
					positionsReached.clear();
					positionsReached = temp;
					directions.clear();
					directions = temp2;
				}
				else {
					directions.push_back(square1Value < square2Value ? square1Value < square3Value ? square1Value < square4Value ? 0 : 3 : square3Value < square4Value ?
						2 : 3 : square2Value < square3Value ? square2Value < square4Value ? 1 : 3 : square3Value < square4Value ?
						2 : 3);
					if (directions[onStep - 1] == 0) {
						positionTest = glm::vec3(positionTest.x, positionTest.y + 5, positionTest.z);
					}
					else if (directions[onStep - 1] == 1) {
						positionTest = glm::vec3(positionTest.x + 5, positionTest.y, positionTest.z);
					}
					else if (directions[onStep - 1] == 2) {
						positionTest = glm::vec3(positionTest.x, positionTest.y - 5, positionTest.z);
					}
					else if (directions[onStep - 1] == 3) {
						positionTest = glm::vec3(positionTest.x - 5, positionTest.y, positionTest.z);
					}
					positionsReached.push_back(positionTest);
				}


			}

			stateOfAlgorithm = 2;
		}
	}
	else if (stateOfAlgorithm == 2) {
		if (!(delay == 0)) {
			delay--;
		}
		else {
			if (directions[index] == 0) {
				transform.SetPosition(glm::vec3(transform.GetLocalPosition().x, transform.GetLocalPosition().y + 5, transform.GetLocalPosition().z));
			}
			else if (directions[index] == 1) {
				transform.SetPosition(glm::vec3(transform.GetLocalPosition().x + 5, transform.GetLocalPosition().y, transform.GetLocalPosition().z));
			}
			else if (directions[index] == 2) {
				transform.SetPosition(glm::vec3(transform.GetLocalPosition().x, transform.GetLocalPosition().y - 5, transform.GetLocalPosition().z));
			}
			else if (directions[index] == 3) {
				transform.SetPosition(glm::vec3(transform.GetLocalPosition().x - 5, transform.GetLocalPosition().y, transform.GetLocalPosition().z));
			}
			delay = 20;
			index++;
			if (index >= directions.size()) {
				stateOfAlgorithm = 3;
			}

		}
	}
	else if (stateOfAlgorithm == 3) {
		if (GetKeyState(0x52) & 0x8000) {
			stateOfAlgorithm = 0;
			index = 0;
			directions.clear();
			onStep = 0;
		}
	}
}




void Listener::Update(entt::entity entity)
{
	using namespace florp::app;
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);

	AudioEngine& audioEngine = AudioEngine::GetInstance();


	// Set listener position
	audioEngine.SetListenerPosition(transform.GetLocalPosition());


	// Set listener orientation

	audioEngine.SetListenerOrientation(transform.GetUp(), transform.GetForward());

}

void Listener::OnLoad(entt::entity entity)
{
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
}

void BackgroundMusic::OnLoad(entt::entity entity)
{
	AudioEngine& audioEngine = AudioEngine::GetInstance();
	audioEngine.LoadEvent("Music");
	audioEngine.PlayEvent("Music");
}

void BackgroundMusic::Update(entt::entity entity)
{
	AudioEngine::GetInstance().SetEventPosition("Music", cameraPos);
}
