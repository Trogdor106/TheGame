#include "MorphObject.h"




void MorphObject::saveMorphObject(const char* filenameOriginal, const char* filenameTarget) {
	morphObject temp = { loadOBJ(filenameOriginal), loadOBJ(filenameOriginal) , loadOBJ(filenameTarget) };
	morphObjectList.push_back(temp);
	currentInterval.push_back(0);
	amountOfTimess.push_back(0);
}

void MorphObject::updateMorphObject() {
	static float filler = 0;
	for (int j = 0; j < morphObjectList.size(); j++) {
		for (int i = 0; i < morphObjectList[j].originalModel.size(); i++) {

			glm::vec3 tempVertices = { 0, 0, 0 };
			glm::vec3 tempNeorm = { 0, 0, 0 };

			MorphObject::lerp( morphObjectList[j].originalModel[i].Position, morphObjectList[j].targetModel[i].Position, tempVertices, filler, 10, j, filler);
			MorphObject::lerp( morphObjectList[j].originalModel[i].Normal, morphObjectList[j].targetModel[i].Normal, tempNeorm, filler, 10, j, filler);
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
	currentInterval[j] += 0.095;
	}
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
	if (currentInterval[objectID] <= amountOfTimess[objectID] || amountOfTimess[objectID] == 0) {
		return morphObjectList[objectID].currentModel;
	}
	else {
		return morphObjectList[objectID].targetModel;
	}
}

void  MorphObject::lerp(glm::vec3& startPoint, glm::vec3& goal, glm::vec3& currentPos, float& deltatime, float total_time, int objectID, float current_Interval) {
	//glm::vec3 diffVec = { goal.x - startPoint.x, goal.y - startPoint.y, goal.z - startPoint.z }
	float totalTime = total_time;
	int amountOfTimes = totalTime / 0.095;
	float percentagePerIntervals = 0.095 / totalTime;
	if (amountOfTimess.empty() != true) {
		if (amountOfTimess[objectID] == 0) {
			amountOfTimess[objectID] = amountOfTimes;
		}
		if (deltatime == 0) {
			deltatime = amountOfTimes;
		}

		if (currentInterval[objectID] <= amountOfTimess[objectID] && currentInterval[objectID] >= 0) {
			float t = startPoint.x != goal.x ? (((percentagePerIntervals * currentInterval[objectID]) * (goal.x - startPoint.x) + startPoint.x) - startPoint.x) / (goal.x - startPoint.x) : 1;
			currentPos.x = (1 - t) * startPoint.x + t * goal.x;
			float t2 = startPoint.y != goal.y ? (((percentagePerIntervals * currentInterval[objectID]) * (goal.y - startPoint.y) + startPoint.y) - startPoint.y) / (goal.y - startPoint.y) : 1;
			currentPos.y = (1 - t2) * startPoint.y + t2 * goal.y;
			float t3 = startPoint.z != goal.z ? (((percentagePerIntervals * currentInterval[objectID]) * (goal.z - startPoint.z) + startPoint.z) - startPoint.z) / (goal.z - startPoint.z) : 1;
			currentPos.z = (1 - t3) * startPoint.z + t3 * goal.z;

		}
	}
	else {
		if (deltatime == 0) {
			deltatime = amountOfTimes;
		}
		if (current_Interval <= deltatime && current_Interval >= 0) {
			float t = startPoint.x != goal.x ? (((percentagePerIntervals * current_Interval) * (goal.x - startPoint.x) + startPoint.x) - startPoint.x) / (goal.x - startPoint.x) : 1;
			currentPos.x = (1 - t) * startPoint.x + t * goal.x;
			float t2 = startPoint.y != goal.y ? (((percentagePerIntervals * current_Interval) * (goal.y - startPoint.y) + startPoint.y) - startPoint.y) / (goal.y - startPoint.y) : 1;
			currentPos.y = (1 - t2) * startPoint.y + t2 * goal.y;
			float t3 = startPoint.z != goal.z ? (((percentagePerIntervals * current_Interval) * (goal.z - startPoint.z) + startPoint.z) - startPoint.z) / (goal.z - startPoint.z) : 1;
			currentPos.z = (1 - t3) * startPoint.z + t3 * goal.z;

		}
	}

	//if (currentInterval == amountOfTimes) {
	//	glm::vec3 temp = startPoint;
	//	startPoint = goal;
	//	goal = startPoint;
	//	currentInterval = 0;
	//}
	
}

//Fowards kinematic here if we have time