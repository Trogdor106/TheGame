#include "Vertex.h"

std::size_t BufferElementHash::operator()(const BufferElement& value) const {
	size_t hash = std::hash<std::string>()(value.Name);
	hash ^= (std::hash<ShaderDataType>()(value.Type)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= (std::hash<uint32_t>()(value.SizeInBytes)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= (std::hash<uint32_t>()(value.Offset)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= (std::hash<bool>()(value.IsNormalized)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= (std::hash<uint32_t>()(value.ArraySize)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return  hash;
}

void BufferLayout::__Calculate() {
	myStride = 0;
	myHash = myElements.size(); // start with non-zero hash

	// Iterate over all buffer elements
	for (unsigned int ix = 0; ix < myElements.size(); ix++) {
		// Abort if a texture is trying to be sent via a vertex attribute
		if (GetShaderDataTypeCode(myElements[ix].Type) == ShaderDataTypecode::Texture) {
			LOG_ASSERT(false, "Attempting to use resource type as vertex attribute is not supported! Aborting!");
			myStride = 0;
			myElements.clear();
			return;
		}
		// Update the element's offset
		myElements[ix].Offset = myStride;

		myStride += myElements[ix].SizeInBytes * myElements[ix].ArraySize;
		// Hashing function
		myHash ^= (BufferElementHash()(myElements[ix])) + 0x9e3779b9 + (myHash << 6) + (myHash >> 2);
	}
}