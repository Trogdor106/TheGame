#include "HitBoxes.h"

HitBoxes::HitBoxes() {
	HitBox temp = { {0.0}, {0.0}, {0.0}, {0.0}, -1 };
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
			hitBoxHolder[i].Bottom = OGHitBoxHolder[i].Bottom + changes[3][0];
			hitBoxHolder[i].Bottom = OGHitBoxHolder[i].Bottom + changes[3][1];
			//hitBoxHolder[i].bottomLeft[2] = OGHitBoxHolder[i].bottomLeft[2] + changes[3][2];

			hitBoxHolder[i].top = OGHitBoxHolder[i].top + changes[3][0];
			hitBoxHolder[i].top = OGHitBoxHolder[i].top + changes[3][1];
			//hitBoxHolder[i].topLeft[2] = OGHitBoxHolder[i].topLeft[2] + changes[3][2];

			hitBoxHolder[i].Right = OGHitBoxHolder[i].Right + changes[3][0];
			hitBoxHolder[i].Right = OGHitBoxHolder[i].Right + changes[3][1];
			//hitBoxHolder[i].bottomRight[2] = OGHitBoxHolder[i].bottomRight[2] + changes[3][2];

			hitBoxHolder[i].Left = OGHitBoxHolder[i].Left + changes[3][0];
			hitBoxHolder[i].Left = OGHitBoxHolder[i].Left + changes[3][1];
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
				hitBoxHolder[i].Left = OGHitBoxHolder[i].Bottom;
				hitBoxHolder[i].Right = OGHitBoxHolder[i].top;
				hitBoxHolder[i].top = OGHitBoxHolder[i].Right;
				hitBoxHolder[i].Bottom = OGHitBoxHolder[i].Left;
			}
			else if (rotation == 180) {
				hitBoxHolder[i].Left = OGHitBoxHolder[i].Right;
				hitBoxHolder[i].Right = OGHitBoxHolder[i].Left;
				hitBoxHolder[i].top = OGHitBoxHolder[i].Bottom;
				hitBoxHolder[i].Bottom = OGHitBoxHolder[i].top;
			}
			else if (rotation == 270) {
				hitBoxHolder[i].Left = OGHitBoxHolder[i].top;
				hitBoxHolder[i].Right = OGHitBoxHolder[i].Bottom;
				hitBoxHolder[i].top = OGHitBoxHolder[i].Left;
				hitBoxHolder[i].Bottom = OGHitBoxHolder[i].Right;
			}
			else if (rotation == -90) {

				hitBoxHolder[i].Left = OGHitBoxHolder[i].top;
				hitBoxHolder[i].Right = OGHitBoxHolder[i].Bottom;
				hitBoxHolder[i].top = OGHitBoxHolder[i].Left;
				hitBoxHolder[i].Bottom = OGHitBoxHolder[i].Right;
			}
			else if (rotation == -180) {
				hitBoxHolder[i].Left = OGHitBoxHolder[i].Right;
				hitBoxHolder[i].Right = OGHitBoxHolder[i].Left;
				hitBoxHolder[i].top = OGHitBoxHolder[i].Bottom;
				hitBoxHolder[i].Bottom = OGHitBoxHolder[i].top;
			}
			else if (rotation == -270) {
				hitBoxHolder[i].Left = OGHitBoxHolder[i].Bottom;
				hitBoxHolder[i].Right = OGHitBoxHolder[i].top;
				hitBoxHolder[i].top = OGHitBoxHolder[i].Right;
				hitBoxHolder[i].Bottom = OGHitBoxHolder[i].Left;
			}
		}
	}
}

bool HitBoxes::isInHitBox(glm::vec3 cameraPos, glm::vec3 cameraPosGoal)
{
	//Innifecient to do it like this but it's simpler
	static int steps = 10;
	float distancePerStepX = (cameraPosGoal.x - cameraPos.x) / steps;
	float distancePerStepY = (cameraPosGoal.y - cameraPos.y) / steps;
	if (distancePerStepX != 0) {
		int hello = 0;
	}
	std::vector <glm::vec2> xY;
	for (int i = 1; i <= steps; i++) {
		xY.push_back(glm::vec2((cameraPos.x + distancePerStepX * i), (cameraPos.y + distancePerStepY * i)));
	}

				// pos							pos
				//  Top Left-----------TopRight
				//  |						  |
				//  |						  |
				//  |						  |
				//  |						  |
				//  |						  |
				//  BottomLeft------BottomRight						  
				//pos							pos

				// if  >BL.x and <TR.x
				// if  >BL.y and <TR.y

				// if  <BR.x and >TL.x
				// if  <BR.y and >TL.y
	for (int i = 0; i < hitBoxHolder.size(); i++) {
		//for (int p = 0; p < steps; p++) {
			//if (xY[p].x > hitBoxHolder[i].bottomLeft[0] && xY[p].x < hitBoxHolder[i].topRight[0]) { 
			//	if (xY[p].y < hitBoxHolder[i].bottomLeft[1] && xY[p].y > hitBoxHolder[i].topRight[1]) {
			//		return true;
			//	}
			//}
			//if (xY[p].x >	 hitBoxHolder[i].bottomRight[0] && xY[p].x < hitBoxHolder[i].topLeft[0]) {
			//	if (xY[p].y < hitBoxHolder[i].bottomRight[1] && xY[p].y > hitBoxHolder[i].topLeft[1]) {
			//		return true;
			//	}
			//}


		//}
		if (cameraPos.y >= hitBoxHolder[i].Bottom && cameraPos.y <= hitBoxHolder[i].top){
			if (cameraPos.x >= hitBoxHolder[i].Left && cameraPos.x <= hitBoxHolder[i].Right) {
				return false;
			}
		}
		//if (cameraPos.x > hitBoxHolder[i].bottomLeft[0] && cameraPos.y < hitBoxHolder[i].topLeft[1] && cameraPos.x < hitBoxHolder[i].bottomRight[0] && cameraPos.y > hitBoxHolder[i].bottomRight[1]) {
		//	return true;
		//}
		//else if (cameraPos.x > hitBoxHolder[i].bottomLeft[1] && cameraPos.x < hitBoxHolder[i].topRight[1]) {
		//	return true;
		//}
	}
	return false;
}
