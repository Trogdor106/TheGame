#pragma once
#include "EnumToString.h"
#include "Logging.h"
#include "glad/glad.h"
#include <glm/glm.hpp>
/*
 * Represents the type of data used in a shader.
 *
 * The bitwise makeup of these values is shown below:
 *
 * HIGH             20      12    8    4    LOW
 * |----------------|--------|----|----|----|
 * |----Reserved----|--Type--|Meta|-D1-|-D2-|
 *
 */
ENUM(ShaderDataType, uint32_t,
	None                        = 0,

	Float                       = 0b00000001'0000'0000'0001,
	Float2                      = 0b00000001'0000'0000'0010,
	Float3                      = 0b00000001'0000'0000'0011,
	Float4                      = 0b00000001'0000'0000'0100,

	Mat2                        = 0b00000010'0000'0010'0010,
	Mat3                        = 0b00000010'0000'0011'0011,
	Mat4                        = 0b00000010'0000'0100'0100,
	Mat2x3                      = 0b00000010'0000'0010'0011,
	Mat2x4                      = 0b00000010'0000'0010'0100,
	Mat3x2                      = 0b00000010'0000'0011'0010,
	Mat3x4                      = 0b00000010'0000'0011'0100,
	Mat4x2                      = 0b00000010'0000'0100'0010,
	Mat4x3                      = 0b00000010'0000'0100'0011,

	Int                         = 0b00000100'0000'0000'0001,
	Int2                        = 0b00000100'0000'0000'0010,
	Int3                        = 0b00000100'0000'0000'0011,
	Int4                        = 0b00000100'0000'0000'0100,

	Uint                        = 0b00001000'0000'0000'0001,
	Uint2                       = 0b00001000'0000'0000'0010,
	Uint3                       = 0b00001000'0000'0000'0011,
	Uint4                       = 0b00001000'0000'0000'0100,

	Uint64                      = 0b00001000'0000'0001'0001,

	Double                      = 0b00010000'0000'0000'0001,
	Double2                     = 0b00010000'0000'0000'0010,
	Double3                     = 0b00010000'0000'0000'0011,
	Double4                     = 0b00010000'0000'0000'0100,

	// Note: Double precision matrices do require some newer RenderAPI's (mainly OpenGL4 or DX11 level hardware)

	Dmat2                       = 0b00100000'0000'0010'0010,
	Dmat3                       = 0b00100000'0000'0011'0011,
	Dmat4                       = 0b00100000'0000'0100'0100,
	Dmat2x3                     = 0b00100000'0000'0010'0011,
	Dmat2x4                     = 0b00100000'0000'0010'0100,
	Dmat3x2                     = 0b00100000'0000'0011'0010,
	Dmat3x4                     = 0b00100000'0000'0011'0100,
	Dmat4x2                     = 0b00100000'0000'0100'0010,
	Dmat4x3                     = 0b00100000'0000'0100'0011,

	Bool                        = 0b01000000'0000'0000'0001,
	Bool2                       = 0b01000000'0000'0000'0010,
	Bool3                       = 0b01000000'0000'0000'0011,
	Bool4                       = 0b01000000'0000'0000'0100,

	// Texture resources (not to be used in vertex elements)

	// Usage of bit fields are a bit different here
	// | 10000000 | 0 0 | 0 0 | 0   0   0   0   0 | 0 0 0 |
	// -----------------|-----|---|---|---|---|---|---------
	// | Sampler  |           | B | M | S | A | R | \Size/      
	//                        | U | U | H | R | E |    *or 100 for cube                   
	//                        | F | L | A | R | C |                       
	//                        | F | T | D | A | T |                        
	//                        | E | I | O | Y |   |
	//                        | R | S | W |   |   |
	//                              A
	//                              M
	//                              P
	//                              L
	//                              E
	//                             

	// Rect texture means no mipmapping, texel fetch is non-normalized
	// Buffer textures are for passing data, can only be accessed via texelFetch

	Tex1D                       = 0b10000000'0000'00000'001,
	Tex1D_Array                 = 0b10000000'0000'00010'001,
	Tex1D_Shadow                = 0b10000000'0000'00100'001,
	Tex1D_ShadowArray           = 0b10000000'0000'00110'001,

	Tex2D                       = 0b10000000'0000'00000'010,
	Tex2D_Rect                  = 0b10000000'0000'00001'010,
	Tex2D_Rect_Shadow           = 0b10000000'0000'00101'010,
	Tex2D_Array                 = 0b10000000'0000'00010'010,
	Tex2D_Shadow                = 0b10000000'0000'00100'010,
	Tex2D_ShadowArray           = 0b10000000'0000'00110'010,
	Tex2D_Multisample           = 0b10000000'0000'01000'010,
	Tex2D_MultisampleArray      = 0b10000000'0000'01010'010,

	Tex3D                       = 0b10000000'0000'00000'011,

	TexCube                     = 0b10000000'0000'00000'100,
	TexCubeShadow               = 0b10000000'0000'00100'100,

	Tex1D_Int                   = 0b10000000'0001'00000'001,
	Tex1D_Int_Array             = 0b10000000'0001'00010'001,
	Tex2D_Int                   = 0b10000000'0001'00000'010,
	Tex2D_Int_Rect              = 0b10000000'0001'00001'010,
	Tex2D_Int_Array             = 0b10000000'0001'00010'010,
	Tex2D_Int_Multisample       = 0b10000000'0001'01000'010,
	Tex2D_Int_MultisampleArray  = 0b10000000'0001'01010'010,
	Tex3D_Int                   = 0b10000000'0001'00000'011,
	TexCube_Int                 = 0b10000000'0001'00000'100,

	Tex1D_Uint                  = 0b10000000'0010'00000'001,
	Tex2D_Uint_Rect             = 0b10000000'0010'00001'010,
	Tex1D_Uint_Array            = 0b10000000'0010'00010'001,
	Tex2D_Uint                  = 0b10000000'0010'00000'010,
	Tex2D_Uint_Array            = 0b10000000'0010'00010'010,
	Tex2D_Uint_Multisample      = 0b10000000'0010'01000'010,
	Tex2D_Uint_MultisampleArray = 0b10000000'0010'01010'010,
	Tex3D_Uint                  = 0b10000000'0010'00000'011,
	TexCube_Uint                = 0b10000000'0010'00000'100,

	BufferTexture               = 0b10000000'0000'10000'000,
	BufferTextureInt            = 0b10000000'0001'10000'000,
	BufferTextureUint           = 0b10000000'0010'10000'000
);

