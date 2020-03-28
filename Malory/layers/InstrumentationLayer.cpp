#include "InstrumentationLayer.h"
#include "florp/app/Application.h"

void InstrumentationPreLayer::Initialize() {	
	ProfilingSession::Start("profiling-init.json");
	PostLayer->_Timers[0].Restart();

	florp::app::Application::Get()->GetWindow()->SetSwapInterval(0);
}

void InstrumentationPostLayer::Initialize() {
	_Timers[0].Stop();
	ProfilingSession::End();
	ProfilingSession::Start("profiling-loop.json");
}

void InstrumentationPreLayer::OnSceneEnter() {
	PostLayer->_Timers[1].Restart();
}
void InstrumentationPostLayer::OnSceneEnter() {
	_Timers[1].Stop();
}

void InstrumentationPreLayer::OnSceneExit() {
	PostLayer->_Timers[2].Restart();
}
void InstrumentationPostLayer::OnSceneExit() {
	_Timers[2].Stop();
}

void InstrumentationPreLayer::Update()
{
	PostLayer->_Timers[3].Restart();
}
void InstrumentationPostLayer::Update() {
	_Timers[3].Stop();
}

void InstrumentationPreLayer::LateUpdate() {
	PostLayer->_Timers[4].Restart();
}
void InstrumentationPostLayer::LateUpdate() {
	_Timers[4].Stop();
}

void InstrumentationPreLayer::FixedUpdate() {
	PostLayer->_Timers[5].Restart();
}

void InstrumentationPostLayer::FixedUpdate() {
	_Timers[5].Stop();
}

void InstrumentationPreLayer::Render()
{
	PostLayer->_Timers[6].Restart();
}
void InstrumentationPostLayer::Render() {
	_Timers[6].Stop();
}

void InstrumentationPreLayer::PostRender() {
	PostLayer->_Timers[7].Restart();
}
void InstrumentationPostLayer::PostRender() {
	_Timers[7].Stop();
}


void InstrumentationPreLayer::PreRender() {
	PostLayer->_Timers[12].Restart();
	PostLayer->_Timers[8].Restart();
}
void InstrumentationPostLayer::PreRender() {
	_Timers[8].Stop();
}

void InstrumentationPreLayer::RenderGUI() {
	PostLayer->_Timers[9].Restart();
}
void InstrumentationPostLayer::RenderGUI() {
	_Timers[9].Stop();
}

void InstrumentationPreLayer::PreGUI() {
	PostLayer->_Timers[10].Restart();
}
void InstrumentationPostLayer::PreGUI() {
	_Timers[10].Stop();
}

void InstrumentationPreLayer::PostGUI() {
	PostLayer->_Timers[11].Restart();
}
void InstrumentationPostLayer::PostGUI() {
	_Timers[11].Stop();
	_Timers[12].Stop();
}
void InstrumentationPostLayer::Shutdown() {
	ProfilingSession::End();
}
