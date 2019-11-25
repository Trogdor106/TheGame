#include "HitBoxes.h"

HitBoxes::HitBoxes() :objectID(0), permHitBoxHolder(0)
{}

void HitBoxes::saveHitBoxes(std::vector<Vertex> objectData) {
	maxAndMin hitBoxHolder;
	
	hitBoxHolder.MaxX = 0;
	hitBoxHolder.MinX = 0;
	hitBoxHolder.MaxY = 0;
	hitBoxHolder.MinY = 0;
	hitBoxHolder.MaxZ = 0;
	hitBoxHolder.MinZ = 0;
	for (int i = 0; i < objectData.size(); i++) {
		float Hello = hitBoxHolder.MinX;
		if (objectData[i].Position[intForX] < hitBoxHolder.MinX) {
			hitBoxHolder.MinX = objectData[i].Position[intForX];
		}
		else if (objectData[i].Position[intForX] > hitBoxHolder.MaxX) {
			hitBoxHolder.MaxX = objectData[i].Position[intForX];
		}
		if (objectData[i].Position[intForY] < hitBoxHolder.MinY) {
			hitBoxHolder.MinY = objectData[i].Position[intForY];
		}
		else if (objectData[i].Position[intForY] > hitBoxHolder.MaxY) {
			hitBoxHolder.MaxY = objectData[i].Position[intForY];
		}
		if (objectData[i].Position[intForZ] < hitBoxHolder.MinZ) {
			hitBoxHolder.MinZ = objectData[i].Position[intForZ];
		}
		else if (objectData[i].Position[intForZ] > hitBoxHolder.MaxZ) {
			hitBoxHolder.MaxZ = objectData[i].Position[intForZ];
		}
	}
	permHitBoxHolder.push_back(hitBoxHolder);
	modHitBoxHolder.push_back(hitBoxHolder);
	objectID++;
}

bool HitBoxes::testHitBoxes(glm::vec3 &cameraPos, int ObjectID) {
	if (cameraPos.z > modHitBoxHolder[ObjectID].MinZ && cameraPos.z < modHitBoxHolder[ObjectID].MaxZ) {
		float diffX = 0;
		float diffY = 0;

		if (cameraPos.x > modHitBoxHolder[ObjectID].MinX && cameraPos.x < modHitBoxHolder[ObjectID].MaxX) {
		
			diffX = abs(cameraPos.x - modHitBoxHolder[ObjectID].MinX) < abs(cameraPos.x - modHitBoxHolder[ObjectID].MaxX) ?
				abs(cameraPos.x - modHitBoxHolder[ObjectID].MinX) : abs(cameraPos.x - modHitBoxHolder[ObjectID].MaxX);
		}
		
		if (cameraPos.y > modHitBoxHolder[ObjectID].MinY&& cameraPos.y < modHitBoxHolder[ObjectID].MaxY) {
			diffY = abs(cameraPos.y - modHitBoxHolder[ObjectID].MinY) < abs(cameraPos.y - modHitBoxHolder[ObjectID].MaxY) ?
				abs(cameraPos.y - modHitBoxHolder[ObjectID].MinY) : abs(cameraPos.y - modHitBoxHolder[ObjectID].MaxY);
		}
		//if (diffX != 0 || diffY != 0) {
		if (diffX != 0 && diffY != 0) {
			if (diffX < diffY) {
				cameraPos.x = abs(cameraPos.x - modHitBoxHolder[ObjectID].MinX) < abs(cameraPos.x - modHitBoxHolder[ObjectID].MaxX) ?
					modHitBoxHolder[ObjectID].MinX : modHitBoxHolder[ObjectID].MaxX;
			}
			else {
				cameraPos.y = abs(cameraPos.y - modHitBoxHolder[ObjectID].MinY) < abs(cameraPos.y - modHitBoxHolder[ObjectID].MaxY) ?
					modHitBoxHolder[ObjectID].MinY : modHitBoxHolder[ObjectID].MaxY;
			}
		return true;
		}
	}
	return false;
}

void HitBoxes::updateHitBoxes(glm::vec3 transformation, int ObjectID) {
	modHitBoxHolder[ObjectID].MaxX = permHitBoxHolder[ObjectID].MaxX + transformation.x;
	modHitBoxHolder[ObjectID].MinX = permHitBoxHolder[ObjectID].MinX + transformation.x;
	modHitBoxHolder[ObjectID].MaxY = permHitBoxHolder[ObjectID].MaxY + transformation.y;
	modHitBoxHolder[ObjectID].MinY = permHitBoxHolder[ObjectID].MinY + transformation.y;
	modHitBoxHolder[ObjectID].MaxZ = permHitBoxHolder[ObjectID].MaxZ + transformation.z;
	modHitBoxHolder[ObjectID].MinZ = permHitBoxHolder[ObjectID].MinZ + transformation.z;
}
