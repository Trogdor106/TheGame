#include "HitBoxes.h"

HitBoxes::HitBoxes() :objectID(0), permHitBoxHolder(0)
{
}

void HitBoxes::saveHitBoxes(std::vector<Vertex> objectData)
{
	maxAndMin hitBoxHolder;
	/*
	hitBoxHolder[objectID].MaxX = 0;
	hitBoxHolder[objectID].MinX = 0;
	hitBoxHolder[objectID].MaxY = 0;
	hitBoxHolder[objectID].MinY = 0;
	hitBoxHolder[objectID].MaxZ = 0;
	hitBoxHolder[objectID].MinZ = 0;
	for (int i = 0; i < objectData.size(); i++) {
		if (objectData[objectID].Position[intForX] < hitBoxHolder[objectID].MinX) {
			hitBoxHolder[objectID].MinX = objectData[objectID].Position[intForX];
		}
		else if (objectData[objectID].Position[intForX] > hitBoxHolder[objectID].MaxX) {
			hitBoxHolder[objectID].MaxX = objectData[objectID].Position[intForX];
		}
		if (objectData[objectID].Position[intForY] < hitBoxHolder[objectID].MinY) {
			hitBoxHolder[objectID].MinY = objectData[objectID].Position[intForY];
		}
		else if (objectData[objectID].Position[intForY] > hitBoxHolder[objectID].MaxY) {
			hitBoxHolder[objectID].MaxY = objectData[objectID].Position[intForY];
		}
		if (objectData[objectID].Position[intForZ] < hitBoxHolder[objectID].MinZ) {
			hitBoxHolder[objectID].MinZ = objectData[objectID].Position[intForZ];
		}
		else if (objectData[objectID].Position[intForZ] > hitBoxHolder[objectID].MaxZ) {
			hitBoxHolder[objectID].MaxZ = objectData[objectID].Position[intForZ];
		}
	}
	*/
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
	objectID++;
}

bool HitBoxes::testHitBoxes(glm::vec3 &cameraPos, int ObjectID)
{
	if (cameraPos.z > permHitBoxHolder[ObjectID].MinZ && cameraPos.z < permHitBoxHolder[ObjectID].MaxZ) {
		float diffX = 0;
		float diffY = 0;

		if (cameraPos.x > permHitBoxHolder[ObjectID].MinX && cameraPos.x < permHitBoxHolder[ObjectID].MaxX) {
		
			diffX = abs(cameraPos.x - permHitBoxHolder[ObjectID].MinX) < abs(cameraPos.x - permHitBoxHolder[ObjectID].MaxX) ?
				abs(cameraPos.x - permHitBoxHolder[ObjectID].MinX) : abs(cameraPos.x - permHitBoxHolder[ObjectID].MaxX);
		}
		
		if (cameraPos.y > permHitBoxHolder[ObjectID].MinY&& cameraPos.y < permHitBoxHolder[ObjectID].MaxY) {
			diffY = abs(cameraPos.y - permHitBoxHolder[ObjectID].MinY) < abs(cameraPos.y - permHitBoxHolder[ObjectID].MaxY) ?
				abs(cameraPos.y - permHitBoxHolder[ObjectID].MinY) : abs(cameraPos.y - permHitBoxHolder[ObjectID].MaxY);
		}
		//if (diffX != 0 || diffY != 0) {
		if (diffX != 0 && diffY != 0) {
			if (diffX < diffY) {
				cameraPos.x = abs(cameraPos.x - permHitBoxHolder[ObjectID].MinX) < abs(cameraPos.x - permHitBoxHolder[ObjectID].MaxX) ?
					permHitBoxHolder[ObjectID].MinX : permHitBoxHolder[ObjectID].MaxX;
			}
			else {
				cameraPos.y = abs(cameraPos.y - permHitBoxHolder[ObjectID].MinY) < abs(cameraPos.y - permHitBoxHolder[ObjectID].MaxY) ?
					permHitBoxHolder[ObjectID].MinY : permHitBoxHolder[ObjectID].MaxY;
			}
		return true;
		}
	}
	return false;
}
