#include "Texture3D.h"
#include "Logging.h"
#include <GLM/gtc/type_ptr.hpp>
#include "stb_image.h"
#include <filesystem>
#include <fstream>

uint32_t Texture3D::MaxTextureSize = 0;

uint8_t* __RepackAtlas(uint8_t* data, int numChannels, int& width, int& height, int& depth, const uint32_t xSlices, const uint32_t ySlices) {
	LOG_ASSERT(numChannels > 0, "Number of channels must be greater than zero!");

	// Determine the dimensions of our 3D texture
	uint32_t zSlices = xSlices * ySlices;
	uint32_t sliceWidth = width / xSlices;
	uint32_t sliceHeight = height / ySlices;

	// The depth of our 3D texture is given by the number of slices along the z axis
	depth = zSlices;

	// Determine the size of each slice, and the total volume of the cube
	uint32_t sliceSize = sliceWidth * sliceHeight;
	uint32_t volume = sliceSize * depth;

	// Allocate a new array of memory to store the packed result
	uint8_t* result = new uint8_t[sizeof(uint8_t) * numChannels * volume];
	
	// Iterate over all layers
	for (uint32_t iz = 0; iz < depth; iz++) {
		// For each layer, iterate over each column and row
		for (uint32_t iy = 0; iy < sliceHeight; iy++) {
			for (uint32_t ix = 0; ix < sliceWidth; ix++) {
				// Determine the location into the data array
				uint32_t xLoc = (iz % xSlices) * sliceWidth + ix;
				uint32_t yLoc = (iz / ySlices) * sliceHeight + iy;
				// Copy each pixel by component
				for(uint32_t ic = 0; ic < numChannels; ic++) {
					result[(iz * sliceSize + iy * sliceWidth + ix) * numChannels + ic] = data[(yLoc * width + xLoc) * numChannels + ic];
				}
			}
		}
	}

	// Output the width and height of the 3D texture
	width = sliceWidth;
	height = sliceHeight;

	// Return the unpacked data
	return result;
}

bool endsWith(std::string const& fullString, std::string const& ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}

Texture3D::Texture3D(const Texture3dDescription& description)
{
	if (MaxTextureSize == 0) {
		int size = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
		MaxTextureSize = size;
		LOG_INFO("Maximum texture size on this renderer is {} texels", MaxTextureSize);
	}

	// Ensure that the texture size is valid
	LOG_ASSERT(description.Width > 0, "Texture width must be > 0");
	LOG_ASSERT(description.Height > 0, "Texture height must be > 0");
	LOG_ASSERT(description.Depth > 0, "Texture depth must be > 0");
	LOG_ASSERT(description.Width <= MaxTextureSize, "Texture width must be < {}", MaxTextureSize);
	LOG_ASSERT(description.Height <= MaxTextureSize, "Texture height must be < {}", MaxTextureSize);
	LOG_ASSERT(description.Depth <= MaxTextureSize, "Texture depth must be < {}", MaxTextureSize);

	// Store the texture description
	myDescription = description;
	// Creates the texture on the GPU
	__RecreateTexture();
}

Texture3D::~Texture3D() {
	LOG_INFO("Deleting texture \"{}\" with ID: {}", myDebugName, myRendererID);
	glDeleteTextures(1, &myRendererID);
}

Texture3D::Sptr Texture3D::LoadFrom2DFile(const std::string& filePath, uint32_t xSlices, uint32_t ySlices, bool loadAlpha, bool generateMipMaps, bool linearize)
{
	using namespace florp::graphics;
	
	// Load the image data from the file
	int width, height, numChannels;
	int targetChannels =
		linearize ? 3 :
		(loadAlpha ? 4 : 0);

	void* data = stbi_load(filePath.c_str(), &width, &height, &numChannels, targetChannels);
	if (targetChannels != 0)
		numChannels = targetChannels;

	// We have data!
	InternalFormat internal_format;
	PixelFormat    image_format;
	switch (numChannels) {
	case 1:
		internal_format = InternalFormat::R8;
		image_format = PixelFormat::Red;
		break;
	case 2:
		internal_format = InternalFormat::RG8;
		image_format = PixelFormat::Rg;
		break;
	case 3:
		internal_format = InternalFormat::RGB8;
		image_format = PixelFormat::Rgb;
		break;

	case 4:
		internal_format = InternalFormat::RGBA8;
		image_format = PixelFormat::Rgba;
		break;

	default:
		LOG_WARN("Unsupported texture format. Texture may look strange. ({})", filePath);
		internal_format = InternalFormat::RGBA8;
		image_format = PixelFormat::Rgba;
		break;
	}

	LOG_ASSERT((numChannels * width) % 4 == 0, "The alignment of a horizontal line in a texture must be 4 bytes! Check that your number of channels * your width is a multiple of 4");

	// Only create the texture if we actually got data out
	if (data != nullptr && width != 0 && height != 0 && numChannels != 0) {

		int texWidth = width;
		int texHeight = height;
		int texDepth = 0;

		uint8_t* packedData = __RepackAtlas((uint8_t*)data, numChannels, texWidth, texHeight, texDepth, xSlices, ySlices);
		
		// Create our texture description
		Texture3dDescription desc = Texture3dDescription();
		desc.Width = texWidth;
		desc.Height = texHeight;
		desc.Depth = texDepth;
		desc.Format = internal_format;

		// Define what data will be loaded into the texture, and it's format
		Texture3dData texData = Texture3dData();
		texData.Width = texWidth;
		texData.Height = texHeight;
		texData.Depth = texDepth;
		texData.Format = image_format;
		texData.Type = PixelType::UByte;
		texData.Data = packedData;

		// Create the image, and load the data into it
		Sptr result = std::make_shared<Texture3D>(desc);
		result->SetData(texData);
		result->SetDebugName(std::filesystem::path(filePath).filename().string());

		// Free the underlying image data and return the image
		stbi_image_free(texData.Data);
		delete[] packedData;
		return result;
	}
	else {
		// Free the data just, to be safe
		stbi_image_free(data);
		LOG_WARN("Failed to load image from \"{}\"", filePath);
		return nullptr;
	}
}

