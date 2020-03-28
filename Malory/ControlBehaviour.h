#pragma once
#include "florp/game/IBehaviour.h"
#include <GLM/glm.hpp>

class ControlBehaviour : public florp::game::IBehaviour {
public:
	ControlBehaviour(const glm::vec3& speed) : IBehaviour(), mySpeed(speed), myYawPitch(glm::vec2(0.0f)) {};
	virtual ~ControlBehaviour() = default;

	virtual void Update(entt::entity entity) override;

private:
	glm::vec3 mySpeed;
	glm::vec2 myYawPitch;
	float angleForX = 0;
	float angleForY = 0;
	float angleForZ = 0;
};

class InputBehaviour : public florp::game::IBehaviour {
public:

	InputBehaviour(glm::vec3 position) : IBehaviour(), positionSaved(position) {};
	virtual ~InputBehaviour() = default;
	virtual void Update(entt::entity entity);

private:
	glm::vec3 positionSaved;
	float mySpeed = 1.0;
};

class Car : public florp::game::IBehaviour {
public:

	Car(glm::vec3 position) : IBehaviour(), positionSaved(position), timer(100) {};
	virtual ~Car() = default;
	virtual void Update(entt::entity entity);

private:
	glm::vec3 positionSaved;
	float mySpeed = 1.0;
	float timer;
};

class Hit : public florp::game::IBehaviour {
public:

	Hit(glm::vec3 position) : IBehaviour(), positionSaved(position) {  };
	virtual ~Hit() = default;
	virtual void Update(entt::entity entity);

private:
	glm::vec3 positionSaved;
	float mySpeed = 1.0;
};