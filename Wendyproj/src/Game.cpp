#pragma once
#include "Game.h"


#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "imgui_impl_opengl3.cpp"
#include "imgui_impl_glfw.cpp"

/*
	Handles debug messages from OpenGL
	https://www.khronos.org/opengl/wiki/Debug_Output#Message_Components
	@param source    Which part of OpenGL dispatched the message
	@param type      The type of message (ex: error, performance issues, deprecated behavior)
	@param id        The ID of the error or message (to distinguish between different types of errors, like nullref or index out of range)
	@param severity  The severity of the message (from High to Notification)
	@param length    The length of the message
	@param message   The human readable message from OpenGL
	@param userParam The pointer we set with glDebugMessageCallback (should be the game pointer)
*/

void lerp(glm::vec3& goal, glm::vec3& startPoint, glm::vec3& currentPos, float duration) {
	static float totalTime = duration;
	static float amountOfTimes = totalTime / (0.009025);
	static float percentagePerIntervals = (0.009025) / totalTime;
	static float currentInterval = 0;
	if (currentInterval <= amountOfTimes && currentInterval >= 0) {
		float t = startPoint.x != goal.x ? (((percentagePerIntervals * currentInterval) * (goal.x - startPoint.x) + startPoint.x) - startPoint.x) / (goal.x - startPoint.x) : 1;
		currentPos.x = (1 - t) * startPoint.x + t * goal.x;
		float t2 = startPoint.y != goal.y ? (((percentagePerIntervals * currentInterval) * (goal.y - startPoint.y) + startPoint.y) - startPoint.y) / (goal.y - startPoint.y) : 1;
		currentPos.y = (1 - t2) * startPoint.y + t2 * goal.y;
		float t3 = startPoint.z != goal.z ? (((percentagePerIntervals * currentInterval) * (goal.z - startPoint.z) + startPoint.z) - startPoint.z) / (goal.z - startPoint.z) : 1;
		currentPos.z = (1 - t3) * startPoint.z + t3 * goal.z;

		currentInterval += 1;

		//All the 0.095s are because delta time was giving us varying results based on the computers, so this constant works better with smaller numbers
	}
}





void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:          LOG_INFO(message); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN(message); break;
	case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR(message); break;
#ifdef LOG_GL_NOTIFICATIONS
	case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO(message); break;
#endif
	default: break;
	}
}

void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	Game* game = (Game*)glfwGetWindowUserPointer(window);
	if (game) {
		game->Resize(width, height);
	}
}

void Game::Resize(int newWidth, int newHeight) {
	myCamera->Projection = glm::perspective(glm::radians(60.0f), newWidth / (float)newHeight, 0.01f, 1000.0f);
}

Game::Game() :
	myWindow(nullptr),
	myWindowTitle("Mallory"),
	myClearColor(glm::vec4(0, 0, 0, 1))
	//genTransform()
{ }

Game::~Game() { }

void Game::Run()
{
	Initialize();

	gBufferInit();
	
	InitImGui();
	LoadSimpleContent();
	LoadContent();
	loadMusic();

	static float prevFrame = glfwGetTime();

	// Run as long as the window is open
	while (!glfwWindowShouldClose(myWindow)) {
		// Poll for events from windows (clicks, keypressed, closing, all that)
		glfwPollEvents();
		alwaysActiveKeys();

		float thisFrame = glfwGetTime();
		float deltaTime = thisFrame - prevFrame;

		switch (gameState) {
		case 0:
			Update(deltaTime);
			Draw(deltaTime);
			break;
		case 1:
			//Draw(deltaTime);

			pauseScreen(); //Also need start and end screen
			break;
		case 2:
			break;
		}
		playSound();
		updateMusic();
		preRender();
		gBuffer();



		//ImGuiNewFrame();
		//DrawGui(deltaTime);
		//ImGuiEndFrame();

		// Store this frames time for the next go around
		prevFrame = thisFrame;

		// Present our image to windows
		glfwSwapBuffers(myWindow);
	}

	LOG_INFO("Shutting down...");

	shutDownSound();
	UnloadContent();
	//UnloadGBuffer();
	ShutdownImGui();
	Shutdown();
}

