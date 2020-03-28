#include "ControlBehaviour.h"
#include "florp/app/Window.h"
#include "florp/app/Application.h"
#include "florp/game/Transform.h"
#include "florp/game/SceneManager.h"
#include "florp/app/Timing.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/wrap.hpp>

template <typename T>
T wrap(const T& value, const T& min, const T& max) {
	//(((x - x_min) % (x_max - x_min)) + (x_max - x_min)) % (x_max - x_min) + x_min;
	T range = max - min;
	return glm::mod(glm::mod(value - min, range) + range, range )+ min;
}

void ControlBehaviour::Update(entt::entity entity) {
	using namespace florp::app;
	auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
	Window::Sptr window = Application::Get()->GetWindow();

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

	
	transform.LookAt({ transform.GetLocalPosition().x + cos(-angleForX), transform.GetLocalPosition().y + sin(-angleForY), transform.GetLocalPosition().z + tan(angleForZ) }, glm::vec3(0,0,1));

	if (glm::length(movement) > 0) {
		transform.SetPosition({
			transform.GetLocalPosition().x + movement.z * cos(angleForX) + movement.x * cos(angleForX + 1.57078145) + finalHeadBobSide * cos(angleForX + 1.57078145),
			transform.GetLocalPosition().y + movement.z * sin(angleForX) + movement.x * sin(angleForX + 1.57078145) + finalHeadBobSide * sin(angleForX + 1.57078145),
			transform.GetLocalPosition().z + finalHeadBob });
	}

	//if (glm::length(movement) > 0) {
	//	movement = glm::mat3(transform.GetLocalTransform()) * movement;
	//	movement += transform.GetLocalPosition();
	//	transform.SetPosition(movement);
	//}
	//if (glm::length(rotation) > 0) {
	//	rotation.x *= glm::abs(myYawPitch.y) > 90 ? -1 : 1;
	//	myYawPitch += rotation;
	//	myYawPitch = wrap(myYawPitch, glm::vec2(-180.0f), glm::vec2(180.0f));
	//	glm::quat rot = glm::angleAxis(glm::radians(myYawPitch.x), glm::vec3(0, 1, 0));
	//	rot = rot * glm::angleAxis(glm::radians(myYawPitch.y), glm::vec3(1, 0, 0));
	//	transform.SetRotation(rot);
	//}
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
