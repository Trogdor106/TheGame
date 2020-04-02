#pragma once
#include "florp/game/IBehaviour.h"
#include <GLM/glm.hpp>
#include "HitBoxes.h"
#include "ShadowLight.h"
#include "PointLightComponent.h"
#include "AudioEngine.h"


class lantern : public florp::game::IBehaviour {
public:
	lantern() {};
	virtual ~lantern() = default;
	virtual void Update(entt::entity entity);


private:
	const float PI = 3.1415926535897932384626433832795028841971693993;

};

class ControlBehaviour : public florp::game::IBehaviour {
public:
	ControlBehaviour(const glm::vec3& speed) : IBehaviour(), mySpeed(speed), myYawPitch(glm::vec2(0.0f)) {};
	virtual ~ControlBehaviour() = default;

	virtual void Update(entt::entity entity) override;

private:
	glm::vec3 mySpeed;
	glm::vec2 myYawPitch;
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

class doorManDoors : public florp::game::IBehaviour {
public:
	doorManDoors(const glm::vec3& position, float floor, int id) : IBehaviour(), myPosition(position), myYawPitch(glm::vec2(0.0f)), interacted(0), floorCurrent(floor), idOfDoor(id) {};
	virtual ~doorManDoors() = default;

	virtual void Update(entt::entity entity) override;

private:
	int idOfDoor;
	int floorCurrent; //Despite the name this actually represents the floor the obeject is on
	int interacted;
	glm::vec3 myPosition;
	glm::vec2 myYawPitch;
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

class death : public florp::game::IBehaviour {
public:
	death() : IBehaviour() {};
	virtual ~death() = default;

	virtual void Update(entt::entity entity) override;

private:
	int floorCurrent; //Despite the name this actually represents the floor the obeject is on
	int interacted;
	glm::vec3 myPosition;
	glm::vec2 myYawPitch;
};

class aStarAlgorithm : public florp::game::IBehaviour {
public:
	aStarAlgorithm() : IBehaviour(), floorCurrent(0), interacted(0), distance(0), estimatedStepsX(0), estimatedStepsY(0), onStep(0) {};
	virtual ~aStarAlgorithm() = default;

	virtual void Update(entt::entity entity) override;

private:
	int floorCurrent; //Despite the name this actually represents the floor the obeject is on
	int interacted;
	glm::vec3 myPosition;
	float distance;
	int estimatedStepsX, estimatedStepsY, onStep;
	int delay = 20;
	int index = 0;

	std::vector <int> directions; //Can go from 0 to 3
									//			0			
									//
									//
									//3			p			1
									//
									//
									//			2			

};

//class ListenerBehaviour : public florp::game::IBehaviour {
//public:
//	ListenerBehaviour() : IBehaviour() {};
//	virtual ~ListenerBehaviour() = default;
//
//	virtual void OnLoad(entt::entity entity) override {
//		auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
//
//	}
//
//	virtual void Update(entt::entity entity) override;
//
//};

class Listener : public florp::game::IBehaviour {
public:
	Listener() : IBehaviour() {};
	virtual ~Listener() = default;
	virtual void Update(entt::entity) override;
	virtual void OnLoad(entt::entity) override;
};

class BackgroundMusic : public florp::game::IBehaviour {
public:
	BackgroundMusic() : IBehaviour() {};
	virtual ~BackgroundMusic() = default;
	virtual void OnLoad(entt::entity entity) override;
	virtual void Update(entt::entity entity) override;

};