const uint32_t ShaderDataType_TypeMask = 0b11111111'0000'0000'0000;
const uint32_t ShaderDataType_Size1Mask = 0b00000000'0000'0000'1111;
const uint32_t ShaderDataType_Size2Mask = 0b00000000'0000'1111'0000;


template <typename T>
ShaderDataType GetShaderDataType() {
	return ShaderDataType::None;
}

template <> inline ShaderDataType GetShaderDataType<float>() {
	return ShaderDataType::Float;
}
template <> inline ShaderDataType GetShaderDataType<glm::vec2>() {
	return ShaderDataType::Float2;
}
template <> inline ShaderDataType GetShaderDataType<glm::vec3>() {
	return ShaderDataType::Float3;
}
template <> inline ShaderDataType GetShaderDataType<glm::vec4>() {
	return ShaderDataType::Float4;
}
template <> inline ShaderDataType GetShaderDataType<glm::mat2>() {
	return ShaderDataType::Mat2;
}
template <> inline ShaderDataType GetShaderDataType<glm::mat2x3>() {
	return ShaderDataType::Mat2x3;
}
template <> inline ShaderDataType GetShaderDataType<glm::mat2x4>() {
	return ShaderDataType::Mat2x4;
}
template <> inline ShaderDataType GetShaderDataType<glm::mat3>() {
	return ShaderDataType::Mat3;
}
template <> inline ShaderDataType GetShaderDataType<glm::mat3x2>() {
	return ShaderDataType::Mat3x2;
}
template <> inline ShaderDataType GetShaderDataType<glm::mat3x4>() {
	return ShaderDataType::Mat3x4;
}
template <> inline ShaderDataType GetShaderDataType<glm::mat4>() {
	return ShaderDataType::Mat4;
}
template <> inline ShaderDataType GetShaderDataType<glm::mat4x2>() {
	return ShaderDataType::Mat4x2;
}
template <> inline ShaderDataType GetShaderDataType<glm::mat4x3>() {
	return ShaderDataType::Mat4x3;
}