Texture3D::Sptr Texture3D::LoadFrom3DFile(const std::string& filePath)
{

	// Open the file
	std::ifstream inFile(filePath);

	uint8_t* textureData = nullptr;
	int lutSize{ 0 };
	uint32_t ix{ 0 };
	float r, g, b;

	std::string line;
	while (std::getline(inFile, line)) {
		// Ignore comments
		if (line._Starts_with("#")) {}
		// Ignore whitespace lines
		else if (std::all_of(line.begin(), line.end(), isspace)) {}
		// Ignore the title directive
		else if (line.find("TITLE") != std::string::npos) { }
		// Ignore domain directives
		else if (line.find("DOMAIN") != std::string::npos) { }
		// Read in the lut size
		else if (line.find("LUT_3D_SIZE") != std::string::npos) {
			std::stringstream lReader(line);
			std::string temp;
			lReader >> temp >> lutSize;
			textureData = new uint8_t[lutSize * lutSize * lutSize * 3];
		}
		// Read in the data lines
		else if (line.size() > 0) {
			std::stringstream lReader(line);
			lReader >> r >> g >> b;
			textureData[ix++] = r * 255;
			textureData[ix++] = g * 255;
			textureData[ix++] = b * 255;

		}
	}

	// Create our texture description
	Texture3dDescription desc = Texture3dDescription();
	desc.Width = lutSize;
	desc.Height = lutSize;
	desc.Depth = lutSize;
	desc.Format = florp::graphics::InternalFormat::RGB8;

	// Define what data will be loaded into the texture, and it's format
	Texture3dData texData = Texture3dData();
	texData.Width = lutSize;
	texData.Height = lutSize;
	texData.Depth = lutSize;
	texData.Format = florp::graphics::PixelFormat::Rgb;
	texData.Type = florp::graphics::PixelType::UByte;
	texData.Data = textureData;

	// Create the image, and load the data into it
	Sptr result = std::make_shared<Texture3D>(desc);
	result->SetData(texData);
	result->SetDebugName(std::filesystem::path(filePath).filename().string());

	// Free the underlying image data and return the image
	free(textureData);
	return result;
}

void Texture3D::SetData(const Texture3dData& data) {
	// Make sure that the dimensions of the data match the dimension of the texture
	LOG_ASSERT(data.Width == myDescription.Width, "Data width does not match texture width");
	LOG_ASSERT(data.Height == myDescription.Height, "Data width does not match texture width");
	LOG_ASSERT(data.Depth == myDescription.Depth, "Data depth does not match texture depth");


	// Load the data into the texture
	glTextureSubImage3D(myRendererID, 0, 0, 0, 0, myDescription.Width, myDescription.Height, myDescription.Depth, (GLenum)data.Format, (GLenum)data.Type, data.Data);

	// Generate the texture's mip maps
	if (myDescription.MipmapLevels > 1)
		glGenerateTextureMipmap(myRendererID);
}

void* Texture3D::GetData(florp::graphics::PixelFormat format, florp::graphics::PixelType type) const {
	size_t dataSize = GetTexelSize(format, type) * myDescription.Width * myDescription.Height * myDescription.Depth;
	void* result = malloc(dataSize);
	glGetTextureImage(myRendererID, 0, (GLenum)format, (GLenum)type, dataSize, result);
	return result;
}

void Texture3D::__RecreateTexture() {
	// If our miplevels is 0, we need to calculate how many mipmap levels we should generate
	myDescription.MipmapLevels = myDescription.MipmapLevels == 0 ?
		(1 + floor(log2(glm::max(glm::max(myDescription.Width, myDescription.Height), myDescription.Depth)))) :
		(myDescription.MipmapLevels);
	
	glCreateTextures(GL_TEXTURE_3D, 1, & myRendererID);

	glTextureStorage3D(myRendererID, myDescription.MipmapLevels, (GLenum)myDescription.Format, myDescription.Width, myDescription.Height, myDescription.Depth);

	glTextureParameteri(myRendererID, GL_TEXTURE_WRAP_S, (GLenum)myDescription.WrapS);
	glTextureParameteri(myRendererID, GL_TEXTURE_WRAP_T, (GLenum)myDescription.WrapT);
	glTextureParameteri(myRendererID, GL_TEXTURE_WRAP_R, (GLenum)myDescription.WrapR);
	glTextureParameteri(myRendererID, GL_TEXTURE_MIN_FILTER, (GLenum)myDescription.MinFilter);
	glTextureParameteri(myRendererID, GL_TEXTURE_MAG_FILTER, (GLenum)myDescription.MagFilter);

	glTextureParameterfv(myRendererID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(myDescription.BorderColor));

	if (myDescription.Aniso != 1.0f)
		glTextureParameterf(myRendererID, GL_TEXTURE_MAX_ANISOTROPY, myDescription.Aniso);
}
