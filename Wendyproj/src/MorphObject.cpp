#include "MorphObject.h"




void MorphObject::saveMorphObject(const char* filenameOriginal, const char* filenameTarget) {
	morphObject temp = { loadOBJ(filenameOriginal), loadOBJ(filenameOriginal) , loadOBJ(filenameTarget) };
	morphObjectList.push_back(temp);
	currentInterval.push_back(0);
	amountOfTimess.push_back(0);
	isAllowed.push_back(false);
}

void MorphObject::updateMorphObject() {
	static float filler = 0;
	for (int j = 0; j < morphObjectList.size(); j++) {
		if (isAllowed[j] && currentInterval[j] <= amountOfTimess[j]) {
			for (int i = 0; i < morphObjectList[j].originalModel.size(); i++) {

				glm::vec3 tempVertices = { 0, 0, 0 };
				glm::vec3 tempNeorm = { 0, 0, 0 };

				MorphObject::lerp(morphObjectList[j].originalModel[i].Position, morphObjectList[j].targetModel[i].Position, tempVertices, filler, 10, j, filler);
				MorphObject::lerp(morphObjectList[j].originalModel[i].Normal, morphObjectList[j].targetModel[i].Normal, tempNeorm, filler, 10, j, filler);
				morphObjectList[j].currentModel[i].Position = tempVertices;
				morphObjectList[j].currentModel[i].Normal = glm::normalize(tempNeorm);

			}
			currentInterval[j] += 1;
		}
	}
}

std::vector<Vertex> MorphObject::getCurrentModel(int objectID) {
	if (currentInterval[objectID] <= amountOfTimess[objectID] || amountOfTimess[objectID] == 0) {
		return morphObjectList[objectID].currentModel;
	}
	else {
		return morphObjectList[objectID].targetModel;
	}
}

void MorphObject::switchToTrue(int objectID)
{
	isAllowed[objectID] = true;
}

void  MorphObject::lerp(glm::vec3& startPoint, glm::vec3& goal, glm::vec3& currentPos, float& deltatime, float total_time, int objectID, float current_Interval) {
	//glm::vec3 diffVec = { goal.x - startPoint.x, goal.y - startPoint.y, goal.z - startPoint.z }
	float totalTime = total_time;
	int amountOfTimes = totalTime /0.009025;
	float percentagePerIntervals = 0.009025/ totalTime;
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

	
}
