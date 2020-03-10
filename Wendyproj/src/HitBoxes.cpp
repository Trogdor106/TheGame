#include "HitBoxes.h"

HitBoxes::HitBoxes() {
	HitBox temp = { {0.0,0.0}, {0.0,0.0}, {0.0,0.0}, {0.0,0.0}, -1 };
	hitBoxHolder.push_back(temp);
	OGHitBoxHolder.push_back(temp);
}

std::vector<float> HitBoxes::rotate90(std::vector<float> toChange)
{
	float temp = toChange[0]; // 0 is x
	toChange[0] = -toChange[1];
	toChange[1] = temp;
	return toChange;
}

std::vector<float> HitBoxes::rotate180(std::vector<float> toChange)
{
	toChange[0] = -toChange[0];
	toChange[1] = -toChange[1];
	return toChange;
}

std::vector<float> HitBoxes::rotate270(std::vector<float> toChange)
{
	float temp = -toChange[0]; // 0 is x
	toChange[0] = toChange[1];
	toChange[1] = temp;
	return toChange;
}

std::vector<float> HitBoxes::rotateNeg90(std::vector<float> toChange)
{
	float temp = -toChange[0]; // 0 is x
	toChange[0] = toChange[1];
	toChange[1] = temp;
	return toChange;
}

std::vector<float> HitBoxes::rotateNeg180(std::vector<float> toChange)
{
	toChange[0] = -toChange[0];
	toChange[1] = -toChange[1];
	return toChange;
}

std::vector<float> HitBoxes::rotateNeg270(std::vector<float> toChange)
{
	float temp = toChange[0]; // 0 is x
	toChange[0] = -toChange[1];
	toChange[1] = temp;
	return toChange;
}

void HitBoxes::saveHitBoxes(HitBox boxToSave)
{
	//hitBoxHolder.size();
	if (hitBoxHolder[0].ID == -1) {
		hitBoxHolder[0] = boxToSave;
		OGHitBoxHolder[0] = boxToSave;
	}
	else {
		hitBoxHolder.push_back(boxToSave);
		OGHitBoxHolder.push_back(boxToSave);
	}
}

void HitBoxes::deleteHitBox(int id)
{
	for (int i = 0; i < hitBoxHolder.size(); i++) {
		if (hitBoxHolder[i].ID == id) {
			hitBoxHolder.erase(hitBoxHolder.begin() + i);
			OGHitBoxHolder.erase(OGHitBoxHolder.begin() + i);
		}
	}
}

void HitBoxes::updateHitBox(int id, glm::mat4 changes)
{
	for (int i = 0; i < hitBoxHolder.size(); i++) {
		if (hitBoxHolder[i].ID == id) {
			hitBoxHolder[i].bottomLeft[0] = OGHitBoxHolder[i].bottomLeft[0] + changes[3][0];
			hitBoxHolder[i].bottomLeft[1] = OGHitBoxHolder[i].bottomLeft[1] + changes[3][1];
			//hitBoxHolder[i].bottomLeft[2] = OGHitBoxHolder[i].bottomLeft[2] + changes[3][2];

			hitBoxHolder[i].topLeft[0] = OGHitBoxHolder[i].topLeft[0] + changes[3][0];
			hitBoxHolder[i].topLeft[1] = OGHitBoxHolder[i].topLeft[1] + changes[3][1];
			//hitBoxHolder[i].topLeft[2] = OGHitBoxHolder[i].topLeft[2] + changes[3][2];

			hitBoxHolder[i].bottomRight[0] = OGHitBoxHolder[i].bottomRight[0] + changes[3][0];
			hitBoxHolder[i].bottomRight[1] = OGHitBoxHolder[i].bottomRight[1] + changes[3][1];
			//hitBoxHolder[i].bottomRight[2] = OGHitBoxHolder[i].bottomRight[2] + changes[3][2];

			hitBoxHolder[i].topRight[0] = OGHitBoxHolder[i].topRight[0] + changes[3][0];
			hitBoxHolder[i].topRight[1] = OGHitBoxHolder[i].topRight[1] + changes[3][1];
			//hitBoxHolder[i].topRight[2] = OGHitBoxHolder[i].topRight[2] + changes[3][2];

		}
	}
}

