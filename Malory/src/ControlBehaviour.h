#pragma once
#include "florp/game/IBehaviour.h"
#include <GLM/glm.hpp>
#include "HitBoxes.h"

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

class doorOpening : public florp::game::IBehaviour {
public:
	doorOpening(int floor) : IBehaviour(), interacted(0), floorObjectIsOn(floor) {  };
	virtual ~doorOpening() = default;
	virtual void Update(entt::entity entity);

	
private:
	int interacted;
	int floorObjectIsOn;
	const float PI = 3.1415926535897932384626433832795028841971693993;

};

class lockedDoor : public florp::game::IBehaviour {
public:
	lockedDoor(int floor, int doorNum) : IBehaviour(), interacted(0), floorObjectIsOn(floor), doorID(doorNum) {  };
	virtual ~lockedDoor() = default;
	virtual void Update(entt::entity entity);


private:
	int doorID;
	int interacted;
	int floorObjectIsOn;
	const float PI = 3.1415926535897932384626433832795028841971693993;

};

class key : public florp::game::IBehaviour {
public:
	key(int floor, int keyNum);
	virtual ~key() = default;
	virtual void Update(entt::entity entity);


private:
	int keyID;
	int interacted;
	int floorObjectIsOn;
	const float PI = 3.1415926535897932384626433832795028841971693993;

};

class stairs1 : public florp::game::IBehaviour {
public:
	stairs1(int floor) : IBehaviour(), interacted(0), floorObjectIsOn(floor) {  };
	virtual ~stairs1() = default;
	virtual void Update(entt::entity entity);


private:
	int interacted;
	int floorObjectIsOn;
	const float PI = 3.1415926535897932384626433832795028841971693993;

};

class stairs2 : public florp::game::IBehaviour {
public:
	stairs2(int floor) : IBehaviour(), interacted(0), floorObjectIsOn(floor) {  };
	virtual ~stairs2() = default;
	virtual void Update(entt::entity entity);


private:
	int interacted;
	int floorObjectIsOn;
	const float PI = 3.1415926535897932384626433832795028841971693993;

};