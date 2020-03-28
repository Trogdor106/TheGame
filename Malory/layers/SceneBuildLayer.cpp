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
ShadowLight& CreateShadowCaster(florp::game::Scene* scene, entt::entity* entityOut, glm::vec3 pos, glm::vec3 target, glm::vec3 up, float distance = 10.0f, float fov = 60.0f, glm::ivec2 bufferSize = { 1024, 1024 }, const char* name = nullptr)
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

	// Creating our 'console'
	{
		// Material for the frame
		Material::Sptr consoleMat = std::make_shared<Material>(emissiveShader);
		consoleMat->Set("s_Albedo", Texture2D::LoadFromFile("console_albedo.png", false, true, true));
		consoleMat->Set("s_Emissive", Texture2D::LoadFromFile("console_emissive.png", false, true, true));
		consoleMat->Set("a_EmissiveStrength", 4.0f);

		// Load the mesh for the frame
		MeshData console = ObjLoader::LoadObj("console.obj", glm::vec4(1.0f));

		// Create the entity, attach renderable, set position
		entt::entity consoleEntt = scene->CreateEntity();
		RenderableComponent& consoleRenderable = scene->Registry().assign<RenderableComponent>(consoleEntt);
		consoleRenderable.Mesh = MeshBuilder::Bake(console);
		consoleRenderable.Material = consoleMat;
		Transform& t = scene->Registry().get<Transform>(consoleEntt);
		t.SetPosition(glm::vec3(0.0f, -1.0f, -4.0f));

		// We'll use a scrolling shader to make our screen more interesting
		Shader::Sptr scrollShader = std::make_shared<Shader>();
		scrollShader->LoadPart(ShaderStageType::VertexShader, "shaders/lighting.vs.glsl");
		scrollShader->LoadPart(ShaderStageType::FragmentShader, "shaders/forward-scrolling.fs.glsl");
		scrollShader->Link();

		// We'll load in an image for our screen, and set it to be fairly strongly emmisive, and scroll along the y axis
		Material::Sptr monitorMat = std::make_shared<Material>(scrollShader);
		monitorMat->Set("s_Albedo", Texture2D::LoadFromFile("matrix.png", false, true, true));
		monitorMat->Set("s_Emissive", Texture2D::LoadFromFile("matrix.png", false, true, true));
		monitorMat->Set("a_EmissiveStrength", 2.0f);
		monitorMat->Set("a_ScrollDir", glm::vec2(0.0f, 1.0f));

		// We'll make a plane out of a flattened cube
		MeshData data = MeshBuilder::Begin("Monitor");
		MeshBuilder::AddAlignedCube(data, glm::vec3(0.0f), glm::vec3(0.41f, 0.36f, 0.0f));
		Mesh::Sptr mesh = MeshBuilder::Bake(data);

		// Attach a new entity to the console, give it a renderable, and position it juust right
		entt::entity monitor = scene->CreateEntity();
		RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(monitor);
		renderable.Mesh = mesh;
		renderable.Material = monitorMat;
		Transform& t2 = scene->Registry().get<Transform>(monitor);
		t2.SetParent(consoleEntt);
		t2.SetPosition(glm::vec3(0.0f, -1.45f, -0.14f));
		t2.SetEulerAngles(glm::vec3(-45.0f, 0.0f, 0.0f));
	}


	meshLoader();//Load the objects Mesh
	textureLoader(); //Load the textures
	



	//Below are texture examples that should be removed if I forget, for now it's here as reference (delete once used somewhere else)
	// Load and set up our simple test material
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


	for (int i = 0; i < 1 /*materialToUse.size()*/; i++) {
		entt::entity entity = scene->CreateEntity();
		RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(entity);
		renderable.Mesh = MeshBuilder::Bake(meshHolder[meshToUse[i]]);
		renderable.Material = materialHolder[materialToUse[i]];
		Transform& t = scene->Registry().get<Transform>(entity);
		t.SetPosition(initPositions[i]);
		t.SetEulerAngles(initAngles[i]);
		//Add what ever behaviour in here (so hitboxing and interaction)
	}


	// We'll use a constant to tell us how many monkeys to use
	const int numMonkeys = 3;
	const float step = glm::two_pi<float>() / numMonkeys; // Determine the angle between monkeys in radians

	// We'll create a ring of monkeys
	for (int ix = 0; ix < numMonkeys; ix++) {
		entt::entity test = scene->CreateEntity();
		RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(test);
		renderable.Mesh = MeshBuilder::Bake(data);
		renderable.Material = testmotion;
		Transform& t = scene->Registry().get<Transform>(test);
		t.SetPosition(glm::vec3(glm::cos(step * ix) * 5.0f, 0.0f, glm::sin(step * ix) * 5.0f));
		t.SetEulerAngles(glm::vec3(-90.0f, glm::degrees(-step * ix), 0.0f));
		scene->AddBehaviour<Hit>(test, t.GetWorldPosition());
		scene->AddBehaviour<Car>(test, t.GetWorldPosition());
	}

	// We'll create a ring of point lights behind each monkey
	for (int ix = 0; ix < numMonkeys; ix++) {
		// We'll attach an indicator cube to all the lights, and align it with the light's facing
		entt::entity entity = scene->CreateEntity();
		PointLightComponent& light = scene->Registry().assign<PointLightComponent>(entity);
		light.Color = glm::vec3(
			glm::sin(-ix * step) + 1.0f,
			glm::cos(-ix * step) + 1.0f,
			glm::sin((-ix * step) + glm::pi<float>()) + 1.0f) / 2.0f * 0.1f;
		light.Attenuation = 1.0f / 10.0f;
		Transform& t = scene->Registry().get<Transform>(entity);
		t.SetPosition(glm::vec3(glm::cos(step * ix) * 20.0f, 2.0f, glm::sin(step * ix) * 20.0f));
		scene->AddBehaviour<LightFlickerBehaviour>(entity, 2.0f, 0.6f, 1.2f);
		scene->AddBehaviour<Car>(entity, t.GetWorldPosition());


	}

	// The central monkey
	{
		entt::entity test = scene->CreateEntity();
		RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(test);
		renderable.Mesh = MeshBuilder::Bake(data);
		renderable.Material = monkeyMat;
		Transform& t = scene->Registry().get<Transform>(test);
		// Make our monkeys spin around the center
		scene->AddBehaviour<RotateBehaviour>(test, glm::vec3(45.0f, 45.0f, 45.0f));
		scene->AddBehaviour<InputBehaviour>(test, t.GetWorldPosition());
		scene->AddBehaviour<Hit>(test, glm::vec3(0, 0, 0));
	}



	// The box with the polka pattern
	{
		MeshData indicatorCube = MeshBuilder::Begin();
		MeshBuilder::AddAlignedCube(indicatorCube, glm::vec3(0.0f, 0, 0.0), glm::vec3(2.0f, 2.0f, 2.0f));
		Mesh::Sptr indicatorMesh = MeshBuilder::Bake(indicatorCube);

		entt::entity test = scene->CreateEntity();
		RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(test);
		renderable.Mesh = indicatorMesh;
		renderable.Material = mat2;
		Transform& t = scene->Registry().get<Transform>(test);
		t.SetPosition({ 20.0f, 0.0f, 0.0f });
	}

	// We'll use a tiny cube to cast a shadow from our camera, and to indicate where the light sources are
	MeshData indicatorCube = MeshBuilder::Begin();
	MeshBuilder::AddAlignedCube(indicatorCube, glm::vec3(0.0f, 0, 0.0), glm::vec3(0.1f, 0.1f, 0.1f));
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
		camTransform.SetPosition(glm::vec3(1, 1, 10));
		camTransform.LookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
		
		
		// We'll add our control behaviour so that we can fly the camera around
		scene->AddBehaviour<ControlBehaviour>(camera, glm::vec3(1.0f));
		//scene->AddBehaviour<Car>(camera, glm::vec3(0.5f));

		// We'll attach a cube to the camera so that it casts shadows
		RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(camera);
		renderable.Mesh = indicatorMesh;
		renderable.Material = marbleMat;

	}

	// We'll create a projector to cast our smile on the floor
	entt::entity lightEnt = entt::null;
	auto& light = CreateShadowCaster(scene, &lightEnt, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 25.0f);
	light.Color = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
	light.Attenuation = 1.0f / 15.0f;
	light.ProjectorImage = Texture2D::LoadFromFile("light_projection.png", false, false, true);
	scene->AddBehaviour<LightFlickerBehaviour>(lightEnt, 10.0f);

	// We'll create a ring of shadow casting lights, one for each monkey
	for (int ix = 0; ix < numMonkeys; ix++) {
		entt::entity lightEnt = entt::null;
		auto& light = CreateShadowCaster(
			scene, &lightEnt,
			glm::vec3(glm::cos(step * ix) * 9.0f, 3.5f, glm::sin(step * ix) * 9.0f), // Each light will be behind the monkey
			glm::vec3(0.0f),                                                         // Look at the center
			glm::vec3(0.0f, 1.0f, 0.0f),                                             // Y is up
			25.0f,                                                                   // The far plane is 25 units away
			75.0f);                                                                  // We'll use a 75 degree field of view
		// We'll generate a color for the light
		light.Color = glm::vec3(1.0f, 0.64f, 0.0f) * 0.2f;
		light.Attenuation = 1.0f / 5.0f;
		scene->AddBehaviour<LightFlickerBehaviour>(lightEnt, 5.0f, 0.5f, 1.0f);

		// We'll attach an indicator cube to all the lights, and align it with the light's facing
		entt::entity entity = scene->CreateEntity();
		RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(entity);
		renderable.Mesh = indicatorMesh;
		renderable.Material = marbleMat;
		Transform& t = scene->Registry().get<Transform>(entity);
		t.SetPosition(glm::vec3(glm::cos(step * ix) * 9.0f, 2.0f, glm::sin(step * ix) * 9.0f));
	}

	// Our floor plane
	{
		// Building the mesh
		MeshData data = MeshBuilder::Begin();
		MeshBuilder::AddAlignedCube(data, glm::vec3(0.0f, -1.0f, 0.0), glm::vec3(100.0f, 0.1f, 100.0f));
		Mesh::Sptr mesh = MeshBuilder::Bake(data);

		// Creating the entity and attaching the renderable 
		entt::entity entity = scene->CreateEntity();
		RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(entity);
		renderable.Mesh = MeshBuilder::Bake(data);
		renderable.Material = marbleMat;
	}
}


