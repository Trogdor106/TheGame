#include "SceneBuildLayer.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>


/*
 * Helper function for creating a shadow casting light
 * @param scene The scene to create the light in
 * @param entityOut An optional pointer to receive the ENTT entity that was created (set to nullptr if you don't care)
 * @param pos The position of the light in world space
 * @param target The point for the light to look at, in world space
 * @param up A unit vector indicating what axis is considered 'up'
 * @param distance The far clipping plane of the light
 * @param fov The field of view of the light, in degrees
 * @param bufferSize The size of the buffer to create for the light (default 1024x1024)
 * @param name The name to associate with the light's buffer
 */
ShadowLight& CreateShadowCaster(florp::game::Scene* scene, entt::entity* entityOut, glm::vec3 pos, glm::vec3 target, glm::vec3 up, float distance = 10.0f, float fov = 60.0f, glm::ivec2 bufferSize = { 2024, 2024 }, const char* name = nullptr)
{
	ProfileTimer timer = ProfileTimer("Create Shadow Casting Light");

	// The depth attachment is a texture, with 32 bits for depth
	RenderBufferDesc depth = RenderBufferDesc();
	depth.ShaderReadable = true;
	depth.Attachment = RenderTargetAttachment::Depth;
	depth.Format = RenderTargetType::Depth32;

	// Our shadow buffer is depth-only
	FrameBuffer::Sptr shadowBuffer = std::make_shared<FrameBuffer>(bufferSize.x, bufferSize.y);
	shadowBuffer->AddAttachment(depth);
	shadowBuffer->Validate();
	if (name != nullptr)
		shadowBuffer->SetDebugName(name);

	// Create a new entity
	entt::entity entity = scene->CreateEntity();

	// Assign and initialize a shadow light component
	ShadowLight& light = scene->Registry().assign<ShadowLight>(entity);
	light.ShadowBuffer = shadowBuffer;
	light.Projection = glm::perspective(glm::radians(fov), (float)bufferSize.x / (float)bufferSize.y, 0.25f, distance);
	light.Attenuation = 1.0f / distance;
	light.Color = glm::vec3(1.0f);

	// Assign and initialize the transformation
	florp::game::Transform& t = scene->Registry().get<florp::game::Transform>(entity);
	t.SetPosition(pos);
	t.LookAt(target, up);

	// Send out the entity ID if we passed in a place to store it
	if (entityOut != nullptr)
		*entityOut = entity;

	return light;
}

florp::graphics::Texture2D::Sptr CreateSolidTexture(glm::vec4 color)
{
	//glEnable(GL_FRAMEBUFFER_SRGB);
	ProfileTimer timer = ProfileTimer("Create Single Pixel Texture");

	using namespace florp::graphics;
	static std::unordered_map<glm::vec4, Texture2D::Sptr> cache;

	// If a texture for that color exists in the cache, return it
	if (cache.find(color) != cache.end())
		return cache[color];
	// Otherwise, we'll create a new texture, cache it, then return it
	else {
		// We'll disable essentially anything fancy for our single-pixel color
		Texture2dDescription desc = Texture2dDescription();
		desc.Width = desc.Height = 1;
		desc.Format = InternalFormat::RGBA8;
		desc.MagFilter = MagFilter::Nearest;
		desc.MinFilter = MinFilter::Nearest;
		desc.MipmapLevels = 1;
		desc.WrapS = desc.WrapT = WrapMode::ClampToEdge;

		// By using the float pixel type, we can simply feed in the address of our color
		Texture2dData data = Texture2dData();
		data.Width = data.Height = 1;
		data.Format = PixelFormat::Rgba;
		data.Type = PixelType::Float;
		data.Data = &color.r;

		// Create the texture, and load the single pixel data
		Texture2D::Sptr result = std::make_shared<Texture2D>(desc);
		result->SetData(data);

		// Store in the cache
		cache[color] = result;

		return result;
	}
}