void HitBoxes::updateHitBox(int id, float rotation)
{
	for (int i = 0; i < hitBoxHolder.size(); i++) {
		if (hitBoxHolder[i].ID == id) {
			//Maybe add actual rotation with angle for angled objects at some point (Not in class since I cannot think about math right now)

			if (rotation == 90) {
				hitBoxHolder[i].bottomLeft = rotate90(OGHitBoxHolder[i].bottomLeft);
				hitBoxHolder[i].bottomRight = rotate90(OGHitBoxHolder[i].bottomRight);
				hitBoxHolder[i].topLeft = rotate90(OGHitBoxHolder[i].topLeft);
				hitBoxHolder[i].topRight = rotate90(OGHitBoxHolder[i].topRight);
			}
			else if (rotation == 180) {
				hitBoxHolder[i].bottomLeft = rotate180(OGHitBoxHolder[i].bottomLeft);
				hitBoxHolder[i].bottomRight = rotate180(OGHitBoxHolder[i].bottomRight);
				hitBoxHolder[i].topLeft = rotate180(OGHitBoxHolder[i].topLeft);
				hitBoxHolder[i].topRight = rotate180(OGHitBoxHolder[i].topRight);
			}
			else if (rotation == 270) {
				hitBoxHolder[i].bottomLeft = rotate270(OGHitBoxHolder[i].bottomLeft);
				hitBoxHolder[i].bottomRight = rotate270(OGHitBoxHolder[i].bottomRight);
				hitBoxHolder[i].topLeft = rotate270(OGHitBoxHolder[i].topLeft);
				hitBoxHolder[i].topRight = rotate270(OGHitBoxHolder[i].topRight);
			}
			else if (rotation == -90) {
				hitBoxHolder[i].bottomLeft = rotateNeg90(OGHitBoxHolder[i].bottomLeft);
				hitBoxHolder[i].bottomRight = rotateNeg90(OGHitBoxHolder[i].bottomRight);
				hitBoxHolder[i].topLeft = rotateNeg90(OGHitBoxHolder[i].topLeft);
				hitBoxHolder[i].topRight = rotateNeg90(OGHitBoxHolder[i].topRight);
			}
			else if (rotation == -180) {
				hitBoxHolder[i].bottomLeft = rotateNeg180(OGHitBoxHolder[i].bottomLeft);
				hitBoxHolder[i].bottomRight = rotateNeg180(OGHitBoxHolder[i].bottomRight);
				hitBoxHolder[i].topLeft = rotateNeg180(OGHitBoxHolder[i].topLeft);
				hitBoxHolder[i].topRight = rotateNeg180(OGHitBoxHolder[i].topRight);
			}
			else if (rotation == -270) {
				hitBoxHolder[i].bottomLeft = rotateNeg270(OGHitBoxHolder[i].bottomLeft);
				hitBoxHolder[i].bottomRight = rotateNeg270(OGHitBoxHolder[i].bottomRight);
				hitBoxHolder[i].topLeft = rotateNeg270(OGHitBoxHolder[i].topLeft);
				hitBoxHolder[i].topRight = rotateNeg270(OGHitBoxHolder[i].topRight);
			}
		}
	}
}

bool HitBoxes::isInHitBox(glm::vec3 cameraPos)
{
				// pos							pos
				//  Top Left-----------TopRight
				//  |						  |
				//  |						  |
				//  |						  |
				//  |						  |
				//  |						  |
				//  BottomLeft------BottomRight						  
				//pos							pos

	for (int i = 0; i < hitBoxHolder.size(); i++) {
		if (cameraPos.x > hitBoxHolder[i].bottomLeft[0] && cameraPos.y < hitBoxHolder[i].topLeft[1] && cameraPos.x < hitBoxHolder[i].bottomRight[0] && cameraPos.y > hitBoxHolder[i].bottomRight[1]) {
			return true;
		}
		//else if (cameraPos.x > hitBoxHolder[i].bottomLeft[1] && cameraPos.x < hitBoxHolder[i].topRight[1]) {
		//	return true;
		//}
	}
	return false;
}
