#pragma once
#include <iostream>
#include <vector>
#include "Mesh.h"
#include "GLM/glm.hpp"

struct HitBox {
	float top;
	float Bottom;
	float Left;
	float Right;
	int ID;
	glm::vec3 centerPoint = {0, 0, 0};
};

class HitBoxes {
public:
	typedef std::shared_ptr<HitBoxes> Sptr;
	HitBoxes();
	std::vector <float> rotate90(std::vector<float> toChange);
	std::vector <float> rotate180(std::vector<float> toChange);
	std::vector <float> rotate270(std::vector<float> toChange);
	std::vector <float> rotateNeg90(std::vector<float> toChange);
	std::vector <float> rotateNeg180(std::vector<float> toChange);
	std::vector <float> rotateNeg270(std::vector<float> toChange);

	void saveHitBoxes(HitBox boxToSave, int floor);
	void deleteHitBox(int id);
	void updateHitBox(int id, glm::mat4 changes);
	/*
	Use this one for translations only, not that we do rotations properly anyway
	*/
	void updateHitBox(int id, float rotation);
	/*
	Use this one for rotation of objects (it's not true rotation because of how our hitboxing works so only does every 45 degrees)
	*/
	bool isInHitBox(glm::vec3 cameraPos, glm::vec3 cameraPosGoal);
	void saveFloor(int floor);

private:
	int currentFloor;

	std::vector <int> rotationFirst;
	std::vector <int> rotationSecond;
	std::vector <HitBox> OGHitBoxHolderFirst; //Keeps the default hitbox
	std::vector <HitBox> OGHitBoxHolderSecond; //Keeps the default hitbox
	std::vector <HitBox> OGHitBoxHolderThird; //Keeps the default hitbox
	std::vector <HitBox> hitBoxHolderFirst; //The transformed into the world space hitBox
	std::vector <HitBox> hitBoxHolderSecond; //The transformed into the world space hitBox
	std::vector <HitBox> hitBoxHolderThird; //The transformed into the world space hitBox
	//^ what would have been better would be to have a vector holding these so they are not as dis-organised
	//Didn't think of that when I typed it at first and now it works so I will leave it (if the game gets expanded put this into a vector
	//And change the switch to simply something like generalHitBoxHolder[currentFloor])
};
