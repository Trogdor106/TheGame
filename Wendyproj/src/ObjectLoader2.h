#pragma once
#include "HitBoxes.h"

#include "Mesh.h"

Mesh::Sptr Bake(MeshData& data);
void ComputeTBN(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, size_t begin = 0, size_t end = 0xFFFFFFFFFFFFFFFFu);

struct MeshAndHitBox {
	MeshData mesh;
	HitBox hitBox;
};

class ObjLoader {
public:
	static MeshData LoadObj(const char* filename, glm::vec4 baseColor);
	static MeshAndHitBox LoadObj2(const char* filename, glm::vec4 baseColor);
};