void Game::Initialize() {
	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		throw std::runtime_error("Failed to initialize GLFW");
	}

	// Enable transparent backbuffers for our windows (note that Windows expects our colors to be pre-multiplied with alpha)
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

	// Create a new GLFW window
	myWindow = glfwCreateWindow(windowSizeWidth, windowSizeHeight, myWindowTitle, nullptr, nullptr);
	glfwMaximizeWindow(myWindow);
	
	// Tie our game to our window, so we can access it via callbacks
	glfwSetWindowUserPointer(myWindow, this);

	// Set our window resized callback
	glfwSetWindowSizeCallback(myWindow, GlfwWindowResizedCallback);

	// We want GL commands to be executed for our window, so we make our window's context the current one
	glfwMakeContextCurrent(myWindow);

	// Let glad know what function loader we are using (will call gl commands via glfw)
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		throw std::runtime_error("Failed to initialize GLAD");
	}

	// Log our renderer and OpenGL version
	LOG_INFO(glGetString(GL_RENDERER));
	LOG_INFO(glGetString(GL_VERSION));

	// Enable debugging, and route messages to our callback
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GlDebugMessage, this);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	
	
}

void Game::Shutdown() {
	glfwTerminate();
}


glm::vec4 testColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

void Game::CreateObjects(int objectNameID, int typeOfObject, int textureNameID, int floor, glm::mat4 transformation)
{
	if (typeOfObject == 100 /* Put the list of objects you want to be collidable here*/|| typeOfObject == 3) {
		saveHitAndMesh(ObjLoader::LoadObj2(filename[objectNameID], glm::vec4(1.0f)), floor);
	}
	else {
		genObjects.push_back(ObjLoader::LoadObj(filename[objectNameID], glm::vec4(1.0f)));
	}
	objFloor.push_back(floor);
	amountOfObjects.push_back(typeOfObject);  
	genMats.push_back(Texture2D::LoadFromFile(texturename[textureNameID]));
	genTransform.push_back(transformation);

}

void Game::saveHitAndMesh(MeshAndHitBox toSave, int floor) {
	toSave.hitBox.ID = amountOfObjects.size();
	genObjects.push_back(toSave.mesh);
	hitBoxManager.saveHitBoxes(toSave.hitBox, floor);
}

