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

	void saveHitBoxes(HitBox boxToSave);
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

private:
	std::vector <HitBox> OGHitBoxHolder; //Keeps the default hitbox
	std::vector <HitBox> hitBoxHolder; //The transformed into the world space hitBox
};
