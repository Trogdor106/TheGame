#include "MorphObject.h"




void MorphObject::saveMorphObject(const char* filenameOriginal, const char* filenameTarget) {
	morphObject temp = { loadOBJ(filenameOriginal), loadOBJ(filenameOriginal) , loadOBJ(filenameTarget) };
	morphObjectList.push_back(temp);
}

void MorphObject::updateMorphObject(float deltatime, int objectID, glm::mat4 objTransform) {
	for (int j = 0; j < morphObjectList.size(); j++) {
		for (int i = 0; i < morphObjectList[j].originalModel.size(); i++) {

			glm::vec3 tempVertices = { 0, 0, 0 };
			glm::vec3 tempNeorm = { 0, 0, 0 };

			MorphObject::lerp(morphObjectList[j].targetModel[i].Position, morphObjectList[j].originalModel[i].Position, tempVertices, deltatime, 10);
			MorphObject::lerp(morphObjectList[j].targetModel[i].Normal, morphObjectList[j].originalModel[i].Normal, tempNeorm, deltatime, 10);
			morphObjectList[j].currentModel[i].Position = tempVertices;
			morphObjectList[j].currentModel[i].Normal = glm::normalize(tempNeorm);
			
			
			//or (int i = 0; i < mesh->vertices.size(); i++)
			//
			//	glm::vec3 v = Math::lerp(m_pMorphTargets[m_pCurrentKeyframe]->vertices[i], m_pMorphTargets[m_pNextKeyframe]->vertices[i], m_pLocalMorphTime);
			//	glm::vec3 n = Math::lerp(m_pMorphTargets[m_pCurrentKeyframe]->normals[i], m_pMorphTargets[m_pNextKeyframe]->normals[i], m_pLocalMorphTime);
			//	mesh->vertices[i] = v;
			//	mesh->normals[i] = glm::normalize(n);
			//
		}
	}
	currentInterval += 0.095;
}

// Uses start point, end point, the percentage to move each frame (speed, essentially), the matrix you're modifying, and the xyz index
//float MorphObject::justLerp(float p1, float p2, float want, glm::mat4 objTransform, int index) //Index 0 = x, 1 = y, 2 = z
//{
//
//	float newPos;
//
//	float t = (want - p1) / (p2 - p1);
//
//	newPos = ((1 - t) * p1) + (t * p2);
//
//	//If you've reached/exceeded p2, stop moving each frame
//	if (objTransform[3][index] >= p2)
//	{
//		newPos = 0;
//	}
//
//	return newPos;
//}

std::vector<Vertex> MorphObject::getCurrentModel(int objectID) {
	if (currentInterval <= amountOfTimess) {
		return morphObjectList[objectID].currentModel;
	}
	else {
		return morphObjectList[objectID].targetModel;
	}
}

void  MorphObject::lerp(glm::vec3& goal, glm::vec3& startPoint, glm::vec3& currentPos, float deltatime, float total_time) {
	//glm::vec3 diffVec = { goal.x - startPoint.x, goal.y - startPoint.y, goal.z - startPoint.z }
	static float totalTime = total_time;
	static int amountOfTimes = totalTime / 0.095;
	amountOfTimess = amountOfTimes;
	static float percentagePerIntervals = 0.095 / totalTime;

	if (currentInterval <= amountOfTimes && currentInterval >= 0) {
		float t = startPoint.x != goal.x ? (((percentagePerIntervals * currentInterval) * (goal.x - startPoint.x) + startPoint.x) - startPoint.x) / (goal.x - startPoint.x) : 1;
		currentPos.x = (1 - t) * startPoint.x + t * goal.x;
		float t2 = startPoint.y != goal.y ? (((percentagePerIntervals * currentInterval) * (goal.y - startPoint.y) + startPoint.y) - startPoint.y) / (goal.y - startPoint.y) : 1;
		currentPos.y = (1 - t2) * startPoint.y + t2 * goal.y;
		float t3 = startPoint.z != goal.z ? (((percentagePerIntervals * currentInterval) * (goal.z - startPoint.z) + startPoint.z) - startPoint.z) / (goal.z - startPoint.z) : 1;
		currentPos.z = (1 - t3) * startPoint.z + t3 * goal.z;

	}

	//if (currentInterval == amountOfTimes) {
	//	glm::vec3 temp = startPoint;
	//	startPoint = goal;
	//	goal = startPoint;
	//	currentInterval = 0;
	//}
	
}