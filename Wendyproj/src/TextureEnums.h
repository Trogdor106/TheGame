#pragma once
#include <glad/glad.h>
#include <EnumToString.h>
#include "Texture2D.h"

// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
// These are some of our more common available internal formats


// These are our options for GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_S and GL_TEXTURE_WRAP_R
ENUM(WrapMode, GLint,
	ClampToEdge = GL_CLAMP_TO_EDGE,
	ClampToBorder = GL_CLAMP_TO_BORDER,
	MirroredRepeat = GL_MIRRORED_REPEAT,
	Repeat = GL_REPEAT, // Default
	MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
);

// These are our available options for the GL_TEXTURE_MIN_FILTER setting
ENUM(MinFilter, GLint,
	Nearest = GL_NEAREST,
	Linear = GL_LINEAR,
	NearestMipNearest = GL_NEAREST_MIPMAP_NEAREST,
	LinearMipNearest = GL_LINEAR_MIPMAP_NEAREST,
	NearestMipLinear = GL_NEAREST_MIPMAP_LINEAR, // This is the default setting
	LinearMipLinear = GL_LINEAR_MIPMAP_LINEAR
);

// These are our available options for the GL_TEXTURE_MAG_FILTER setting
ENUM(MagFilter, GLint,
	Nearest = GL_NEAREST,
	Linear = GL_LINEAR  // This is the default setting
);

/*
 * Gets the number of bytes needed to represent a single texel of the given format and type
 * @param format The format of the texel
 * @param type The data type of the texel
 * @returns The size of a single texel of the given format and type, in bytes
 */
constexpr size_t GetTexelSize(PixelFormat format, PixelType type) {
	size_t result = 0;

	switch (type) {
	case PixelType::UByte:
	case PixelType::Byte:
		result = 1; break;
	case PixelType::UShort:
	case PixelType::Short:
		result = 2; break;
	case PixelType::Int:
	case PixelType::UInt:
		result = 4; break;
	default:
		_ASSERT(false);
	}

	switch (format)
	{
	case PixelFormat::Depth:
	case PixelFormat::DepthStencil:
	case PixelFormat::Red:
		result *= 1; break;
	case PixelFormat::Rg:
		result *= 2; break;
	case PixelFormat::Rgb:
	case PixelFormat::Bgr:
		result *= 3; break;
	case PixelFormat::Rgba:
	case PixelFormat::Bgra:
		result *= 4; break;
	default:
		_ASSERT(false);
	}

	return result;
}

