#include "GeneralMath.h"

void deCasteJau::saveDeCasteJauObject(const char* filenameOriginal, const char* filenameOriginal2, const char* filenameTarget, const char* filenameTarget2)
{
	/*
	morphObject temp = { loadOBJ(filenameOriginal), loadOBJ(filenameOriginal) , loadOBJ(filenameTarget) };
	morphObjectList.push_back(temp);
	currentInterval.push_back(0);
	amountOfTimess.push_back(0);
	*/
	deCasteJauObject temp = { loadOBJ(filenameOriginal), loadOBJ(filenameOriginal2), loadOBJ(filenameOriginal) , loadOBJ(filenameTarget), loadOBJ(filenameTarget2) };
	deCastleObjectList.push_back(temp);
	currentInterval.push_back(0);
	amountOfTimess.push_back(0);
	isAllowed.push_back(false);
}

void deCasteJau::calculatedeCasteJau() {
	for (int j = 0; j < deCastleObjectList.size(); j++) {
		if (isAllowed[j] == true) {
			float tempAmountOfTimes = amountOfTimess[j];
			if (currentInterval[j] <= amountOfTimess[j]) {
				for (int i = 0; i < deCastleObjectList[j].originalModel1.size(); i++) {

					glm::vec3 tempVertices = { 0, 0, 0 };
					glm::vec3 tempVertices2 = { 0, 0, 0 };
					glm::vec3 tempVertices3 = { 0, 0, 0 };
					glm::vec3 tempVertices4 = { 0, 0, 0 };
					glm::vec3 tempVertices5 = { 0, 0, 0 };
					glm::vec3 tempVertices6 = { 0, 0, 0 };
					glm::vec3 tempNeorm = { 0, 0, 0 };
					glm::vec3 tempNeorm2 = { 0, 0, 0 };
					glm::vec3 tempNeorm3 = { 0, 0, 0 };
					glm::vec3 tempNeorm4 = { 0, 0, 0 };
					glm::vec3 tempNeorm5 = { 0, 0, 0 };
					glm::vec3 tempNeorm6 = { 0, 0, 0 };


					MorphObject::lerp(deCastleObjectList[j].originalModel1[i].Position, deCastleObjectList[j].originalModel2[i].Position, tempVertices, tempAmountOfTimes,		2, j, currentInterval[j]);
					MorphObject::lerp(deCastleObjectList[j].originalModel2[i].Position, deCastleObjectList[j].targetModel1[i].Position, tempVertices2, tempAmountOfTimes,		2, j, currentInterval[j]);
					MorphObject::lerp(deCastleObjectList[j].targetModel1[i].Position, deCastleObjectList[j].targetModel2[i].Position, tempVertices3, tempAmountOfTimes,			2, j, currentInterval[j]);
					MorphObject::lerp(tempVertices, tempVertices2, tempVertices4, tempAmountOfTimes,																			2, j, currentInterval[j]);
					MorphObject::lerp(tempVertices2, tempVertices3, tempVertices5, tempAmountOfTimes,																			2, j, currentInterval[j]);
					MorphObject::lerp(tempVertices4, tempVertices5, tempVertices6, tempAmountOfTimes,																			2, j, currentInterval[j]);
																																												
					MorphObject::lerp(deCastleObjectList[j].originalModel1[i].Normal, deCastleObjectList[j].originalModel2[i].Normal, tempNeorm, tempAmountOfTimes,				2, j, currentInterval[j]);
					MorphObject::lerp(deCastleObjectList[j].originalModel2[i].Normal, deCastleObjectList[j].targetModel1[i].Normal, tempNeorm2, tempAmountOfTimes,				2, j, currentInterval[j]);
					MorphObject::lerp(deCastleObjectList[j].targetModel1[i].Normal, deCastleObjectList[j].targetModel2[i].Normal, tempNeorm3, tempAmountOfTimes,				2, j, currentInterval[j]);
					MorphObject::lerp(tempNeorm, tempNeorm2, tempNeorm4, tempAmountOfTimes,																						2, j, currentInterval[j]);
					MorphObject::lerp(tempNeorm2, tempNeorm3, tempNeorm5, tempAmountOfTimes,																					2, j, currentInterval[j]);
					MorphObject::lerp(tempNeorm4, tempNeorm5, tempNeorm6, tempAmountOfTimes,																					2, j, currentInterval[j]);

					deCastleObjectList[j].currentModel[i].Position = tempVertices6;
					deCastleObjectList[j].currentModel[i].Normal = glm::normalize(tempNeorm6);

					//deCastleObjectList[j].currentModel[i].Position = tempVertices;
					//deCastleObjectList[j].currentModel[i].Normal = glm::normalize(tempNeorm);

					//MorphObject::lerp(deCastleObjectList[j].currentModel[i].Normal, deCastleObjectList[j].originalModel1[i].Normal, tempNeorm, 0, 10, j);
					//deCastleObjectList[j].currentModel[i].Position = tempVertices;
					//deCastleObjectList[j].currentModel[i].Normal = glm::normalize(tempNeorm);


					//or (int i = 0; i < mesh->vertices.size(); i++)
					//
					//	glm::vec3 v = Math::lerp(m_pMorphTargets[m_pCurrentKeyframe]->vertices[i], m_pMorphTargets[m_pNextKeyframe]->vertices[i], m_pLocalMorphTime);
					//	glm::vec3 n = Math::lerp(m_pMorphTargets[m_pCurrentKeyframe]->normals[i], m_pMorphTargets[m_pNextKeyframe]->normals[i], m_pLocalMorphTime);
					//	mesh->vertices[i] = v;
					//	mesh->normals[i] = glm::normalize(n);
					//
				}
			}
			amountOfTimess[j] = tempAmountOfTimes;
			currentInterval[j] += 0.095;
		}
	}
}

std::vector<Vertex> deCasteJau::getCurrentCasteJau(int objectID)
{
	if (currentInterval[objectID] <= amountOfTimess[objectID] || amountOfTimess[objectID] == 0) {
		return deCastleObjectList[objectID].currentModel;
	}
	else {
		return deCastleObjectList[objectID].targetModel2;
	}
}

void deCasteJau::switchToTrue(int objectID)
{
	isAllowed[objectID] = true;
}