void Game::alwaysActiveKeys()
{
	if (glfwGetKey(myWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (gameState == 0) {
			gameState = 1;
		}
		else if (gameState == 1) {
			gameState = 0;
		}
	}
}

void Game::UnloadContent() {

}


void Game::InitImGui() {
	//// Creates a new ImGUI context
	//ImGui::CreateContext();
	//// Gets our ImGUI input/output 
	//ImGuiIO& io = ImGui::GetIO();
	//// Enable keyboard navigation
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//// Allow docking to our window
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//// Allow multiple viewports (so we can drag ImGui off our window)
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//// Allow our viewports to use transparent backbuffers
	//io.ConfigFlags |= ImGuiConfigFlags_TransparentBackbuffers;
	//
	//// Set up the ImGui implementation for OpenGL
	//ImGui_ImplGlfw_InitForOpenGL(myWindow, true);
	//ImGui_ImplOpenGL3_Init("#version 410");
	//
	//// Dark mode FTW
	//ImGui::StyleColorsDark();
	//
	//// Get our imgui style
	//ImGuiStyle& style = ImGui::GetStyle();
	////style.Alpha = 1.0f;
	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
	//	style.WindowRounding = 0.0f;
	//	style.Colors[ImGuiCol_WindowBg].w = 0.8f;
	//}
}
void Game::ShutdownImGui() {
	//// Cleanup the ImGui implementation
	//ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	//// Destroy our ImGui context
	//ImGui::DestroyContext();
}
void Game::ImGuiNewFrame() {
	//// Implementation new frame
	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplGlfw_NewFrame();
	//// ImGui context new frame
	//ImGui::NewFrame();
}
void Game::ImGuiEndFrame() {
	//// Make sure ImGui knows how big our window is
	//ImGuiIO& io = ImGui::GetIO();
	//int width{ 0 }, height{ 0 };
	//glfwGetWindowSize(myWindow, &width, &height);
	//io.DisplaySize = ImVec2(width, height);

	//// Render all of our ImGui elements
	//ImGui::Render();
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//
	//// If we have multiple viewports enabled (can drag into a new window)
	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
	//	// Update the windows that ImGui is using
	//	ImGui::UpdatePlatformWindows();
	//	ImGui::RenderPlatformWindowsDefault();
	//	// Restore our gl context
	//	glfwMakeContextCurrent(myWindow);
	//}
}



void Game::Draw(float deltaTime) {
	static float time = 0.0;
	// Clear our screen every frame
	glClearColor(myClearColor.x, myClearColor.y, myClearColor.z, myClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//myShader->Bind();
	//myShader->SetUniform("a_ModelViewProjection", interactCamera->GetViewProjection());
	//myMesh->Draw();

	// We'll grab a reference to the ecs to make things easier   
	auto& ecs = CurrentRegistry();

	// We sort our mesh renderers based on material properties   
	// This will group all of our meshes based on shader first, then material second   
	ecs.sort<MeshRenderer>([](const MeshRenderer& lhs, const MeshRenderer& rhs) {
		if (rhs.Material == nullptr || rhs.Mesh == nullptr)
			return false;
		else if (lhs.Material == nullptr || lhs.Mesh == nullptr)
			return true;
		else if (lhs.Material->GetShader() != rhs.Material->GetShader())
			return lhs.Material->GetShader() < rhs.Material->GetShader();
		else
			return lhs.Material < rhs.Material;
		});

	// These will keep track of the current shader and material that we have bound    
	Material::Sptr mat = nullptr;
	Shader::Sptr   boundShader = nullptr;
	// A view will let us iterate over all of our entities that have the given component types    
	auto view = ecs.view<MeshRenderer>();
	int object = genObjects.size() - 1;


	for (const auto& entity : view) {
		if (objFloor[object] == currentFloor) { //Only draw the objects on the floor we are on, because why would we draw something we don't see?
			// Get our shader    
			const MeshRenderer& renderer = ecs.get<MeshRenderer>(entity);
			// Early bail if mesh is invalid 
			if (renderer.Mesh == nullptr || renderer.Material == nullptr)
				continue;

			//renderer.Mesh = Bake(genObjects[hello]);
			if (amountOfObjects[object] == 100 || amountOfObjects[object] == 3 || amountOfObjects[object] == 1) {
				hitBoxManager.updateHitBox(object, genTransform[object]);
			}

			// If our shader has changed, we need to bind it and update our frame-level uniforms    
			if (renderer.Material->GetShader() != boundShader) {
				boundShader = renderer.Material->GetShader();
				boundShader->Bind();
				//boundShader->SetUniform("a_CameraPos", interactCamera->GetPosition());
				boundShader->SetUniform("a_CameraPos", myCamera->GetPosition());
			}



			//testMat->Set("s_Albedo", genMats[hello]);
			testMat2->Set("s_Albedo", genMats[object]);
			//if (renderer.Material != mat) {
			mat = renderer.Material;
			mat->Apply();
			//}


			//}
			// We'll need some info about the entities position in the world 
			const TempTransform& transform = ecs.get_or_assign<TempTransform>(entity);
			// Our normal matrix is the inverse-transpose of our object's world rotation
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform.GetWorldTransform())));
			// Update the MVP using the item's transform  

			if (amountOfObjects[object] == 3) {
				//testMat->Set("time", time);
				mat->GetShader()->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() * genTransform[object]);
				//mat->GetShader()->SetUniform("a_ModelViewProjection", interactCamera->GetViewProjection() * genTransform[hello]);
				// Update the model matrix to the item's world transform
				mat->GetShader()->SetUniform("a_Model", transform.GetWorldTransform());
				// Update the model matrix to the item's world transform 
				mat->GetShader()->SetUniform("a_NormalMatrix", normalMatrix);
				// Draw the item   
				renderer.Mesh->Draw();
			}
			else if (amountOfObjects[object] == 4) {
				if (isPickedUp && isDoneReading == false) {
					//mat->GetShader()->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() * genTransform[hello]);
					mat->GetShader()->SetUniform("a_ModelViewProjection", interactCamera->GetViewProjection() * genTransform[object]);


					// Update the model matrix to the item's world transform
					mat->GetShader()->SetUniform("a_Model", transform.GetWorldTransform());
					// Update the model matrix to the item's world transform 
					mat->GetShader()->SetUniform("a_NormalMatrix", normalMatrix);
					// Draw the item   
					renderer.Mesh->Draw();

				}
				else if (isDoneReading == false) {
					mat->GetShader()->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() * genTransform[object]);
					//mat->GetShader()->SetUniform("a_ModelViewProjection", interactCamera->GetViewProjection() * genTransform[hello]);


					// Update the model matrix to the item's world transform
					mat->GetShader()->SetUniform("a_Model", transform.GetWorldTransform());
					// Update the model matrix to the item's world transform 
					mat->GetShader()->SetUniform("a_NormalMatrix", normalMatrix);
					// Draw the item   
					renderer.Mesh->Draw();
				}
			}
			else {
				boundShader->SetUniform("a_ModelViewProjection", myCamera->GetViewProjection() * genTransform[object]);
				//mat->GetShader()->SetUniform("a_ModelViewProjection", interactCamera->GetViewProjection() * genTransform[hello]);


				// Update the model matrix to the item's world transform
				boundShader->SetUniform("a_Model", transform.GetWorldTransform());
				// Update the model matrix to the item's world transform 
				boundShader->SetUniform("a_NormalMatrix", normalMatrix);
				// Draw the item   
				renderer.Mesh->Draw();
			}
		}
		object--;
	}
	time += 0.02;
}