void SceneBuilder::Initialize()
{
	ProfileTimer timer = ProfileTimer("Scene Build");

	florp::app::Application* app = florp::app::Application::Get();

	using namespace florp::game;
	using namespace florp::graphics;

	auto* scene = SceneManager::RegisterScene("main");
	SceneManager::SetCurrentScene("main");

	ProfileTimer loadMonkey = ProfileTimer("Load Monkey Mesh"); //Timer for debuging


	// We'll load in a monkey head to render something interesting
	MeshData data = ObjLoader::LoadObj("monkey.obj", glm::vec4(1.0f));


	loadMonkey.Stop(); //Stop the timer

	shader = std::make_shared<Shader>();
	shader->LoadPart(ShaderStageType::VertexShader, "shaders/lighting.vs.glsl");
	shader->LoadPart(ShaderStageType::FragmentShader, "shaders/forward.fs.glsl");
	shader->Link();


	// This is our emissive lighting shader
	emissiveShader = std::make_shared<Shader>();
	emissiveShader->LoadPart(ShaderStageType::VertexShader, "shaders/lighting.vs.glsl");
	emissiveShader->LoadPart(ShaderStageType::FragmentShader, "shaders/forward-emissive.fs.glsl");
	emissiveShader->Link();


	displacementmap = std::make_shared<Shader>();
	displacementmap->LoadPart(ShaderStageType::VertexShader, "shaders/heightmap.vs.glsl");
	displacementmap->LoadPart(ShaderStageType::FragmentShader, "shaders/forward-emissive.fs.glsl");
	displacementmap->Link();

	meshLoader();//Load the objects Mesh
	textureLoader(); //Load the textures


	//Below are texture examples that should be removed if I forget, for now it's here as reference (delete once used somewhere else)
	// Load and set up our simple test material

	Material::Sptr heightMat = std::make_shared<Material>(displacementmap);
	heightMat->Set("s_Albedo", Texture2D::LoadFromFile("marble.png", false, true, true));
	heightMat->Set("s_Emissive", Texture2D::LoadFromFile("monkey_emissive.png", false, true, true));
	heightMat->Set("a_EmissiveStrength", 4.0f);


	Material::Sptr monkeyMat = std::make_shared<Material>(emissiveShader);
	monkeyMat->Set("s_Albedo", Texture2D::LoadFromFile("marble.png", false, true, true));
	monkeyMat->Set("s_Emissive", Texture2D::LoadFromFile("monkey_emissive.png", false, true, true));
	monkeyMat->Set("a_EmissiveStrength", 4.0f);

	// We'll have another material for the marble without any emissive spots
	Material::Sptr marbleMat = std::make_shared<Material>(shader);
	marbleMat->Set("s_Albedo", Texture2D::LoadFromFile("marble.png", false, true, true));

	// This will be for the polka-cube
	Material::Sptr mat2 = std::make_shared<Material>(emissiveShader);
	mat2->Set("s_Albedo", Texture2D::LoadFromFile("polka.png", false, true, true));
	mat2->Set("s_Emissive", Texture2D::LoadFromFile("polka.png", false, true, true));
	mat2->Set("a_EmissiveStrength", 1.0f);

	Material::Sptr testmotion = std::make_shared<Material>(shader);
	testmotion->Set("s_Albedo", Texture2D::LoadFromFile("marbleBlue.png", false, true, true));





	//Create objects, put them in the scene and attach a behavior down below

	int lockedDoorID = 0;
	int lockedKeyID = 0;

	for (int i = 0; i < materialToUse.size(); i++) {

		entt::entity entity = scene->CreateEntity();
		RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(entity);
		renderable.Mesh = MeshBuilder::Bake(meshHolder[meshToUse[i]]);
		renderable.Material = materialHolder[materialToUse[i]];
		Transform& t = scene->Registry().get<Transform>(entity);
		t.SetPosition(glm::vec3(genTransform[i][3][0], genTransform[i][3][1], genTransform[i][3][2]));//   initPositions[i]);
		t.SetEulerAngles(glm::vec3(genTransform[i][0][0], genTransform[i][1][1], genTransform[i][2][2]));
		//Add what ever behaviour in here (so hitboxing and interaction)

		tempHit = { 0,0,0,0,0,0 };
		for (int y = 0; y < meshHolder[meshToUse[i]].Vertices.size(); y++) {
			if (y == 0) {
				tempHit.Bottom = meshHolder[meshToUse[i]].Vertices[y].Position.y;//.y;
				tempHit.top = meshHolder[meshToUse[i]].Vertices[y].Position.y;//.y;
				tempHit.Left = meshHolder[meshToUse[i]].Vertices[y].Position.x;//.x;
				tempHit.Right = meshHolder[meshToUse[i]].Vertices[y].Position.x;//.x;
			}
			else {

				// pos							pos
				//  Top Left-----------TopRight
				//  |						  |
				//  |						  |
				//  |						  |
				//  |						  |
				//  |						  |
				//  BottomLeft------BottomRight						  
				//pos							pos
				if (meshHolder[meshToUse[i]].Vertices[y].Position.y < tempHit.Bottom) {
					tempHit.Bottom = meshHolder[meshToUse[i]].Vertices[y].Position.y;
				}
				if (meshHolder[meshToUse[i]].Vertices[y].Position.y > tempHit.top) {
					tempHit.top = meshHolder[meshToUse[i]].Vertices[y].Position.y;
				}
				if (meshHolder[meshToUse[i]].Vertices[y].Position.x > tempHit.Right) {
					tempHit.Right = meshHolder[meshToUse[i]].Vertices[y].Position.x;
				}
				if (meshHolder[meshToUse[i]].Vertices[y].Position.x < tempHit.Left) {
					tempHit.Left = meshHolder[meshToUse[i]].Vertices[y].Position.x;
				}
			}
		}
		tempHit.ID = i;
		tempHit.floor = floorToUse[i];
		HitBoxes::GetInstance().saveHitBoxes(tempHit, floorToUse[i]);
		HitBoxes::GetInstance().updateHitBox(i, genTransform[i]);

		if (meshToUse[i] == 4 || meshToUse[i] == 7 || meshToUse[i] == 10) { //Normal door (change the number if it's not 3 that you want)
			scene->AddBehaviour<doorManDoors>(entity, t.GetWorldPosition(), floorToUse[i], i);;// scene->AddBehaviour<doorOpening>(entity, glm::vec3(genTransform[i][3][0], genTransform[i][3][1], genTransform[i][3][2]), floorToUse[i]);
		}
		else if (meshToUse[i] == 8 || meshToUse[i] == 5 || meshToUse[i] == 32) { //Locked door (change the number if it's not 6 that you want(hint: it's not))
			scene->AddBehaviour<lockedDoorManDoors>(entity, t.GetWorldPosition(), floorToUse[i], lockedDoorID, i);
			lockedDoorID++;
		}
		else if (meshToUse[i] == 18 || meshToUse[i] == 19 || meshToUse[i] == 20 || meshToUse[i] == 21) { //Locked door (change the number if it's not 3 that you want)
			scene->AddBehaviour<keymebaby>(entity, floorToUse[i], lockedKeyID);
			lockedKeyID++;
		}
		else if (meshToUse[i] == 20) { //stairs near spawn
			scene->AddBehaviour<stairs1>(entity, floorToUse[i]);
		}
		else if (meshToUse[i] == 21) { //stairs near kitchen
			scene->AddBehaviour<stairs2>(entity, floorToUse[i]);
		}
		else if (meshToUse[i] == 38) {
			scene->AddBehaviour<Refuel>(entity);
		}

	}
	int goodbye = 0;

	// We'll use a constant to tell us how many monkeys to use
	//const int numMonkeys = 3;
	//const float step = glm::two_pi<float>() / numMonkeys; // Determine the angle between monkeys in radians
	//
	//// We'll create a ring of monkeys
	//for (int ix = 0; ix < numMonkeys; ix++) {
	//	entt::entity test = scene->CreateEntity();
	//	RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(test);
	//	renderable.Mesh = MeshBuilder::Bake(data);
	//	renderable.Material = testmotion;
	//	Transform& t = scene->Registry().get<Transform>(test);
	//	t.SetPosition(glm::vec3(glm::cos(step * ix) * 5.0f, 0.0f, glm::sin(step * ix) * 5.0f));
	//	t.SetEulerAngles(glm::vec3(-90.0f, glm::degrees(-step * ix), 0.0f));
	//	scene->AddBehaviour<Hit>(test, t.GetWorldPosition());
	//	scene->AddBehaviour<Car>(test, t.GetWorldPosition());
	//}
	//
	//// We'll create a ring of point lights behind each monkey
	//for (int ix = 0; ix < numMonkeys; ix++) {
	//	// We'll attach an indicator cube to all the lights, and align it with the light's facing
	//	entt::entity entity = scene->CreateEntity();
	//	PointLightComponent& light = scene->Registry().assign<PointLightComponent>(entity);
	//	light.Color = glm::vec3(
	//		glm::sin(-ix * step) + 1.0f,
	//		glm::cos(-ix * step) + 1.0f,
	//		glm::sin((-ix * step) + glm::pi<float>()) + 1.0f) / 2.0f * 0.1f;
	//	light.Attenuation = 1.0f / 10.0f;
	//	Transform& t = scene->Registry().get<Transform>(entity);
	//	t.SetPosition(glm::vec3(glm::cos(step * ix) * 20.0f, 2.0f, glm::sin(step * ix) * 20.0f));
	//	scene->AddBehaviour<LightFlickerBehaviour>(entity, 2.0f, 0.6f, 1.2f);
	//	scene->AddBehaviour<Car>(entity, t.GetWorldPosition());
	//
	//
	//}
	//
	//// The central monkey
	//{
	//	entt::entity test = scene->CreateEntity();
	//	RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(test);
	//	renderable.Mesh = MeshBuilder::Bake(data);
	//	renderable.Material = monkeyMat;
	//	Transform& t = scene->Registry().get<Transform>(test);
	//	// Make our monkeys spin around the center
	//	scene->AddBehaviour<RotateBehaviour>(test, glm::vec3(45.0f, 45.0f, 45.0f));
	//	scene->AddBehaviour<InputBehaviour>(test, t.GetWorldPosition());
	//	scene->AddBehaviour<Hit>(test, glm::vec3(0, 0, 0));
	//}
	//
	//
	//
	//// The box with the polka pattern
	//{
	//	MeshData indicatorCube = MeshBuilder::Begin();
	//	MeshBuilder::AddAlignedCube(indicatorCube, glm::vec3(0.0f, 0, 0.0), glm::vec3(2.0f, 2.0f, 2.0f));
	//	Mesh::Sptr indicatorMesh = MeshBuilder::Bake(indicatorCube);
	//
	//	entt::entity test = scene->CreateEntity();
	//	RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(test);
	//	renderable.Mesh = indicatorMesh;
	//	renderable.Material = mat2;
	//	Transform& t = scene->Registry().get<Transform>(test);
	//	t.SetPosition({ 20.0f, 0.0f, 0.0f });
	//}

	// We'll use a tiny cube to cast a shadow from our camera, and to indicate where the light sources are
	MeshData indicatorCube = MeshBuilder::Begin();
	MeshBuilder::AddAlignedCube(indicatorCube, glm::vec3(28.0f, 8.0f, 15.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	Mesh::Sptr indicatorMesh = MeshBuilder::Bake(indicatorCube);

	// Creates our main camera
	{
		// The color buffer should be marked as shader readable, so that we generate a texture for it
		RenderBufferDesc mainColor = RenderBufferDesc();
		mainColor.ShaderReadable = true;
		mainColor.Attachment = RenderTargetAttachment::Color0;
		mainColor.Format = RenderTargetType::ColorRgb8;

		// The normal buffer
		RenderBufferDesc normalBuffer = RenderBufferDesc();
		normalBuffer.ShaderReadable = true;
		normalBuffer.Attachment = RenderTargetAttachment::Color1;
		normalBuffer.Format = RenderTargetType::ColorRgb10; // Note: this format is 10 bits per component

		// The normal buffer
		RenderBufferDesc emissiveBuffer = RenderBufferDesc();
		emissiveBuffer.ShaderReadable = true;
		emissiveBuffer.Attachment = RenderTargetAttachment::Color2;
		emissiveBuffer.Format = RenderTargetType::ColorRgb10; // Note: this format is 10 bits per component

		// The depth attachment does not need to be a texture (and would cause issues since the format is DepthStencil)
		RenderBufferDesc depth = RenderBufferDesc();
		depth.ShaderReadable = true;
		depth.Attachment = RenderTargetAttachment::Depth;
		depth.Format = RenderTargetType::Depth32;

		// Our main frame buffer needs a color output, and a depth output
		FrameBuffer::Sptr buffer = std::make_shared<FrameBuffer>(app->GetWindow()->GetWidth(), app->GetWindow()->GetHeight(), 4);
		buffer->AddAttachment(mainColor);
		buffer->AddAttachment(normalBuffer);
		buffer->AddAttachment(emissiveBuffer);
		buffer->AddAttachment(depth);
		buffer->Validate();
		buffer->SetDebugName("MainBuffer");

		// We'll create an entity, and attach a camera component to it
		entt::entity camera = scene->CreateEntity();
		CameraComponent& cam = scene->Registry().assign<CameraComponent>(camera);
		cam.BackBuffer = buffer;
		cam.FrontBuffer = buffer->Clone();
		cam.IsMainCamera = true;
		cam.Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 1000.0f);



		auto& camTransform = scene->Registry().get<Transform>(camera);
		camTransform.SetPosition(glm::vec3(-5.0f, -5.0f, 10.0f));
		camTransform.LookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));


		// We'll add our control behaviour so that we can fly the camera around
		scene->AddBehaviour<ControlBehaviour>(camera, glm::vec3(1.0f));
		scene->AddBehaviour<death>(camera);
		scene->AddBehaviour<Listener>(camera);
		scene->AddBehaviour<BackgroundMusic>(camera);

	}

	// We'll create a ring of shadow casting lights, one for each monkey
	for (int ix = 0; ix < 3; ix++) {
		entt::entity lightEnt = entt::null;
		if (ix == 0) {
			auto& light = CreateShadowCaster(
				scene, &lightEnt,
				glm::vec3(51.0f, 18.0f, 15.0f), // Each light will be behind the monkey
				glm::vec3(0, 0, 0.0f),                                                         // Look at the center
				glm::vec3(0.0f, 0.0f, 1.0f),                                             // Z is up
				100.0f,                                                                   // The far plane is 25 units away
				120.0f);                                                                  // We'll use a 75 degree field of view
					// We'll generate a color for the light
			light.Color = glm::vec3(1.0f, 0.64f, 0.0f) * 0.2f;
			light.Attenuation = 5000.0f;
			light.ProjectorImage = Texture2D::LoadFromFile("textures/Fire.png", false, false, true);
		}
		else if (ix == 1) {
			auto& light = CreateShadowCaster(
				scene, &lightEnt,
				glm::vec3(glm::cos(90 * ix) * 9.0f, 3.5f, glm::sin(90 * ix) * 9.0f), // Each light will be behind the monkey
				glm::vec3(0.0f, 0.0f, 0.0f),			                         // Look at
				glm::vec3(0.0f, 1.0f, 0.0f),                                             // Y is up
				25.0f,                                                                   // The far plane is 25 units away
				75.0f);
			// We'll use a 75 degree field of view
			// We'll generate a color for the light
			light.Color = glm::vec3(1.0f, 0.64f, 0.0f) * 0.2f;
			light.Attenuation = 1.0f / 5.0f;
		}
		else if (ix == 2) {
			auto& light = CreateShadowCaster(
				scene, &lightEnt,
				glm::vec3(49.0f, 29.0f, 1.0f), // Each light will be behind the monkey
				glm::vec3(0.0f, 0.0f, 0.0f),			                         // Look at
				glm::vec3(0.0f, 1.0f, 0.0f),                                             // Y is up
				25.0f,                                                                   // The far plane is 25 units away
				75.0f);
			// We'll use a 75 degree field of view
			// We'll generate a color for the light
			light.Color = glm::vec3(0.0f, 0.00f, 1.0f);
			light.Attenuation = 1.0f / 5.0f;
			scene->AddBehaviour<aStarAlgorithm>(lightEnt);
		}

		if (ix == 1) {
			scene->AddBehaviour<LightFlickerBehaviour>(lightEnt, 5.0f, 0.5f, 1.0f);
			scene->AddBehaviour<lantern>(lightEnt);
		}
		else if (ix == 0) {
			scene->AddBehaviour<safeRoomLight>(lightEnt);
		}
	}
}

