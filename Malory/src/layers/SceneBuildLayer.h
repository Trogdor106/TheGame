#pragma once
#include "florp/app/ApplicationLayer.h"
#include "florp/game/SceneManager.h"
#include "florp/game/RenderableComponent.h"
#include <florp\graphics\MeshData.h>
#include <florp\graphics\MeshBuilder.h>
#include <florp\graphics\ObjLoader.h>
#include <florp\game\Transform.h>
#include "RotateBehaviour.h"
#include "CameraComponent.h"
#include "florp/app/Application.h"
#include <ControlBehaviour.h>
#include <ShadowLight.h>
#include "PointLightComponent.h"
#include "LightFlickerBehaviour.h"
#include "Instrumentation.h"
#include "Texture3D.h"
#include "HitBoxes.h"

class SceneBuilder : public florp::app::ApplicationLayer {
public:
	void Initialize() override;

	void textureLoader();
	void meshLoader();
private:
	const float PI = 3.1415926535897932384626433832795028841971693993;
	florp::graphics::Shader::Sptr emissiveShader;
	florp::graphics::Shader::Sptr shader;
	florp::graphics::Shader::Sptr displacementmap;
	std::vector <florp::game::Material::Sptr> materialHolder;
	std::vector <float> materialToUse;
	std::vector <florp::graphics::MeshData> meshHolder;
	std::vector <float> meshToUse;
	std::vector <float> floorToUse;

	std::vector <glm::mat4> genTransform;

	HitBox tempHit;
};
