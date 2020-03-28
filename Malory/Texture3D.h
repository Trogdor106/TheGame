#pragma once
#include "florp/graphics/ITexture.h"
#include <GLM/glm.hpp>

/*
 * Represents the data that will be transferred from the CPU to the GPU when uploading 3D texture data
 */
struct Texture3dData {
	uint32_t    Width, Height, Depth;
	florp::graphics::PixelFormat Format;
	florp::graphics::PixelType   Type;
	void* Data;
};

/*
   Holds information representing the internal state of a 3D texture
*/
struct Texture3dDescription {
	/*
	 * The width, height or depth of the texture, in texels
	 * @default 0
	 */
	uint32_t       Width, Height, Depth;
	/*
		The number of mip levels to generate for this texture, 0 will calculate the number of miplevels,
		1 will disable mipmapping
		@default 0
	*/
	uint32_t       MipmapLevels;
	/*
	 * The sized internal format of the texture
	 * @default InternalFormat::RGBA8
	 */
	florp::graphics::InternalFormat Format;
	/*
	 * The wrap parameter for the horizontal texture axis
	 * @default WrapMode::Repeat
	 */
	florp::graphics::WrapMode       WrapS;
	/*
	 * The wrap parameter for the vertical texture axis
	 * @default WrapMode::Repeat
	 */
	florp::graphics::WrapMode       WrapT;
	/*
	 * The wrap parameter for the height texture axis
	 * @default WrapMode::Repeat
	 */
	florp::graphics::WrapMode       WrapR;
	/*
	 * The minification filter to use for the texture if no sampler is in use
	 * @default MinFilter::NearestMipLinear
	 */
	florp::graphics::MinFilter      MinFilter;
	/*
	 * The magnification filter to use for the texture if no sampler is in use
	 * @default MagFilter::Linear
	 */
	florp::graphics::MagFilter      MagFilter;
	/*
		The default amount of anisotropic filtering on this texture
		@default 1.0f
	*/
	float          Aniso;
	/*
	 * The default border color for this texture when using WrapMode::ClampToBorder, and no
	 * sampler is specified
	 * @default (0,0,0,1)
	 */
	glm::vec4      BorderColor;

	/*
	 * Initializes a texture description to it's default parameters
	 */
	Texture3dDescription() :
		Width(0), Height(0), Depth(0),
		MipmapLevels(0),
		Format(florp::graphics::InternalFormat::RGBA8),
		WrapS(florp::graphics::WrapMode::Repeat), WrapT(florp::graphics::WrapMode::Repeat), WrapR(florp::graphics::WrapMode::Repeat),
		MinFilter(florp::graphics::MinFilter::NearestMipLinear),
		MagFilter(florp::graphics::MagFilter::Linear),
		Aniso(1.0f),
		BorderColor({ 0.0f, 0.0f, 0.0f, 1.0f }) { }
};

/*
 * Represents a 3D texture
 * In OpenGL, this is a wrapper around the glTexture3d* functions
 */
class Texture3D : public florp::graphics::ITexture {
public:
	GraphicsClass(Texture3D);

	Texture3D(const Texture3dDescription& description);
	virtual ~Texture3D();

	/*
	 * Loads a 3D texture from the given file path, as a 2D texture atlas
	 * @param filePath The path to a texture asset on disk
	 * @param xSlices The number of layers along the x axis
	 * @param ySlices The number of layers along the y axis
	 * @returns A shared pointer for a 3D texture as loaded from the given path
	 */
	static Texture3D::Sptr LoadFrom2DFile(const std::string& filePath, uint32_t xSlices, uint32_t ySlices, bool loadAlpha = true, bool generateMipMaps = true, bool linearize = false);
	/*
	 * Loads a 3D texture from the given file path, can be either a texture atlas (2D) or a .cube file
	 * @param filePath The path to a texture asset on disk
	 * @returns A shared pointer for a 3D texture as loaded from the given path
	 */
	static Texture3D::Sptr LoadFrom3DFile(const std::string& filePath);
	// Mutators
public:
	/*
	 * Sets the data in this texture, overwriting any existing data
	 * @param data The new data for this texture, must match the current texture's dimensions
	 */
	void SetData(const Texture3dData& data);

	// Accessors
public:
	uint32_t GetWidth() const { return myDescription.Width; }
	uint32_t GetHeight() const { return myDescription.Height; }

	bool IsMipmapEnabled() const { return myDescription.MipmapLevels; }
	uint32_t GetMipLevels() const { return myDescription.MipmapLevels; }

	florp::graphics::WrapMode GetWrapS() const { return myDescription.WrapS; }
	florp::graphics::WrapMode GetWrapT() const { return myDescription.WrapT; }
	florp::graphics::WrapMode GetWrapR() const { return myDescription.WrapR; }

	florp::graphics::MinFilter GetMinFilter() const { return myDescription.MinFilter; };
	florp::graphics::MagFilter GetMagFilter() const { return myDescription.MagFilter; };

	float GetAniso() const { return myDescription.Aniso; }

	/*
		Extracts data from this texture with the given format. Note that memory is allocated with malloc,
		and is owned by the calling function. This means that you MUST release the memory with a call to
		free() after you are done with the data.
		@param format The texel format to extract data as
		@param type   The data type of the components of the texel
		@returns A block of data, allocated with malloc, that contains the data in the texture
	*/
	void* GetData(florp::graphics::PixelFormat format, florp::graphics::PixelType type) const;

protected:
	Texture3dDescription myDescription;

	// Handles actually allocating space for the texture on the GPU
	void __RecreateTexture();

private:
	static uint32_t MaxTextureSize;
};
