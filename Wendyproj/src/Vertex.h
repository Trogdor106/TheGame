#pragma once
#include <GLM/glm.hpp> // For vec3 and vec4
#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include "ShaderData.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec3 Normal;
	glm::vec2 UV;
	glm::vec3 Tangent;
	glm::vec3 BiTangent;

	Vertex(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 norm = glm::vec3(0.0f), glm::vec2 uv = glm::vec2(0.0f)) :
		Position(pos),
		Color(glm::vec4(1.0f)),
		Normal(norm),
		UV(uv), 
		Tangent(glm::vec3(0.0f)),
		BiTangent(glm::vec3(0.0f)) { }
};

struct MeshData {
	std::vector<Vertex>   Vertices;
	std::vector<uint32_t> Indices;
	std::vector<glm::vec2> UV;
	std::string           DebugName;
};




//BufferLayout


/// <summary>
/// Represents a single element in a buffer or vertex declaration
/// </summary>


ENUM(VertexUsage, uint32_t,
	User,
	Position,
	Normal,
	Tangent,
	Bitangent,
	Texture,
	Color
);

struct BufferElement {
	std::string    Name;
	ShaderDataType Type;
	uint32_t       ArraySize;
	uint32_t       SizeInBytes;
	uint32_t       Offset;
	bool           IsNormalized;
	VertexUsage    Usage;

	BufferElement() = default;

	/*
	 * Creates a new buffer element
	 * @param name The name of the attribute in the shader
	 * @param type The type for this attribute
	 * @param normalized Whether or not to normalize integer types to the [-1 -> 1] range (or [0 -> 1] for unsigned)
	 */
	BufferElement(const std::string& name, ShaderDataType type, VertexUsage usage = VertexUsage::User, bool normalized = false, uint32_t arraySize = 1) :
		Name(name), Type(type), SizeInBytes(ShaderDataTypeSize(type)), Offset(0), IsNormalized(normalized), ArraySize(arraySize), Usage(usage) {}

	/*
	 * Returns the number of components that this buffer element has
	 * ex: a vec3 component will return 3
	 */
	uint32_t GetComponentCount() const { return ShaderDataTypeComponentCount(Type); }

	/*
	 * Compares this buffer element to another for equality
	 * @returns True if this element is exactly equal to the other, otherwise false
	 */
	inline bool operator ==(const BufferElement& other) const;
	/*
	 * Compares this buffer element to another for inequality
	 * @returns False if this element is exactly equal to the other, otherwise true
	 */
	inline bool operator !=(const BufferElement& other) const { return !(*this == other); }
};

/*
 * Used to calculate hash codes for buffer elements
 */
struct BufferElementHash {
	/*
	 * Computes a hash for the given buffer element
	 * @param value The value to hash
	 * @returns A semi-unique hash for the given element
	 */
	std::size_t operator()(const BufferElement& value) const;
};

/*
 * Represents the layout of a buffer. This stores all of the attributes that define a single
 * object within the buffer
 */
class BufferLayout {
public:
	BufferLayout() = default;

	/*
	 * Creates a new buffer layout from an initializer list
	 */
	BufferLayout(const std::initializer_list<BufferElement>& elements)
		: myStride(0), myHash(0), isSTD140(isSTD140), myElements(elements) {
		__Calculate();
	}

	/*
	 * Creates a new buffer layout from a vector introspected from a shader
	 */
	BufferLayout(const std::vector<BufferElement>& elements)
		: myStride(0), myHash(0), isSTD140(isSTD140), myElements(elements) {
		__Calculate();
	}

	/*
	 * Gets the stride of a this buffer layout. i.e. the number of bytes that a single object occupies
	 */
	uint32_t GetStride() const { return myStride; }
	/*
	 * Gets the number of elements / attributes in this buffer layout
	 */
	uint32_t ElementCount() const { return (uint32_t)myElements.size(); }

	/*
	 * Gets the buffer element with the given vertex usage, stores the element in result
	 * @param usage The vertex usage to search for
	 * @param result A reference to the value to store the result in
	 * @param index The index of the usage to get
	 * @returns True if an element was found matching the usage and index, false if otherwise
	 */
	bool GetElementByUsage(VertexUsage usage, BufferElement& result, int index = 0) const;

	/*
	 * Gets the beginning of this layout for iteration
	 */
	std::vector<BufferElement>::const_iterator begin() const { return myElements.begin(); }
	/*
	 * Gets the end of this layout for iteration
	 */
	std::vector<BufferElement>::const_iterator end() const { return myElements.end(); }

	inline bool operator ==(const BufferLayout& other) const;
	inline bool operator !=(const BufferLayout& other) const { return !(*this == other); }

protected:
	uint32_t myStride;
	size_t   myHash;
	bool     isSTD140;
	std::vector<BufferElement> myElements;

	void __Calculate();

	inline bool DeepCompare(const BufferLayout& other) const;
};




//struct Vertex {
//	glm::vec3 Position;
//	glm::vec4 Color;
//	glm::vec3 Normal;
//	// New in tutorial 08    
//	glm::vec2 UV;
//};