void Game::DrawGui(float deltaTime) {
	//// Open a new ImGui window
	//static bool testOpen = true;
	//ImGui::Begin("Test", &testOpen, ImVec2(300, 200));
	//// Draw a color editor
	//auto it = SceneManager::Each();
	//for (auto& kvp : it) {
	//	if (ImGui::Button(kvp.first.c_str())) {
	//		SceneManager::SetCurrentScene(kvp.first);
	//	}
	//}
	//
	//
	//ImGui::ColorEdit4("Clear Color", &myClearColor[0]);
	//// Check if a textbox has changed, and update our window title if it has
	//if (ImGui::InputText("Window Title", myWindowTitle, 32)) {
	//	glfwSetWindowTitle(myWindow, myWindowTitle);
	//}
	//// Our object's test color
	//ImGui::ColorEdit4("Object Color", &testColor[0]);
	//ImGui::End();
	//
	//// Open a second ImGui window
	//ImGui::Begin("Debug");
	//// Draw a formatted text line
	//ImGui::Text("Time: %f", glfwGetTime());
	//
	//// Start a new ImGui header for our camera settings
	//if (ImGui::CollapsingHeader("Camera Settings")) {
	//	// Draw our camera's normal
	//	glm::vec3 camNormal = myCamera->GetForward();
	//	ImGui::DragFloat3("Normal", &camNormal[0]);
	//
	//	// Get the camera's position so we can edit it
	//	glm::vec3 position = myCamera->GetPosition();
	//	// Draw an editor control for the position, and update camera position
	//	if (ImGui::DragFloat3("Position", &position[0])) {
	//		myCamera->SetPosition(position);
	//	}
	//	if (ImGui::Button("Look at center")) {
	//		myCamera->LookAt(glm::vec3(0), glm::vec3(0, 0, 1));
	//	}
	//	// Get the camera pinning value
	//	static glm::vec3 camPin;
	//
	//	// Get whether or not camera pinning is enabled
	//	bool camPlaneEnabled = myCamera->GetPinnedUp().has_value();
	//	// Draw a checkbox for camera pinning
	//	if (ImGui::Checkbox("Pinning Enabled", &camPlaneEnabled)) {
	//		// If we've disabled pinning, cache our pinning vector and remove it
	//		if (!camPlaneEnabled) {
	//			camPin = myCamera->GetPinnedUp().value();
	//			myCamera->SetPinnedUp(std::optional<glm::vec3>());
	//		}
	//		// Set our camera's pinning vector to our cached value
	//		else {
	//			myCamera->SetPinnedUp(camPin);
	//		}
	//	}
	//	// If we have enabled pinning
	//	if (camPlaneEnabled) {
	//		// Draw a slider for our camera pin direction
	//		if (ImGui::InputFloat3("Pin Direction", &camPin[0])) {
	//			myCamera->SetPinnedUp(camPin);
	//		}
	//	}
	//}
	//ImGui::End();
}