void SceneBuilder::textureLoader()
{
	//using namespace florp::game;
	//using namespace florp::graphics;


	std::vector <char*> texturename = { "",
				//1						//2					//3							//4							//5								//6				
		"textures/f_Bed.png", "textures/f_Door.png", "textures/f_doorFrame.png", "textures/f_Drawer.png", "textures/f_DresserNoDrawer.png", "textures/F_fatWall.png",
			//7
		"textures/NewCask.png",
			//8						//9					//10						 //11					 //12					//13		   
		"textures/f_Key1.png", "textures/Note.png", "textures/KitchenTable.png", "textures/Cupboards.png", "textures/Door.png", "textures/DoorOpen.png",
				//14					//15
		"textures/1stFloor.png", "textures/WideWindow.png",
				//16				//17					//18					  //19							//20					
		"textures/Piano.png", "textures/Sink.png", "textures/BigVase.png", "textures/BookShelfFull.png", "textures/KitchenStairs.png",
			//21						  //22
		"textures/SmallWindow.png", "textures/FrontDoor.png",
			//23				   //24						 //25					 //26				  //27					  //28
		"textures/Toilet.png", "textures/Fireplace.png", "textures/Ceiling.png", "textures/Key1.png", "textures/Portrait.png", "textures/Portrait2.png"
	};

	
	florp::game::Material::Sptr temp = std::make_shared<florp::game::Material>(shader);
	temp->Set("s_Albedo", florp::graphics::Texture2D::LoadFromFile("marble.png", false, true, true));
	materialHolder.push_back(temp);
	materialHolder.push_back(temp);
	//Textures need to be fixed for down here

	materialToUse = { 0, 0, 0, 0 }; //Put the id of the texture you will want to use with the object that will show up in the list

	//Can uncomment this once all the textures have been resized
	//for (int i = 1; i < texturename.size(); i++) {
	//	temp = std::make_shared<florp::game::Material>(shader);
	//	temp->Set("s_Albedo", florp::graphics::Texture2D::LoadFromFile(texturename[i], false, true, true));
	//	materialHolder.push_back(temp);
	//}

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
			//1							 //2					//3						 //4						//5						 //6               
		"Objects/1stFloor.obj",	"Objects/2ndFloor.obj",	 "Objects/Bed.obj",	 "Objects/BigVaseUV.obj",	 "Objects/SmallVase.obj", "Objects/Book.obj",
				//7
		"Objects/BookShelf.obj",
				//8						    //9								 //10										 //11
		"Objects/Door.obj", "Objects/f_Door(Reverse_open_1_3).obj", "Objects/f_Door(Reverse_open_2_3).obj", "Objects/DoorOpen.obj",
				//12								//13							  //14								//15				   //16                
		"Objects/f_Door(open_1_3).obj", "Objects/f_Door(open_2_3).obj", "Objects/f_Door(open_Max).obj", "Objects/LargeDrawer.obj", "Objects/Dresser.obj",
				//17							//18						   //19					 //20				 //21
		"Objects/Dresser.obj", "Objects/f_DresserWithMorror.obj",  "Objects/Key1.obj",   "Objects/Key2.obj",   "Objects/Key3.obj",
				//22					    //23					 //24					 //25							//26          
		"Objects/f_Key4.obj", "Objects/Portrait.obj", "Objects/SmallFrame.obj", "Objects/SmallWindow.obj", "Objects/Stairs.obj",
				//27					//28
		"Objects/f_LargeWindow.obj", "Objects/WideWindow.obj" ,
				//29					  //30							//31									 //32					//33
		"Objects/BookShelfFull.obj", "Objects/f_rotatedDoor.obj", "Objects/f_shortFlatWall.obj", "Objects/f_rotatedShortWall.obj", "Objects/FrontDoor.obj",
				//34							//35					//36				//37				 //38							//39			
		"Objects/SafeRoomStairs.obj", "Objects/KitchenStairs.obj", "Objects/Piano.obj", "Objects/Toilet.obj" ,"Objects/PaintBucket.obj" , "Objects/NewCask.obj",
				//40
		"Objects/note.obj",
				//41					  //42				  //43							//44					 //45					//46					
		"Objects/sink UV.obj", "Objects/Mirror.obj", "Objects/Fireplace.obj", "Objects/KitchenTable.obj", "Objects/Cupboards.obj", "Objects/HallwayWindow.obj",
				//47
		"Objects/FireplaceSmallWindow.obj",
				//48
		"Objects/Ceiling.obj"
	};

	initPositions = {
		glm::vec3(44.2f, 49.9f, 0.0f), //Object 0 The first floor
		glm::vec3(50.5f, 11.5f, 0.0f), //Object 1 Door right saferoom
		glm::vec3(3.85f, -11.1f, 0.0f),// object 2 is door bottom of saferoom
		glm::vec3(61.6f, 11.5f, -0.1f) // object 3 door right across saferoom
	};

	for (int i = 0; i < initPositions.size(); i++) {
		initAngles.push_back(glm::vec3(0.0));
		if (i == 2) { //Put any objects with rotations in an else if list
			initAngles[i] = glm::vec3(PI);
		}
	}

	MeshData data = ObjLoader::LoadObj("monkey.obj", glm::vec4(1.0f));

	for (int i = 1; i < filename.size(); i++) {
		data = ObjLoader::LoadObj(filename[i], glm::vec4(1.0f));
		meshHolder.push_back(data);
	}

	meshToUse = { 0, 0, 0, 0 }; //Put the id of the mesh you will want to use with the object that will show up in the list


	int hello = 0;
}
