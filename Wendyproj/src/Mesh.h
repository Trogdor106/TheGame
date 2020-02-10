#pragma once

#include "Vertex.h"
#include <glad/glad.h>
#include <GLM/glm.hpp> // For vec3 and vec4
#include <cstdint> // Needed for uint32_t
#include <memory> // Needed for smart pointers
#include <string>
#include "IGraphicsResource.h"

class Mesh : public IGraphicsResource {
public:
	// Shorthand for shared_ptr
	GraphicsClass(Mesh);

	/*
	 * Create a new mesh from some arbitrary data
	 * @param vertices The data to load into the mesh's buffer
	 * @param numVerts the number of vertices to load
	 * @param layout The layout of the data that will be loaded into the mesh
	 * @param indices The indices to load into the index buffer for the mesh, or nullptr for no indexing
	 * @param numIndices The number of indices to load into the index buffer
	 */
	Mesh(void* vertices, size_t numVerts, const BufferLayout& layout, uint32_t* indices, size_t numIndices);
	~Mesh();

	// Draws this mesh
	void Draw();

	size_t GetVertexCount() const { return myVertexCount; }
	size_t GetIndexCount() const { return myIndexCount; }
	size_t GetTriangleCount() const { return myIndexCount / 3ul; }

	// Extracts the vertex data from this mesh, uses malloc
	void* ExtractVertices(size_t& outSize) const;
	// Extracts the index data from this mesh, uses malloc
	uint32_t* ExtractIndices(size_t& outSize) const;

	// Gets the layout of the data that is stored in this mesh
	const BufferLayout& GetLayout() const { return myLayout; }

	// Debug / editor name
	std::string Name;

private:
	// 0 is vertices, 1 is indices
	GLuint myBuffers[2];
	// The number of vertices and indices in this mesh
	size_t myVertexCount, myIndexCount;
	// How this mesh is arranged in memory
	BufferLayout myLayout;
};