#include "HitBoxes.h"

HitBoxes::HitBoxes() : currentFloor(0) {
	HitBox temp = { {0.0}, {0.0}, {0.0}, {0.0}, -1, -1 };
	hitBoxHolderFirst.push_back(temp);
	hitBoxHolderSecond.push_back(temp);
	hitBoxHolderThird.push_back(temp);
	OGHitBoxHolderFirst.push_back(temp);
	OGHitBoxHolderSecond.push_back(temp);
	OGHitBoxHolderThird.push_back(temp);

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

void HitBoxes::saveHitBoxes(HitBox boxToSave, int floor)
{
	switch (floor) {
	case 0:
			hitBoxHolderFirst.push_back(boxToSave);
			OGHitBoxHolderFirst.push_back(boxToSave);
		break;
	case 1:
		
			hitBoxHolderSecond.push_back(boxToSave);
			OGHitBoxHolderSecond.push_back(boxToSave);
		break;
	case 2:
			hitBoxHolderThird.push_back(boxToSave);
			OGHitBoxHolderThird.push_back(boxToSave);
		break;

	}
	//hitBoxHolder.size();

}

void HitBoxes::deleteHitBox(int id)
{
	//Doesn't work for some reason
	//for (int i = 0; i < hitBoxHolder.size(); i++) {
	//	if (hitBoxHolder[i].ID == id) {
	//		hitBoxHolder.erase(hitBoxHolder.begin() + i);
	//		OGHitBoxHolder.erase(OGHitBoxHolder.begin() + i);
	//	}
	//}
}

void HitBoxes::updateHitBox(int id, glm::mat4 changes)
{
	switch (currentFloor) {
	case 0:
		for (int i = 0; i < hitBoxHolderFirst.size(); i++) {



			if (hitBoxHolderFirst[i].ID == id) {
				if (abs(1 - changes[0].y) < 0.1) {

					hitBoxHolderFirst[i].centerPoint.x = OGHitBoxHolderFirst[i].centerPoint.x + changes[3][0];
					hitBoxHolderFirst[i].centerPoint.y = OGHitBoxHolderFirst[i].centerPoint.y + changes[3][1];
					hitBoxHolderFirst[i].Bottom = hitBoxHolderFirst[i].centerPoint.y + OGHitBoxHolderFirst[i].Left;
					hitBoxHolderFirst[i].Right = hitBoxHolderFirst[i].centerPoint.x - OGHitBoxHolderFirst[i].Bottom;
					hitBoxHolderFirst[i].top = hitBoxHolderFirst[i].centerPoint.y + OGHitBoxHolderFirst[i].Right;
					hitBoxHolderFirst[i].Left = hitBoxHolderFirst[i].centerPoint.x - OGHitBoxHolderFirst[i].top;

				}
				else {

					hitBoxHolderFirst[i].centerPoint.x = OGHitBoxHolderFirst[i].centerPoint.x + changes[3][0];
					hitBoxHolderFirst[i].centerPoint.y = OGHitBoxHolderFirst[i].centerPoint.y + changes[3][1];
					hitBoxHolderFirst[i].Bottom = hitBoxHolderFirst[i].centerPoint.y + OGHitBoxHolderFirst[i].Bottom;
					hitBoxHolderFirst[i].top = hitBoxHolderFirst[i].centerPoint.y + OGHitBoxHolderFirst[i].top;
					hitBoxHolderFirst[i].Left = hitBoxHolderFirst[i].centerPoint.x + OGHitBoxHolderFirst[i].Left;
					hitBoxHolderFirst[i].Right = hitBoxHolderFirst[i].centerPoint.x + OGHitBoxHolderFirst[i].Right;

				}

			}
		}
		break;
	case 1:
		for (int i = 0; i < hitBoxHolderSecond.size(); i++) {
			if (hitBoxHolderSecond[i].ID == id) {
				hitBoxHolderSecond[i].centerPoint.x = OGHitBoxHolderSecond[i].centerPoint.x + changes[3][0];
				hitBoxHolderSecond[i].centerPoint.y = OGHitBoxHolderSecond[i].centerPoint.y + changes[3][1];
				hitBoxHolderSecond[i].Bottom = hitBoxHolderSecond[i].centerPoint.y + OGHitBoxHolderSecond[i].Bottom;
				hitBoxHolderSecond[i].top = hitBoxHolderSecond[i].centerPoint.y + OGHitBoxHolderSecond[i].top;
				hitBoxHolderSecond[i].Left = hitBoxHolderSecond[i].centerPoint.x + OGHitBoxHolderSecond[i].Left;
				hitBoxHolderSecond[i].Right = hitBoxHolderSecond[i].centerPoint.x + OGHitBoxHolderSecond[i].Right;
			}
		}
		break;
	case 2:
		for (int i = 0; i < hitBoxHolderThird.size(); i++) {
			if (hitBoxHolderThird[i].ID == id) {
				hitBoxHolderThird[i].centerPoint.x = OGHitBoxHolderThird[i].centerPoint.x + changes[3][0];
				hitBoxHolderThird[i].centerPoint.y = OGHitBoxHolderThird[i].centerPoint.y + changes[3][1];
				hitBoxHolderThird[i].Bottom = hitBoxHolderThird[i].centerPoint.y + OGHitBoxHolderThird[i].Bottom;
				hitBoxHolderThird[i].top = hitBoxHolderThird[i].centerPoint.y + OGHitBoxHolderThird[i].top;
				hitBoxHolderThird[i].Left = hitBoxHolderThird[i].centerPoint.x + OGHitBoxHolderThird[i].Left;
				hitBoxHolderThird[i].Right = hitBoxHolderThird[i].centerPoint.x + OGHitBoxHolderThird[i].Right;
			}
		}
		break;
	}
}

void HitBoxes::updateHitBox(int id, float rotation)
{
	//for (int i = 0; i < hitBoxHolder.size(); i++) {
	//	if (hitBoxHolder[i].ID == id) {
	//		//Maybe add actual rotation with angle for angled objects at some point (Not in class since I cannot think about math right now)
	//		//Need to fix, this doesn't actually work right now
	//		if (rotation == 90) {
	//			hitBoxHolder[i].Left = OGHitBoxHolder[i].Bottom;
	//			hitBoxHolder[i].Right = OGHitBoxHolder[i].top;
	//			hitBoxHolder[i].top = OGHitBoxHolder[i].Right;
	//			hitBoxHolder[i].Bottom = OGHitBoxHolder[i].Left;
	//		}
	//		else if (rotation == 180) {
	//			hitBoxHolder[i].Left = OGHitBoxHolder[i].Right;
	//			hitBoxHolder[i].Right = OGHitBoxHolder[i].Left;
	//			hitBoxHolder[i].top = OGHitBoxHolder[i].Bottom;
	//			hitBoxHolder[i].Bottom = OGHitBoxHolder[i].top;
	//		}
	//		else if (rotation == 270) {
	//			hitBoxHolder[i].Left = OGHitBoxHolder[i].top;
	//			hitBoxHolder[i].Right = OGHitBoxHolder[i].Bottom;
	//			hitBoxHolder[i].top = OGHitBoxHolder[i].Left;
	//			hitBoxHolder[i].Bottom = OGHitBoxHolder[i].Right;
	//		}
	//		else if (rotation == -90) {
	//
	//			hitBoxHolder[i].Left = OGHitBoxHolder[i].top;
	//			hitBoxHolder[i].Right = OGHitBoxHolder[i].Bottom;
	//			hitBoxHolder[i].top = OGHitBoxHolder[i].Left;
	//			hitBoxHolder[i].Bottom = OGHitBoxHolder[i].Right;
	//		}
	//		else if (rotation == -180) {
	//			hitBoxHolder[i].Left = OGHitBoxHolder[i].Right;
	//			hitBoxHolder[i].Right = OGHitBoxHolder[i].Left;
	//			hitBoxHolder[i].top = OGHitBoxHolder[i].Bottom;
	//			hitBoxHolder[i].Bottom = OGHitBoxHolder[i].top;
	//		}
	//		else if (rotation == -270) {
	//			hitBoxHolder[i].Left = OGHitBoxHolder[i].Bottom;
	//			hitBoxHolder[i].Right = OGHitBoxHolder[i].top;
	//			hitBoxHolder[i].top = OGHitBoxHolder[i].Right;
	//			hitBoxHolder[i].Bottom = OGHitBoxHolder[i].Left;
	//		}
	//	}
	//}
	//Do this ^
}

bool HitBoxes::isInHitBox(glm::vec3 cameraPos, glm::vec3 cameraPosGoal)
{
	//Innifecient to do it like this but it's simpler

	switch (currentFloor) {
	case 0:
		for (int i = 0; i < hitBoxHolderFirst.size(); i++) {
			if (cameraPosGoal.y >= hitBoxHolderFirst[i].Bottom && cameraPosGoal.y <= hitBoxHolderFirst[i].top) {
				if (cameraPosGoal.x >= hitBoxHolderFirst[i].Left && cameraPosGoal.x <= hitBoxHolderFirst[i].Right) {
					return true;
				}
			}
		}
		break;
	case 1:
		for (int i = 0; i < hitBoxHolderSecond.size(); i++) {
			if (cameraPosGoal.y >= hitBoxHolderSecond[i].Bottom && cameraPosGoal.y <= hitBoxHolderSecond[i].top) {
				if (cameraPosGoal.x >= hitBoxHolderSecond[i].Left && cameraPosGoal.x <= hitBoxHolderSecond[i].Right) {
					return true;
				}
			}
		}
		break;
	case 2:
		for (int i = 0; i < hitBoxHolderThird.size(); i++) {
			if (cameraPosGoal.y >= hitBoxHolderThird[i].Bottom && cameraPosGoal.y <= hitBoxHolderThird[i].top) {
				if (cameraPosGoal.x >= hitBoxHolderThird[i].Left && cameraPosGoal.x <= hitBoxHolderThird[i].Right) {
					return true;
				}
			}
		}
		break;

	}

	return false;
}

void HitBoxes::saveFloor(int floor)
{
	currentFloor = floor;
}

int HitBoxes::getFloor()
{
	return currentFloor;
}


