#pragma once
#include "OBJLoader.h"


class MorphObject {
public:
	void saveMorphObject(const char* filenameOriginal, const char* filenameTarget);
	void updateMorphObject();
	//float justLerp(float p1, float p2, float want, glm::mat4 objTransform, int index);
	void lerp(glm::vec3& startPoint, glm::vec3& goal, glm::vec3& currentPos, float& deltatime, float total_time, int objectID, float current_Interval);
	std::vector <Vertex> getCurrentModel(int objectID);
	typedef std::shared_ptr<MorphObject> Sptr;
	void switchToTrue(int objectID);
private:

	struct morphObject {
		std::vector <Vertex> originalModel;
		std::vector <Vertex> currentModel;
		std::vector <Vertex> targetModel;
	};
	
	std::vector <morphObject> morphObjectList;

	std::vector <bool> isAllowed;
	std::vector <float> currentInterval;
	std::vector <float> amountOfTimess;
};