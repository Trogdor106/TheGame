#pragma once
#include "TextureEnums.h"
#include "IGraphicsResource.h"


/*
	Represents the base class for all textures
*/
class ITexture : public IGraphicsResource {
public:
	GraphicsClass(ITexture);

	virtual ~ITexture() = default;

	/*
		Gets the internal format of this texture, which indicates how each
		individual texel is composed (ex: RGB8)
		@returns The textures internal texel format
	*/
	InternalFormat GetInternalFormat() const { return myInternalFormat; }


	/*
		Binds this texture to the given texture slot. Different renderers will
		have different amounts of slots, in OpenGL this can be retrieved by
		querying GL_MAX_TEXTURE_IMAGE_UNITS
		@param slot The texture slot to bind this texture to
	*/
	virtual void Bind(uint32_t slot);
	virtual	uint32_t GetRenderID();

protected:
	ITexture() = default;

	InternalFormat myInternalFormat;
};

