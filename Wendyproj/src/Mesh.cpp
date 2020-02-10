#include "Mesh.h"

/*
Mesh::Mesh(Vertex* vertices, size_t numVerts, uint32_t* indices, size_t numIndices, Vertex* vertices2, size_t numVerts2, uint32_t* indices2, size_t numIndices2) {
	myIndexCount = numIndices;
	myVertexCount = numVerts;

	// Create and bind our vertex array
	glCreateVertexArrays(1, &myVao);
	glBindVertexArray(myVao);

	// Create 2 buffers, 1 for vertices and the other for indices
	glCreateBuffers(2, myBuffers);
	glCreateBuffers(2, myBuffers2);


	// Bind and buffer our vertex data
	glBindBuffer(GL_ARRAY_BUFFER, myBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, myBuffers2[0]);
	glBufferData(GL_ARRAY_BUFFER, numVerts2 * sizeof(Vertex), vertices2, GL_STATIC_DRAW);
	
	// Bind and buffer our index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBuffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	// Bind and buffer our index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBuffers2[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices2 * sizeof(uint32_t), indices2, GL_STATIC_DRAW);

	// Get a null vertex to get member offsets from
	Vertex* vert = nullptr;

	// Enable vertex attribute 0
	glEnableVertexAttribArray(0);
	// Our first attribute is 3 floats, the distance between 
	// them is the size of our vertex, and they will map to the position in our vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), &(vert->Position));
	//glVertexAttribBinding(0, 0);

	// Enable vertex attribute 1
	glEnableVertexAttribArray(1);
	// Our second attribute is 4 floats, the distance between 
	// them is the size of our vertex, and they will map to the color in our vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex), &(vert->Color));
	//glVertexAttribBinding(0, 1);

	// Enable vertex attribute 1
	glEnableVertexAttribArray(2);
	// Our second attribute is 4 floats, the distance between 
	// them is the size of our vertex, and they will map to the color in our vertices
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), &(vert->Normal));
	//glVertexAttribBinding(0, 2);

	// New in tutorial 08    
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, false, sizeof(Vertex), &(vert->UV));
	//glVertexAttribBinding(0, 3);

	// Our first attribute is 3 floats, the distance between 
	// them is the size of our vertex, and they will map to the position in our vertices
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof(Vertex), &(vert->Position));
	//glVertexAttribBinding(1, 4);

	// Enable vertex attribute 1
	glEnableVertexAttribArray(5);
	// Our second attribute is 4 floats, the distance between 
	// them is the size of our vertex, and they will map to the color in our vertices
	glVertexAttribPointer(5, 4, GL_FLOAT, false, sizeof(Vertex), &(vert->Color));
	//glVertexAttribBinding(1, 5);

	// Enable vertex attribute 1
	glEnableVertexAttribArray(6);
	// Our second attribute is 4 floats, the distance between 
	// them is the size of our vertex, and they will map to the color in our vertices
	glVertexAttribPointer(6, 3, GL_FLOAT, false, sizeof(Vertex), &(vert->Normal));
	//glVertexAttribBinding(1, 6);

	// New in tutorial 08    
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 2, GL_FLOAT, false, sizeof(Vertex), &(vert->UV));
	//glVertexAttribBinding(1, 7);

	//GLuint ptrs[] = { myBuffers[0], myBuffers2[0] };
	//GLintptr offsets[] = { 0,0 };
	//int strides[] = { sizeof(Vertex),sizeof(Vertex) };
	//glBindVertexBuffers(0, 2, ptrs, offsets, strides);
	

	// Unbind our VAO
	glBindVertexArray(0);
}
*/
Mesh::Mesh(void* vertices, size_t numVerts, const BufferLayout& layout, uint32_t* indices, size_t numIndices) {
	myIndexCount = numIndices;
	myVertexCount = numVerts;

	// Cache the layout
	myLayout = layout;

	// Create and bind our vertex array
	glCreateVertexArrays(1, &myRendererID);
	glBindVertexArray(myRendererID);

	// Create 2 buffers, 1 for vertices and the other for indices
	glCreateBuffers(2, myBuffers);

	// Bind and buffer our vertex data
	glBindBuffer(GL_ARRAY_BUFFER, myBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * layout.GetStride(), vertices, GL_STATIC_DRAW);

	// Bind and buffer our index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBuffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t), indices, GL_STATIC_DRAW);

	// Our elements will be sequential in the shaders (so attrib 0, 1, 2 ...)
	uint32_t index = 0;
	// Iterate over all elements
	for (const BufferElement& element : layout) {
		// Enable the attribute
		glEnableVertexAttribArray(index);
		// Set up the vertex attribute
		glVertexAttribPointer(index,
			element.GetComponentCount(), // Number of components in the attribute
			ToGLElementType(GetShaderDataTypeCode(element.Type)), // The data type of the attribute
			element.IsNormalized, // Whether or not the element is normalized
			layout.GetStride(),
			(const void*)element.Offset);
		index++;
	}

	// Unbind our VAO
	glBindVertexArray(0);
}

void* Mesh::ExtractVertices(size_t& outSize) const {
	outSize = myLayout.GetStride() * myVertexCount;
	void* result = malloc(outSize);
	glGetNamedBufferSubData(myBuffers[0], 0, outSize, result);
	return result;
}

uint32_t* Mesh::ExtractIndices(size_t& outSize) const {
	outSize = sizeof(uint32_t) * myIndexCount;
	void* data = malloc(outSize);
	glGetNamedBufferSubData(myBuffers[1], 0, outSize, data);
	return (uint32_t*)data;
}

Mesh::~Mesh() {
	LOG_INFO("Deleting mesh with ID: {}", myRendererID);
	// Clean up our buffers
	glDeleteBuffers(2, myBuffers);
	// Clean up our VAO
	glDeleteVertexArrays(1, &myRendererID);
}

void Mesh::Draw() {
	// Bind the mesh
	glBindVertexArray(myRendererID);
	if (myIndexCount > 0)
		// Draw all of our vertices as triangles, our indexes are unsigned ints (uint32_t)
		glDrawElements(GL_TRIANGLES, myIndexCount, GL_UNSIGNED_INT, nullptr);
	else
		glDrawArrays(GL_TRIANGLES, 0, myVertexCount);
}