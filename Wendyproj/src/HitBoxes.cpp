#include "HitBoxes.h"

HitBoxes::HitBoxes() :objectID(0), permHitBoxHolder(0)
{}

void HitBoxes::saveHitBoxes(std::vector<Vertex> objectData, int type) {
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
	typeOfObject.push_back(type);
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

bool HitBoxes::isInRangeToInteract(glm::vec3& cameraPos, int ObjectID)
{
	bool isRange = false;
	if (typeOfObject[ObjectID] != 0) {
		if (cameraPos.z > (modHitBoxHolder[ObjectID].MinZ - 10) && cameraPos.z < (modHitBoxHolder[ObjectID].MaxZ + 10)) {

			float distanceX = abs((cameraPos.x) - (((modHitBoxHolder[ObjectID].MaxX + modHitBoxHolder[ObjectID].MinX) / 2)));

			float distanceY = abs((cameraPos.y) - (((modHitBoxHolder[ObjectID].MaxY + modHitBoxHolder[ObjectID].MinY) / 2)));
			if (distanceX < 10 && distanceY < 10) {
				return true;
			}
		}
	}
	return isRange;
}


//bool isRangeAndInteract(glm::vec3& objectPosition, glm::vec3& playerPosition) {
//	bool isRange = false;
//
//	if (playerPosition.z - 10 > )
//
//		//if (cameraPos.z > modHitBoxHolder[ObjectID].MinZ&& cameraPos.z < modHitBoxHolder[ObjectID].MaxZ) {
//		//	float diffX = 0;
//		//	float diffY = 0;
//		//
//		//	if (cameraPos.x > modHitBoxHolder[ObjectID].MinX&& cameraPos.x < modHitBoxHolder[ObjectID].MaxX) {
//		//
//		//		diffX = abs(cameraPos.x - modHitBoxHolder[ObjectID].MinX) < abs(cameraPos.x - modHitBoxHolder[ObjectID].MaxX) ?
//		//			abs(cameraPos.x - modHitBoxHolder[ObjectID].MinX) : abs(cameraPos.x - modHitBoxHolder[ObjectID].MaxX);
//		//	}
//		//
//		//	if (cameraPos.y > modHitBoxHolder[ObjectID].MinY&& cameraPos.y < modHitBoxHolder[ObjectID].MaxY) {
//		//		diffY = abs(cameraPos.y - modHitBoxHolder[ObjectID].MinY) < abs(cameraPos.y - modHitBoxHolder[ObjectID].MaxY) ?
//		//			abs(cameraPos.y - modHitBoxHolder[ObjectID].MinY) : abs(cameraPos.y - modHitBoxHolder[ObjectID].MaxY);
//		//	}
//		//	//if (diffX != 0 || diffY != 0) {
//		//	if (diffX != 0 && diffY != 0) {
//		//		if (diffX < diffY) {
//		//			cameraPos.x = abs(cameraPos.x - modHitBoxHolder[ObjectID].MinX) < abs(cameraPos.x - modHitBoxHolder[ObjectID].MaxX) ?
//		//				modHitBoxHolder[ObjectID].MinX : modHitBoxHolder[ObjectID].MaxX;
//		//		}
//		//		else {
//		//			cameraPos.y = abs(cameraPos.y - modHitBoxHolder[ObjectID].MinY) < abs(cameraPos.y - modHitBoxHolder[ObjectID].MaxY) ?
//		//				modHitBoxHolder[ObjectID].MinY : modHitBoxHolder[ObjectID].MaxY;
//		//		}
//		//		return true;
//		//	}
//		//}
//		//return false;
//		//
//		//bool inRange = false;
//		//glm::vec3 distance = { abs(abs(myLanternTransform[3][0]) - abs(myCamera->GetPosition().x)),
//		//						abs(abs(myLanternTransform[3][1]) - abs(myCamera->GetPosition().y)),
//		//						abs(abs(myLanternTransform[3][2]) - abs(myCamera->GetPosition().z)) };
//		//
//		//if (distance.x < 5 && distance.y < 5 && distance.z < 5)
//		//	inRange = true;
//		//
//		//static float currentTime = 0;
//		//currentTime = currentTime + deltaTime;
//		//static bool wasLPressed = false;
//		//static bool isLPressed = false;
//		//if (glfwGetKey(myWindow, GLFW_KEY_L) == GLFW_PRESS && wasLPressed == false && inRange == true)
//		//{
//		//	wasLPressed = true;
//		//}
//		//
//		//static glm::vec3 start = myLanternTransform[3];
//		//glm::vec3 currentPos = myLanternTransform[3];
//		//static glm::vec3 end = glm::vec3(myLanternTransform[3][0], myLanternTransform[3][1], myLanternTransform[3][2] + 20);
//		//static float startTime = 0;
//		//
//		//if (wasLPressed)
//		//	lerp(end, start, currentPos, (10));
//		//
//		//myLanternTransform[3][0] = currentPos.x;
//		//myLanternTransform[3][1] = currentPos.y;
//		//myLanternTransform[3][2] = currentPos.z;
//
//}