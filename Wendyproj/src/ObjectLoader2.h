#pragma once

#include "Mesh.h"

Mesh::Sptr Bake(MeshData& data);
void ComputeTBN(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, size_t begin = 0, size_t end = 0xFFFFFFFFFFFFFFFFu);


class ObjLoader {
public:
	static MeshData LoadObj(const char* filename, glm::vec4 baseColor);
};