void SceneBuilder::textureLoader()
{
	//using namespace florp::game;
	//using namespace florp::graphics;


	std::vector <char*> texturename = { "",
		//0                            //1                        //2                    //3                                //4
		"textures/floor 1 4096New.png", "textures/Door.png", "textures/DoorOpen.png", "textures/f_Drawer.png", "textures/f_DresserNoDrawer.png",
		//5                            //6                            //7                     //8                    //9
		"textures/NewCask.png", "textures/KitchenTable.png", "textures/Cupboards.png", "textures/Door.png", "textures/DoorOpen.png",
		//10                        //11                //12                    //13                    //14
		"textures/WideWindow.png", "textures/Piano.png", "textures/Sink.png", "textures/BigVase.png", "textures/BookShelfFull.png",
		//15                            //16                        //17                    //18                    //19
		"textures/KitchenStairs.png", "textures/SmallWindow.png", "textures/FrontDoor.png", "textures/Toilet.png", "textures/Fireplace.png",
		//20                    //21                    //22                    //23                    //24
		"textures/Ceiling.png", "textures/Key1.png", "textures/Portrait.png", "textures/Portrait2.png", "textures/Chandelier.png",
		//25                        //26                    //27						//28						//29
		"textures/EndTable.png", "textures/TableObjects.png",  "textures/Key1.png", "textures/PauseScreen.png", "textures/Dead.png",
			//30					//31
		"textures/Title.png", "textures/Win.png"
			};

	
	florp::game::Material::Sptr temp = std::make_shared<florp::game::Material>(shader);
	temp->Set("s_Albedo", florp::graphics::Texture2D::LoadFromFile("marble.png", false, true, true));
	//materialHolder.push_back(temp);
	//materialHolder.push_back(temp);
	//Textures need to be fixed for down here

	materialToUse = {  //Put the id of the texture you will want to use with the object that will show up in the list
		0,                                        //0
		27, 27, 2 , 1 , 2 , 4 , 3 , 3 , 3 , 5 , //10
		16, 11, 11, 14, 14, 2 , 2 , 2 , 17, 2 ,    //20
		15, 15, 11, 18, 12, 6 , 2 , 5 , 19, 6 ,    //30
		7 , 13, 13, 27, 27, 10, 10, 10, 16, 16,    //40
		22, 23, 20, 24, 25, 3 , 26, 25, 25, 0 ,    //50
		1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,    //60
		1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,    //70
		1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,    //80
		1 , 1, 28, 30, 29, 31, 27, 27
	};

	for (int i = 1; i < texturename.size(); i++) {
		
		if (!(i >= 29 && i <= 32)) {
			temp = std::make_shared<florp::game::Material>(shader);
			temp->Set("s_Albedo", florp::graphics::Texture2D::LoadFromFile(texturename[i], false, true, true));
			materialHolder.push_back(temp);
		}
		else {
			temp = std::make_shared<florp::game::Material>(displacementmap);
			temp->Set("s_Albedo", florp::graphics::Texture2D::LoadFromFile(texturename[i], false, true, true));
			temp->Set("s_Emissive", florp::graphics::Texture2D::LoadFromFile("textures/fire.png", false, true, true));
			temp->Set("a_EmissiveStrength", 10.03f);


			materialHolder.push_back(temp);
		}
	}

	int hello = 0;


	//Material::Sptr monkeyMat = std::make_shared<Material>(emissiveShader);
	//monkeyMat->Set("s_Albedo", Texture2D::LoadFromFile("marble.png", false, true, true));
	//monkeyMat->Set("s_Emissive", Texture2D::LoadFromFile("monkey_emissive.png", false, true, true));
	//monkeyMat->Set("a_EmissiveStrength", 4.0f);
	
}