bool Game::interactionIsPossible(glm::vec3 playerPos, glm::vec3 objectPos)
{
	float distanceX = abs((playerPos.x) - (objectPos.x));

	float distanceY = abs((playerPos.y) - (objectPos.y));
	if (distanceX < 10 && distanceY < 10) {
		return true;
	}
	return false;
}

void Game::pauseScreen()
{
	glClearColor(myClearColor.x, myClearColor.y, myClearColor.z, myClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);

	simpleShader->Bind();
	mySimpleMesh->Drawmini();
}

void Game::gBufferInit()
{
	glfwGetWindowSize(myWindow, &windowSizeWidth, &windowSizeHeight);
	//Create shader and initialise some values (Not all)
	gBufferShader = std::make_shared<Shader>();
	//gBufferShader->Load("post.vs.glsl", "post.fs.glsl");
	gBufferShader->LoadPart(ShaderStageType::VertexShader, "Shaders/PostLayers/post.vs.glsl");
	gBufferShader->LoadPart(ShaderStageType::FragmentShader, "Shaders/PostLayers/post.fs.glsl");
	gBufferShader->Link();

	RenderBufferDesc mainColor = RenderBufferDesc();
	mainColor.ShaderReadable = true;
	mainColor.Attachment = RenderTargetAttachment::Color0;
	mainColor.Format = RenderTargetType::ColorRgb16F;
	
	// We'll use one buffer to accumulate all the lighting
	myAccumulationBuffer = std::make_shared<FrameBuffer>(windowSizeWidth, windowSizeHeight);
	myAccumulationBuffer->AddAttachment(mainColor);
	myAccumulationBuffer->Validate();
	myAccumulationBuffer->SetDebugName("Intermediate");

	// Create our fullscreen quad (just like in PostLayer)
	{
		float vert[] = {
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		uint32_t indices[] = {
			0, 1, 2,
			1, 3, 2
		};
		BufferLayout layout = {
			{ "inPosition", ShaderDataType::Float2 },
			{ "inUV",       ShaderDataType::Float2 }
		};
	
		myFullscreenQuad = std::make_shared<Mesh>(vert, 4, layout, indices, 6);
	}

}

void Game::preRender()
{
	auto& ecs = CurrentRegistry();
	
	// We'll only handle stuff if we actually have a shadow casting light in the scene
	auto view = ecs.view<PointLight>();
	if (view.size() > 0) {
		// We'll make sure depth testing and culling are enabled
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT); // enable front face culling
	
		// Iterate over all the shadow casting lights
		Shader::Sptr shader = nullptr;
		shader = gBufferShader;
		ecs.view<PointLight>().each([&](auto entity, PointLight& light) {
			// Get the light's transform
			const TempTransform& lightTransform = ecs.get<TempTransform>(entity);
			// Use the shader, and tell it what our output resolution is
			shader->Use();
			//shader->SetUniform("a_OutputResolution", (glm::vec2)light.Attenuation);
	
			// Bind, viewport, and clear
			myAccumulationBuffer->Bind();
			glViewport(0, 0, myAccumulationBuffer->GetWidth(), myAccumulationBuffer->GetHeight());
			glClear(GL_DEPTH_BUFFER_BIT);
	
			// Determine the position and matrices for the light
			//glm::vec3 position = lightTransform.GetLocalPosition();
			//glm::mat4 viewMatrix = glm::inverse(lightTransform.GetWorldTransform());
			//glm::mat4 viewProjection = * viewMatrix;
	
			// We're going to iterate over every renderable component
			auto view = ecs.view<MeshRenderer>();
	
			for (const auto& entity : view) {
				// Get our shader
				const MeshRenderer& renderer = ecs.get<MeshRenderer>(entity);
	
				// Early bail if mesh is invalid (or if if does not cast a shadow)
				if (renderer.Mesh == nullptr || renderer.Material == nullptr)
					continue;
	
				// We'll need some info about the entities position in the world
				const TempTransform& transform = ecs.get_or_assign<TempTransform>(entity);
	
				// Update the MVP using the item's transform
				shader->SetUniform(
					"a_ModelViewProjection",
					transform.GetWorldTransform());
	
				// Draw the item
				renderer.Mesh->Draw();
			}
	
			// Unbind so that we can use the texture later
			myAccumulationBuffer->UnBind();
			});
	
		glCullFace(GL_BACK); // enable back face culling
	}
}

