#pragma once
#include <iostream>
#include <vector>
#include "Mesh.h"



class HitBoxes {
public:
	typedef std::shared_ptr<HitBoxes> Sptr;
	HitBoxes();
	void saveHitBoxes(std::vector <Vertex> objectData);
	bool testHitBoxes(glm::vec3& cameraPos, int ObjectID);
	void updateHitBoxes(glm::vec3 transformation, int ObjectID);
private:
	struct maxAndMin {
		float MaxX;
		float MinX;
		float MaxY;
		float MinY;
		float MaxZ;
		float MinZ;
	};

	std::vector <maxAndMin> permHitBoxHolder; //the max min holder (OG as to not be rewritten)
	std::vector <maxAndMin> modHitBoxHolder; //the max min holder (What get's passed (this takes the modifications))

	
	int objectID;
	int intForX = 0;
	int intForY = 1;
	int intForZ = 2;
};