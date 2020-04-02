#pragma once
#include "florp/app/ApplicationLayer.h"
#include <string>
#include <vector>

class AudioLayer : public florp::app::ApplicationLayer
{
public:
	void Initialize() override;
	void Shutdown() override;
	void Update() override;
	
};
