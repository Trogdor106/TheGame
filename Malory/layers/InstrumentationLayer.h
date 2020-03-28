#pragma once
#include "florp/app/ApplicationLayer.h"
#include "Instrumentation.h"

class InstrumentationPreLayer; // Pre-declare the pre layer so that post layer can access it

class InstrumentationPostLayer : public florp::app::ApplicationLayer
{
public:
	InstrumentationPostLayer() = default;
	void Initialize() override;
	void Shutdown() override;
	void OnSceneEnter() override;
	void OnSceneExit() override;
	void Update() override;
	void LateUpdate() override;
	void FixedUpdate() override;
	void Render() override;
	void PostRender() override;
	void PreRender() override;
	void RenderGUI() override;
	void PreGUI() override;
	void PostGUI() override;

protected:
	friend class InstrumentationPreLayer;

	ProfileTimer _Timers[13] = {
		{"Initialization", false},
		{"SceneEnter", false},
		{"SceneExit", false},
		{"Update", false},
		{"LateUpdate", false},
		{"FixedUpdate", false},
		{"Render", false},
		{"PostRender", false},
		{"PreRender", false},
		{"RenderGUI", false},
		{"PreGUI", false},
		{"PostGUI", false},
		{"Frame", false}
	};
};

class InstrumentationPreLayer : public florp::app::ApplicationLayer
{
public:
	InstrumentationPreLayer() = default;
	
	void Initialize() override;
	void OnSceneEnter() override;
	void OnSceneExit() override;
	void Update() override;
	void LateUpdate() override;
	void FixedUpdate() override;
	void Render() override;
	void PostRender() override;
	void PreRender() override;
	void RenderGUI() override;
	void PreGUI() override;
	void PostGUI() override;

	InstrumentationPostLayer* PostLayer;

};