void SceneBuilder::meshLoader()
{
	using namespace florp::game;
	using namespace florp::graphics;


	std::vector <char*> filename = { "",
					//1						 //2					//3					   //4						  //5					 //6             
			"Objects/1stFloorUVD_1.obj",	"Objects/2ndFloor.obj",	 "Objects/Bed.obj",	 "Objects/note.obj",	 "Objects/DoorReverse.obj", "Objects/DoorReverseLocked.obj",
			//7
			"Objects/BookShelf.obj",
			//8						//9							 //10							 //11
			"Objects/Door.obj", "Objects/DoorLocked.obj", "Objects/f_Door(Reverse_open_2_3).obj", "Objects/DoorOpen.obj",
			//12								//13							  //14								//15				  //16                
			"Objects/f_Door(open_1_3).obj", "Objects/f_Door(open_2_3).obj", "Objects/f_Door(open_Max).obj", "Objects/f_Drawer.obj", "Objects/Dresser.obj",
			//17							//18						   //19					 //20				 //21
			"Objects/Dresser.obj", "Objects/f_DresserWithMorror.obj",  "Objects/Key1.obj",   "Objects/Key2.obj",   "Objects/Key3.obj",
			//22					    //23					 //24					 //25						//26          
			"Objects/Key4.obj", "Objects/Portrait.obj", "Objects/SmallFrame.obj", "Objects/SmallWindow.obj", "Objects/Stairs.obj",
			//27								//28
			"Objects/f_LargeWindow.obj", "Objects/WideWindow.obj" ,
			//29								  //30							//31								 //32						//33
			"Objects/BookShelfFull.obj", "Objects/f_rotatedDoor.obj", "Objects/f_shortFlatWall.obj", "Objects/f_rotatedShortWall.obj", "Objects/FrontDoor.obj",
			//34									//35					//36				//37				 //38							//39			
			"Objects/SafeRoomStairs.obj", "Objects/KitchenStairs.obj", "Objects/Piano.obj", "Objects/Toilet.obj" ,"Objects/PaintBucket.obj" , "Objects/NewCask.obj",
			//40
			"Objects/BigVaseUV.obj",
			//41						  //42				  //43							//44					 //45					//46					
			"Objects/sink UV.obj", "Objects/Mirror.obj", "Objects/Fireplace.obj", "Objects/KitchenTable.obj", "Objects/Cupboards.obj", "Objects/HallwayWindow.obj",
			//47										//48					//49					//50					//51
			"Objects/FireplaceSmallWindow.obj",	"Objects/Ceiling.obj", "Objects/Chandelier.obj", "Objects/EndTable.obj", "Objects/TableObjects.obj",
			//52						//53						//54						//55						//56
			"Objects/FrontEndTable.obj", "Objects/FrontDrawer.obj", "Walls/TopRightSafeWall.obj", "Walls/TopLeftSafeWall.obj", "Walls/BottomLeftSafeWall.obj",
			//57								//58								//59					//60
			"Walls/BottomRightSafeWall.obj", "Walls/RightBottomSafeWall.obj", "Walls/RightTopSafeWall.obj", "Walls/LeftSafeWall.obj",
			//61										//62							//63							//64
			"Walls/BottomLeftMainHallWall.obj", "Walls/BottomRightMainHallWall.obj", "Walls/TopLeftMainHallWall.obj", "Walls/BottomBasementWall.obj",
			//65						//66								//67								//68
			"Walls/TopMainHallWall.obj", "Walls/RightMainHallWall.obj", "Walls/LeftRightStairwayWall.obj", "Walls/RightRightStairwayWall.obj",
			//69						//70						//71						//72						//73
			"Walls/RightStudyWall.obj", "Walls/LeftStudyWall.obj", "Walls/BottomStudyWall.obj", "Walls/TopStudyWall.obj", "Walls/LeftTopMainHallWall.obj",
			//74								//75								//76							//77
			"Walls/LeftBottomKitchenWall.obj", "Walls/LeftTopKitchenWall.obj", "Walls/RightBottomKitchenWall.obj", "Walls/RightTopKitchenWall.obj",
			//78							//79						//80							//81						//82
			"Walls/TopLeftKitchenWall.obj", "Walls/TopRightKitchenWall.obj", "Walls/LeftPantryWall.obj", "Walls/TopBathroomWall.obj", "Walls/BottomBathroomWall.obj",
			//83							//84							//85								//86
			"Walls/LeftTopBathroomWall.obj", "Walls/LeftBottomBathroomWall.obj", "Walls/BottomLeftStairwayWall.obj", "Walls/BottomRightStairwayWall.obj",
			//87
			"Objects/plane.obj"
	};


	glm::mat4 temp = glm::mat4(1.0f);

	for (int i = 0; i < 100; i++) {
		switch (i) {
		case 0: //Object 0 The first floor
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f))); //Working
			break;
		case 1: //Object 1 Bathroom key
			genTransform.push_back(glm::translate(temp, glm::vec3(90.55f, 24.6f, -1.0f))); //Working
			break;
		case 2:// object 2 Kitchen key
			genTransform.push_back(glm::translate(temp, glm::vec3(133.0f, 40.0f, -1.5f)));
			break;
		case 3:// object 3 door right across saferoom
			genTransform.push_back(glm::translate(temp, glm::vec3(66.2f, 20.55f, -0.8f))); //Working
			break;
		case 4:// object 4 is door top across saferoom
			genTransform.push_back(glm::translate(temp, glm::vec3(8.3f, 49.0f, -0.1))); //Working
			break;
		case 5:// object 5 is door top safe room
			genTransform.push_back(glm::translate(temp, glm::vec3(12.7f, 36000.5f, -0.1))); //Not used
			break;
		case 6:// object 6 is the dresser in the room where you find the red key
			genTransform.push_back(glm::translate(temp, glm::vec3(96.55f, -9.0f, -110.1f))); //Not used
			break;
		case 7:// object 7 is the top drawer in the room where you find the red key 
			genTransform.push_back(glm::translate(temp, glm::vec3(96.55f, -7.92f, 161.1f))); //Not used
			break;
		case 8:// object 8 is the middle drawer in the room where you find the red key
			genTransform.push_back(glm::translate(temp, glm::vec3(96.55f, -7.92f, 113.8f))); //Not used
			break;
		case 9:// object 9 is the bottom drawer in the room where you find the red key
			genTransform.push_back(glm::translate(temp, glm::vec3(96.55f, -7.92f, 111.8f))); //Not used
			break;
		case 10:// object 10 is a long broken window in the safe room
			genTransform.push_back(glm::translate(temp, glm::vec3(-11.85f, 16.5f, 117.5f))); //Not used
			break;
		case 11:// object 11 is a broken window in the stairwell
			genTransform.push_back(glm::translate(temp, glm::vec3(-9.0f, -17.5f, 5.5f))); //Working
			break;
		case 12:// object 12 is a big vase in the safe room
			genTransform.push_back(glm::translate(temp, glm::vec3(-4.85f, 1.75f, 1000.0f))); //Not used
			break;
		case 13:// object 13 is a big vase in the safe room
			genTransform.push_back(glm::translate(temp, glm::vec3(-4.00f, -3.25f, 1000.0f))); //Not used
			break;
		case 14:// object 14 is the bookshelf with way, way too many faces
			genTransform.push_back(glm::translate(temp, glm::vec3(24.0f, -7.1f, -0.2f))); //Working
			break;
		case 15:// object 15 is the bookshelf in the study
			genTransform.push_back(glm::translate(temp, glm::vec3(122.0f, -37.4f, -3.0f))); //Working
			break;
		case 16:// object 16 is the left kitchen door
			genTransform.push_back(glm::translate(temp, glm::vec3(106.6f, 12.65f, -2.0f))); //Working
			break;
		case 17:// object 17 is the right kitchen door
			genTransform.push_back(glm::translate(temp, glm::vec3(169.35f, 23.15f, -3.0f))); //Working
			break;

		case 18:// object 18 is the study door
			genTransform.push_back(glm::translate(temp, glm::vec3(169.35f, -7.35f, -3.0f))); //Working(106.6f, 12.65f, -2.0f)
			break;

		case 19:// object 19 is the front door
			genTransform.push_back(glm::translate(temp, glm::vec3(78.8f, 97.65f, 0))); //Working
			break;

		case 20:// object 20 is the pantry door
			genTransform.push_back(glm::translate(temp, glm::vec3(106.6f, 43.15f, -1.7f))); //Working
			break;

		case 21:// object 21 is the safe room staircase
			genTransform.push_back(glm::translate(temp, glm::vec3(15.85f, -15.85f, 0.0f))); //Working
			break;

		case 22:// object 22 is the kitchen staircase
			genTransform.push_back(glm::translate(temp, glm::vec3(192.0f, -0.9f, -3.0f))); //Working
			break;

		case 23:// object 23 is the piano
			genTransform.push_back(glm::translate(temp, glm::vec3(123.4f, -11.0f, -3.0f))); //Working
			break;

		case 24:// object 24 is the 1st floor toilet
			genTransform.push_back(glm::translate(temp, glm::vec3(85.55f, 11.9f, -1.5f))); //Working
			break;

		case 25:// object 25 is the bathroom sink
			genTransform.push_back(glm::translate(temp, glm::vec3(90.55f, 25.3f, -1.0f))); //Working
			break;

		case 26:// object 26 is the mirror above the sink
			genTransform.push_back(glm::translate(temp, glm::vec3(90.55f, 26.0f, 700.0f))); //Not used
			break;

		case 27:// object 27 is the basement door
			genTransform.push_back(glm::translate(temp, glm::vec3(32.15f, 68.3f, 0.0f))); //Working
			break;

		case 28:// object 28 is the oil cask
			genTransform.push_back(glm::translate(temp, glm::vec3(49.0f, 29.0f, 1.0f))); //Working
			break;

		case 29:// object 29 is the fireplace
			genTransform.push_back(glm::translate(temp, glm::vec3(-3.0f, 20.0f, -0.0f))); //Working
			break;

		case 30:// object 30 is the kitchen table
			genTransform.push_back(glm::translate(temp, glm::vec3(135.0f, 45.0f, -1.5f))); //Working
			break;

		case 31:// object 31 is the kitchen cupboards
			genTransform.push_back(glm::translate(temp, glm::vec3(161.5f, 58.5f, -2.0f))); //Working
			break;

		case 32:// object 32 is the 2nd floor
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, -32.0f))); //Working
			break;

		case 33:// object 33 is the 2nd floor bed
			genTransform.push_back(glm::translate(temp, glm::vec3(-4.0f, 19.0f, 48.0f))); //Working
			break;

		case 34:// object 34 is the end table key
			genTransform.push_back(glm::translate(temp, glm::vec3(194.85f, 94.0f, -1.9f)));
			break;

		case 35:// object 35 is the study key
			genTransform.push_back(glm::translate(temp, glm::vec3(122.0f, -37.4f, -3.0f)));
			break;

		case 36:// object 36 is the left window in the top hallway
			genTransform.push_back(glm::translate(temp, glm::vec3(105.0f, 95.3f, 6.0f))); //Working
			break;

		case 37:// object 37 is the right window in the top hallway
			genTransform.push_back(glm::translate(temp, glm::vec3(137.0f, 95.5f, 6.0f))); //Working
			break;

		case 38:// object 38 is the window in the right hallway
			genTransform.push_back(glm::translate(temp, glm::vec3(198.26f, 60.0f, 6.0f))); //Working
			break;

		case 39:// object 39 is the window in the kitchen stairwell
			genTransform.push_back(glm::translate(temp, glm::vec3(198.26f, 12.0f, 4.0f))); //Working
			break;

		case 40:// object 40 is the window on the fireplace wall
			genTransform.push_back(glm::translate(temp, glm::vec3(-9.0f, 0.0f, 9.5f))); //Working
			break;

		case 41:// object 41 is the portrait across from left window in the top hallway
			genTransform.push_back(glm::translate(temp, glm::vec3(97.5f, 80.0f, 14.5f))); //Working
			break;

		case 42:// object 42 is the portrait past the first one
			genTransform.push_back(glm::translate(temp, glm::vec3(117.5f, 80.65f, 14.5f))); //Working
			break;

		case 43:// object 43 is the ceiling
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 5.0f))); //Working
			break;

		case 44:// object 44 is the chandelier over the kitchen table
			genTransform.push_back(glm::translate(temp, glm::vec3(135.0f, 45.0f, 20.0f))); //Working
			break;

		case 45:// object 45 is an end table
			genTransform.push_back(glm::translate(temp, glm::vec3(194.85f, 94.0f, -1.9f))); //Working
			break;

		case 46:// object 46 is the end table drawer
			genTransform.push_back(glm::translate(temp, glm::vec3(194.85f, 94.0f, -1.9f))); //Working
			break;

		case 47:// object 47 is the stuff on the doorway end table
			genTransform.push_back(glm::translate(temp, glm::vec3(79.75f, 54.75f, -0.5))); //Working
			break;

		case 48:// object 48 is the doorway end table
			genTransform.push_back(glm::translate(temp, glm::vec3(79.75f, 54.5f, -0.5))); //Working
			break;

		case 49:// object 49 is the doorway end table drawer
			genTransform.push_back(glm::translate(temp, glm::vec3(79.75f, 54.5f, -0.5))); //Working
			break;

		case 50: //Object 50 The safe room top right wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 51: //Object 51 The safe room top left wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 52: //Object 52 The safe room bottom left wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 53: //Object 53 The safe room bottom right wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 54: //Object 54 The safe room right bottom wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 55: //Object 55 The safe room right top wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 56: //Object 56 The safe room left wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 57: //Object 57 The main hall bottom left wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 58: //Object 58 The main hall bottom right wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 59: //Object 59 The main hall top left wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 60: //Object 60 The basement bottom wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 61: //Object 61 The main hall top wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 62: //Object 62 The main hall right wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 63: //Object 63 The right stairway left wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 64: //Object 64 The right stairway right wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 65: //Object 65 The study right wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 66: //Object 66 The study left wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 67: //Object 67 The study bottom wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 68: //Object 68 The study top wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 69: //Object 69 The hallway left top wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 70: //Object 70 The kitchen left bottom wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 71: //Object 71 The kitchen left top wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 72: //Object 72 The kitchen right top wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 51.9f, 0.0f)));
			break;

		case 73: //Object 73 The kitchen right bottom wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 51.9f, 0.0f)));
			break;

		case 74: //Object 74 The kitchen top left wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 75: //Object 75 The kitchen top right wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 76: //Object 76 is the pantry left wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 77: //Object 77 is the bathroom top wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 78: //Object 78 is the bathroom bottom wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 79: //Object 79 is the bathroom left top wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 80: //Object 80 is the bathroom left bottom wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 81: //Object 81 is the bottom left hallway wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;

		case 82: //Object 82 is the bottom right stairway wall
			genTransform.push_back(glm::translate(temp, glm::vec3(44.2f, 49.9f, 0.0f)));
			break;
		case 83: //Object 83 is the Pause Screen
			genTransform.push_back(glm::translate(temp, glm::vec3(0.0f, 9990.0f, 0.0f)));
			break;
		case 84: //Object 84 is the Title Screen
			genTransform.push_back(glm::translate(temp, glm::vec3(0.0f, 19990.0f, 0.0f)));
			break;
		case 85: //Object 85 is the Death Screen
			genTransform.push_back(glm::translate(temp, glm::vec3(0.0f, 29990.0f, 0.0f)));
			break;
		case 86: //Object 86 is the Win Screen
			genTransform.push_back(glm::translate(temp, glm::vec3(0.0f, 39990.0f, 0.0f)));
			break;

		case 87: //Object 87 is a key that can never be found so we can have a door that can never open
			genTransform.push_back(glm::translate(temp, glm::vec3(0.0f, 290.0f, 10000.0f)));
			break;
		case 88: //Object 88 is a key that can never be found so we can have a door that can never open
			genTransform.push_back(glm::translate(temp, glm::vec3(0.0f, 390.0f, 10000.0f)));
			break;
		}
	}

	MeshData data = ObjLoader::LoadObj("monkey.obj", glm::vec4(1.0f));

	for (int i = 1; i < filename.size(); i++) {
		
		data = ObjLoader::LoadObj(filename[i], glm::vec4(1.0f));
		meshHolder.push_back(data);
	}
	//4, 7, 10 are regular doors, 5, 8 are locked doors (there's only one 10 used so it doesn't need to have a locked version)
	//18, 19, 20, 21 are keys, number of keys must equal number of locked doors
	//2 doors are locked forever
	meshToUse = { //Put the id of the mesh you will want to use with the object that will show up in the list
	//      1st                                 10th                                    20th                                    30th                                   40th
		0 , 18, 19, 10, 8 , 4 , 15, 14, 14, 14, 27, 46, 39, 39, 28, 28, 5 , 4 , 5 , 32 , 5, 33, 34, 35, 36, 40, 41, 5 , 38, 42, 43, 44, 1 , 3, 20, 21, 45, 45, 27, 24, 46, 22, 22, 47, 48, 49, 14, 50, 51, 52,
	//  50th                                    60th                                    70th                                    80th        From 0 to 82, so 83 objects
		53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 86, 86, 86, 21, 21

	}; //Put the id of the mesh you will want to use with the object that will show up in the list

	floorToUse = { // floor that the mesh is on
	  -1,                                //0
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, -1, -1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, -1, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, -1, -1, -1, -1, -1, -1
	};


	int hello = 0;
}
