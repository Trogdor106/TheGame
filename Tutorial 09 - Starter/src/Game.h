#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>

#include "GLM/glm.hpp"

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

class Game {
public:
	Game();
	~Game();

	void Run();
	void Resize(int newWidth, int newHeight);

protected:
	void Initialize();
	void Shutdown();

	void LoadContent();
	void UnloadContent();

	void InitImGui();
	void ShutdownImGui();

	void ImGuiNewFrame();
	void ImGuiEndFrame();

	void Update(float deltaTime);
	void Draw(float deltaTime);
	void DrawGui(float deltaTime);

private:
	// Stores the main window that the game is running in
	GLFWwindow* myWindow;
	// Stores the clear color of the game's window
	glm::vec4   myClearColor;
	// Stores the title of the game's window
	char        myWindowTitle[32];

	Camera::Sptr myCamera;
	// A shared pointer to our mesh
	Mesh::Sptr   myMesh;
	Mesh::Sptr   myMesh2;
	Mesh::Sptr   myMesh3;
	// A shared pointer to our shader
	Shader::Sptr myShader;
	// Shader for viewing normal maps
	Shader::Sptr myNormalShader;

	// Our models transformation matrix
	glm::mat4   myModelTransform;

	glm::ivec2 myWindowSize;
	void __RenderScene(glm::ivec4 viewport, Camera::Sptr camera);
};