template <> inline ShaderDataType GetShaderDataType<double>() {
	return ShaderDataType::Double;
}
template <> inline ShaderDataType GetShaderDataType<glm::dvec2>() {
	return ShaderDataType::Double2;
}
template <> inline ShaderDataType GetShaderDataType<glm::dvec3>() {
	return ShaderDataType::Double3;
}
template <> inline ShaderDataType GetShaderDataType<glm::dvec4>() {
	return ShaderDataType::Double4;
}
template <> inline ShaderDataType GetShaderDataType<glm::dmat2>() {
	return ShaderDataType::Dmat2;
}
template <> inline ShaderDataType GetShaderDataType<glm::dmat2x3>() {
	return ShaderDataType::Dmat2x3;
}
template <> inline ShaderDataType GetShaderDataType<glm::dmat2x4>() {
	return ShaderDataType::Dmat2x4;
}
template <> inline ShaderDataType GetShaderDataType<glm::dmat3>() {
	return ShaderDataType::Dmat3;
}
template <> inline ShaderDataType GetShaderDataType<glm::dmat3x2>() {
	return ShaderDataType::Dmat3x2;
}
template <> inline ShaderDataType GetShaderDataType<glm::dmat3x4>() {
	return ShaderDataType::Dmat3x4;
}
template <> inline ShaderDataType GetShaderDataType<glm::dmat4>() {
	return ShaderDataType::Dmat4;
}
template <> inline ShaderDataType GetShaderDataType<glm::dmat4x2>() {
	return ShaderDataType::Dmat4x2;
}
template <> inline ShaderDataType GetShaderDataType<glm::dmat4x3>() {
	return ShaderDataType::Dmat4x3;
}

template <> inline ShaderDataType GetShaderDataType<int>() {
	return ShaderDataType::Int;
}
template <> inline ShaderDataType GetShaderDataType<glm::ivec2>() {
	return ShaderDataType::Int2;
}
template <> inline ShaderDataType GetShaderDataType<glm::ivec3>() {
	return ShaderDataType::Int3;
}
template <> inline ShaderDataType GetShaderDataType<glm::ivec4>() {
	return ShaderDataType::Int4;
}

template <> inline ShaderDataType GetShaderDataType<unsigned int>() {
	return ShaderDataType::Uint;
}
template <> inline ShaderDataType GetShaderDataType<glm::uvec2>() {
	return ShaderDataType::Uint2;
}
template <> inline ShaderDataType GetShaderDataType<glm::uvec3>() {
	return ShaderDataType::Uint3;
}
template <> inline ShaderDataType GetShaderDataType<glm::uvec4>() {
	return ShaderDataType::Uint4;
}

template <> inline ShaderDataType GetShaderDataType<bool>() {
	return ShaderDataType::Bool;
}
template <> inline ShaderDataType GetShaderDataType<glm::bvec2>() {
	return ShaderDataType::Bool2;
}
template <> inline ShaderDataType GetShaderDataType<glm::bvec3>() {
	return ShaderDataType::Bool3;
}
template <> inline ShaderDataType GetShaderDataType<glm::bvec4>() {
	return ShaderDataType::Bool4;
}