void Game::gBuffer()
{
	// We grab the application singleton to get the size of the screen
	//florp::app::Application* app = florp::app::Application::Get();
	//auto& ecs = CurrentRegistry();

	// We'll get the back buffer from the frame state
	

	//Always crashes here, Don't know why
	//AppFrameState& state = ecs.ctx<AppFrameState>();
	//state.Current = ecs.ctx<AppFrameState>();// = ecs.<>(); 
	//FrameBuffer::Sptr mainBuffer = ecs.ctx<AppFrameState>().Current.Output;


	/*
	ecs.sort<MeshRenderer>([](const MeshRenderer& lhs, const MeshRenderer& rhs) {
		if (rhs.Material == nullptr || rhs.Mesh == nullptr)
			return false;
		else if (lhs.Material == nullptr || lhs.Mesh == nullptr)
			return true;
		else if (lhs.Material->GetShader() != rhs.Material->GetShader())
			return lhs.Material->GetShader() < rhs.Material->GetShader();
		else
			return lhs.Material < rhs.Material;
		});

	// These will keep track of the current shader and material that we have bound    
	Material::Sptr mat = nullptr;
	Shader::Sptr   boundShader = nullptr;
	// A view will let us iterate over all of our entities that have the given component types    
	auto view = ecs.view<MeshRenderer>();
	int object = genObjects.size() - 1;
	
	



	// We can extract our near and far plane by reversing the projection calculation
	float m22 = state.Current.Projection[2][2];
	float m32 = state.Current.Projection[3][2];
	float nearPlane = (2.0f * m32) / (2.0f * m22 - 2.0f);
	float farPlane = ((m22 - 1.0f) * nearPlane) / (m22 + 1.0);

	// We set up all the camera state once, since we use the same shader for compositing all shadow-casting lights
	gBufferShader->Use();
	gBufferShader->SetUniform("a_View", state.Current.View);
	glm::mat4 viewInv = glm::inverse(state.Current.View);
	gBufferShader->SetUniform("a_ViewInv", viewInv);
	gBufferShader->SetUniform("a_CameraPos", glm::vec3(viewInv * glm::vec4(0, 0, 0, 1)));
	gBufferShader->SetUniform("a_ProjectionInv", glm::inverse(state.Current.Projection));
	gBufferShader->SetUniform("a_ViewProjectionInv", glm::inverse(state.Current.ViewProjection));
	gBufferShader->SetUniform("a_NearPlane", nearPlane);
	gBufferShader->SetUniform("a_FarPlane", farPlane);
	gBufferShader->SetUniform("a_Bias", 0.000001f);
	gBufferShader->SetUniform("a_MatShininess", 1.0f); // This should be from the GBuffer

	// Bind our GBuffer textures (note that we skipped 2, since that's the slot for the shadow sampler)
	mainBuffer->Bind(0, RenderTargetAttachment::Color0);
	mainBuffer->Bind(1, RenderTargetAttachment::Depth);
	mainBuffer->Bind(3, RenderTargetAttachment::Color1); // The normal buffer

	// Iterate over all the ShadowLights in the scene
	auto view = CurrentRegistry().view<PointLight>();
	if (view.size() > 0) {
		view.each([&](auto entity, PointLight& light) {
			const TempTransform & transform = ecs.get_or_assign<TempTransform>(entity);
			glm::vec3 pos = glm::vec3(transform.GetWorldTransform() * glm::vec4(0.0, 0.0, 0.0, 1.0));


			//Upload data to shaders
			//gBufferShader->SetUniform("a_LightPos", pos);
			gBufferShader->SetUniform("a_LightColor", light.Color);
			gBufferShader->SetUniform("a_LightAttenuation", light.Attenuation);

			myFullscreenQuad->Draw();
			
		});
	}

	//gBufferShader->Bind();
	//glBindBuffer(fbo, GL_BUFFER);
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	//
	//glDisable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	//
	////do post processing
	//
	//
	//
	//
	//glBindBuffer(0, GL_BUFFER);
	//glDisable(GL_BLEND);


	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//// draw (without glutSwapBuffers)
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//
	//glUseProgram(program_postproc);
	//glBindTexture(GL_TEXTURE_2D, fbo_texture);
	//glUniform1i(uniform_fbo_texture, /*GL_TEXTURE*///0);
	//glEnableVertexAttribArray(attribute_v_coord_postproc);
	//
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
	//glVertexAttribPointer(
	//	attribute_v_coord_postproc,  // attribute
	//	2,                  // number of elements per vertex, here (x,y)
	//	GL_FLOAT,           // the type of each element
	//	GL_FALSE,           // take our values as-is
	//	0,                  // no extra data between each position
	//	0                   // offset of first element
	//);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//glDisableVertexAttribArray(attribute_v_coord_postproc);
	//
	//glUseProgram(program_postproc);
	////GLfloat move = glfwGetTime();  // 3/4 of a wave cycle per second
	////GLint loc = glGetUniformLocation(myShaderHandle, name);
	////GLint loc = glGetUniformLocation(program_postproc, "offset");
	////glUniform1f(loc, move);
	////gBufferShader->Bind();
	////gBufferShader->SetUniform("xImage", 0);
	////gBufferShader->SetUniform("xScreenRes", glm::ivec2(windowSizeWidth, windowSizeHeight));
	////myFullscreenQuad->Draw();
	

}

void Game::UnloadGBuffer()
{
	//glDeleteRenderbuffers(1, &rbo_depth);
	//glDeleteTextures(1, &fbo_texture);
	//glDeleteFramebuffers(1, &fbo);
	//glDeleteBuffers(1, &vbo_fbo_vertices);
	//glDeleteProgram(program_postproc);


}


