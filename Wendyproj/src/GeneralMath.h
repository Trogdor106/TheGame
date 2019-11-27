#pragma once
#include "MorphObject.h"

class deCasteJau:MorphObject {
public:
	void saveDeCasteJauObject(const char* filenameOriginal, const char* filenameOriginal2, const char* filenameTarget, const char* filenameTarget2);
	void calculatedeCasteJau();
	std::vector<Vertex> getCurrentCasteJau(int objectID);

	typedef std::shared_ptr<deCasteJau> Sptr;
private:
	struct deCasteJauObject {
		std::vector <Vertex> originalModel1;
		std::vector <Vertex> originalModel2;
		std::vector <Vertex> currentModel;
		std::vector <Vertex> targetModel1;
		std::vector <Vertex> targetModel2;
	};	

	std::vector <deCasteJauObject> deCastleObjectList;

	std::vector <float> currentInterval;
	std::vector <float> amountOfTimess;

};