/// <summary>
/// Represents the underlying element data type of a ShaderDataType
/// </summary>
ENUM(ShaderDataTypecode, uint32_t,
	None = 0,
	Float = 0b00000001'0000'0000'0000,
	Matrix = 0b00000010'0000'0000'0000,
	Int = 0b00000100'0000'0000'0000,
	Uint = 0b00001000'0000'0000'0000,
	Double = 0b00010000'0000'0000'0000,
	MatrixD = 0b00100000'0000'0000'0000,
	Bool = 0b01000000'0000'0000'0000,
	Texture = 0b10000000'0000'0000'0000
);

/// <summary>
/// Gets the underlying ShaderDataTypecode for the given ShaderDataType
/// This is the underlying element type for the shader data type (ex, float2->float)
/// </summary>
/// <param name="type">The shader data type to examine</param>
/// <returns>The ShaderDataTypecode corresponding to the type</returns>
constexpr ShaderDataTypecode GetShaderDataTypeCode(ShaderDataType type) {
	return (ShaderDataTypecode)((uint32_t)type & ShaderDataType_TypeMask);
}

/// <summary>
/// Gets the size of the underlying ShaderDataType in bytes
/// </summary>
/// <param name="type">The type to determine the size for</param>
/// <returns>The size in bytes, or 0 if the type is invalid</returns>
constexpr uint32_t ShaderDataTypeSize(ShaderDataType type)
{
	if (type == ShaderDataType::Uint64)
		return sizeof(uint64_t);

	ShaderDataTypecode typeCode = GetShaderDataTypeCode(type);
	switch (typeCode) {
	case ShaderDataTypecode::Float:
	case ShaderDataTypecode::Int:
	case ShaderDataTypecode::Uint:
		return 4 * ((uint32_t)type & ShaderDataType_Size1Mask);
	case ShaderDataTypecode::Matrix:
		return 4 * ((uint32_t)type & ShaderDataType_Size1Mask) * (((uint32_t)type & ShaderDataType_Size2Mask) >> 4);
	case ShaderDataTypecode::Double:
		return 8 * ((uint32_t)type & ShaderDataType_Size1Mask);
	case ShaderDataTypecode::MatrixD:
		return 8 * ((uint32_t)type & ShaderDataType_Size1Mask) * (((uint32_t)type & ShaderDataType_Size2Mask) >> 4);
	case ShaderDataTypecode::Bool:
		return (uint32_t)type & ShaderDataType_Size1Mask;
	default:
		LOG_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
}

/// <summary>
/// Gets the number of underlying components for a given ShaderDataType
/// </summary>
/// <param name="type">The type to determine the number of components for</param>
/// <returns>The number of components in the type</returns>
constexpr uint32_t ShaderDataTypeComponentCount(ShaderDataType type)
{
	ShaderDataTypecode typeCode = (ShaderDataTypecode)((uint32_t)type & ShaderDataType_TypeMask);
	switch (typeCode) {
	case ShaderDataTypecode::Float:
	case ShaderDataTypecode::Int:
	case ShaderDataTypecode::Uint:
	case ShaderDataTypecode::Double:
	case ShaderDataTypecode::Bool:
		return (uint32_t)type & ShaderDataType_Size1Mask;
	case ShaderDataTypecode::Matrix:
	case ShaderDataTypecode::MatrixD:
		return (uint32_t)type & ShaderDataType_Size1Mask * (uint32_t)type & ShaderDataType_Size2Mask;
		return (uint32_t)type & ShaderDataType_Size1Mask;
	default:
		LOG_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
}


/// <summary>
/// Handles mapping a GL data type to a ShaderDataType
/// </summary>					  CompressedRGBA
constexpr ShaderDataType FromGLShaderDataType(GLenum glType) {
	switch (glType) {
	case GL_FLOAT:				return ShaderDataType::Float;
	case GL_FLOAT_VEC2:			return ShaderDataType::Float2;
	case GL_FLOAT_VEC3:			return ShaderDataType::Float3;
	case GL_FLOAT_VEC4:			return ShaderDataType::Float4;
	case GL_FLOAT_MAT2:			return ShaderDataType::Mat2;
	case GL_FLOAT_MAT3:			return ShaderDataType::Mat3;
	case GL_FLOAT_MAT4:			return ShaderDataType::Mat4;
	case GL_FLOAT_MAT2x3:		return ShaderDataType::Mat2x3;
	case GL_FLOAT_MAT2x4:		return ShaderDataType::Mat2x4;
	case GL_FLOAT_MAT3x2:		return ShaderDataType::Mat3x2;
	case GL_FLOAT_MAT3x4:		return ShaderDataType::Mat3x4;
	case GL_FLOAT_MAT4x2:		return ShaderDataType::Mat4x2;
	case GL_FLOAT_MAT4x3:		return ShaderDataType::Mat4x3;
	case GL_INT:				return ShaderDataType::Int;
	case GL_INT_VEC2:			return ShaderDataType::Int2;
	case GL_INT_VEC3:			return ShaderDataType::Int3;
	case GL_INT_VEC4:			return ShaderDataType::Int4;
	case GL_UNSIGNED_INT:		return ShaderDataType::Uint;
	case GL_UNSIGNED_INT_VEC2:	return ShaderDataType::Uint2;
	case GL_UNSIGNED_INT_VEC3:	return ShaderDataType::Uint3;
	case GL_UNSIGNED_INT_VEC4:	return ShaderDataType::Uint4;
	case GL_UNSIGNED_INT64_ARB:	return ShaderDataType::Uint64;
	case GL_DOUBLE:				return ShaderDataType::Double;
	case GL_DOUBLE_VEC2:		return ShaderDataType::Double2;
	case GL_DOUBLE_VEC3:		return ShaderDataType::Double3;
	case GL_DOUBLE_VEC4:		return ShaderDataType::Double4;
	case GL_DOUBLE_MAT2:		return ShaderDataType::Dmat2;
	case GL_DOUBLE_MAT3:		return ShaderDataType::Dmat3;
	case GL_DOUBLE_MAT4:		return ShaderDataType::Dmat4;
	case GL_DOUBLE_MAT2x3:		return ShaderDataType::Dmat2x3;
	case GL_DOUBLE_MAT2x4:		return ShaderDataType::Dmat2x4;
	case GL_DOUBLE_MAT3x2:		return ShaderDataType::Dmat3x2;
	case GL_DOUBLE_MAT3x4:		return ShaderDataType::Dmat3x4;
	case GL_DOUBLE_MAT4x2:		return ShaderDataType::Dmat4x2;
	case GL_DOUBLE_MAT4x3:		return ShaderDataType::Dmat4x3;
	case GL_BOOL:				return ShaderDataType::Bool;
	case GL_BOOL_VEC2:			return ShaderDataType::Bool2;
	case GL_BOOL_VEC3:			return ShaderDataType::Bool3;
	case GL_BOOL_VEC4:			return ShaderDataType::Bool4;

		// Textures

	case GL_SAMPLER_1D:							return ShaderDataType::Tex1D;
	case GL_SAMPLER_1D_ARRAY:					return ShaderDataType::Tex1D_Array;
	case GL_SAMPLER_1D_SHADOW:					return ShaderDataType::Tex1D_Shadow;
	case GL_SAMPLER_1D_ARRAY_SHADOW:			return ShaderDataType::Tex1D_ShadowArray;

	case GL_SAMPLER_2D:							return ShaderDataType::Tex2D;
	case GL_SAMPLER_2D_RECT:					return ShaderDataType::Tex2D_Rect;
	case GL_SAMPLER_2D_RECT_SHADOW:				return ShaderDataType::Tex2D_Rect_Shadow;
	case GL_SAMPLER_2D_ARRAY:					return ShaderDataType::Tex2D_Array;
	case GL_SAMPLER_2D_SHADOW:					return ShaderDataType::Tex2D_Shadow;
	case GL_SAMPLER_2D_ARRAY_SHADOW:			return ShaderDataType::Tex2D_ShadowArray;
	case GL_SAMPLER_2D_MULTISAMPLE:				return ShaderDataType::Tex2D_Multisample;
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:		return ShaderDataType::Tex2D_MultisampleArray;

	case GL_SAMPLER_3D:							return ShaderDataType::Tex3D;
	case GL_SAMPLER_CUBE:						return ShaderDataType::TexCube;
	case GL_SAMPLER_CUBE_SHADOW:				return ShaderDataType::TexCubeShadow;

		// Buffer Textures

	case GL_SAMPLER_BUFFER:						return ShaderDataType::BufferTexture;
	case GL_INT_SAMPLER_BUFFER:					return ShaderDataType::BufferTextureInt;
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:		return ShaderDataType::BufferTextureUint;

		// Integer textures

	case GL_INT_SAMPLER_1D:						return ShaderDataType::Tex1D_Int;
	case GL_INT_SAMPLER_1D_ARRAY:				return ShaderDataType::Tex1D_Int_Array;
	case GL_INT_SAMPLER_2D:						return ShaderDataType::Tex2D_Int;
	case GL_INT_SAMPLER_2D_RECT:				return ShaderDataType::Tex2D_Int_Rect;
	case GL_INT_SAMPLER_2D_ARRAY:				return ShaderDataType::Tex2D_Int_Array;
	case GL_INT_SAMPLER_2D_MULTISAMPLE:			return ShaderDataType::Tex2D_Int_Multisample;
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:	return ShaderDataType::Tex2D_Int_MultisampleArray;

	case GL_INT_SAMPLER_3D:						return ShaderDataType::Tex3D_Int;
	case GL_INT_SAMPLER_CUBE:					return ShaderDataType::TexCube_Int;

		// Unsigned int textures

	case GL_UNSIGNED_INT_SAMPLER_1D:					return ShaderDataType::Tex1D_Uint;
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:				return ShaderDataType::Tex1D_Uint_Array;
	case GL_UNSIGNED_INT_SAMPLER_2D:					return ShaderDataType::Tex2D_Uint;
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:				return ShaderDataType::Tex2D_Uint_Rect;
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:				return ShaderDataType::Tex2D_Uint_Array;
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:		return ShaderDataType::Tex2D_Uint_Multisample;
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:	return ShaderDataType::Tex2D_Uint_MultisampleArray;

	case GL_UNSIGNED_INT_SAMPLER_3D:					return ShaderDataType::Tex3D_Uint;
	case GL_UNSIGNED_INT_SAMPLER_CUBE:					return ShaderDataType::TexCube_Uint;

	default: return ShaderDataType::None;
	}
}

/// <summary>
/// Handles mapping from a ShaderDataTypecode to an underlying GL data type
/// </summary>
constexpr GLenum ToGLElementType(ShaderDataTypecode typecode) {
	switch (typecode) {
	case ShaderDataTypecode::Float:
	case ShaderDataTypecode::Matrix:
		return GL_FLOAT;
	case ShaderDataTypecode::Int:
		return GL_INT;
	case ShaderDataTypecode::Uint:
		return GL_UNSIGNED_INT;
	case ShaderDataTypecode::Double:
	case ShaderDataTypecode::MatrixD:
		return GL_DOUBLE;
	case ShaderDataTypecode::Bool:
		return GL_BOOL;
	default:
		LOG_ASSERT(false, "Unknown Shader Data Typecode!"); return 0;
	}
}
