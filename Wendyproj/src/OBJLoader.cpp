#include "OBJLoader.h"

std::vector <Vertex> loadOBJ(
	const char* path
)
{
	std::vector <Vertex> toReturn;
	//	vertex indices, uvs, normals
	std::vector<unsigned int> v_Indices, uv_Indices, n_Indices;
	std::vector<glm::vec3> t_vert;
	std::vector<glm::vec2> t_uvs;
	std::vector<glm::vec3> t_norms;

	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Can't open file!\n");
		exit(1);
	}
	while (1) {
		char lineHeader[128];
		//read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break;
		}
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			t_vert.push_back(vertex);

		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			t_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			t_norms.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int v_Index[3], uv_Index[3], n_Index[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &v_Index[0], &uv_Index[0], &n_Index[0], &v_Index[1], &uv_Index[1], &n_Index[1], &v_Index[2], &uv_Index[2], &n_Index[2]);
			if (matches != 9) {
				printf("File can't be read by the parser, TOO COMPLEX!\n");
				exit(1);
			}
			v_Indices.push_back(v_Index[0]);
			v_Indices.push_back(v_Index[1]);
			v_Indices.push_back(v_Index[2]);
			uv_Indices.push_back(uv_Index[0]);
			uv_Indices.push_back(uv_Index[1]);
			uv_Indices.push_back(uv_Index[2]);
			n_Indices.push_back(n_Index[0]);
			n_Indices.push_back(n_Index[1]);
			n_Indices.push_back(n_Index[2]);

		}

	}
	for (unsigned int i = 0; i < v_Indices.size(); i++) {
		unsigned int v_Index = v_Indices[i];
		unsigned int t_Index = uv_Indices[i];
		unsigned int n_Index = n_Indices[i];
		glm::vec3 vertex = t_vert[v_Index - 1];
		glm::vec2 uv = t_Index != 0 ? t_uvs[t_Index - 1] : glm::vec2();
		glm::vec3 norm = n_Index != 0 ? t_norms[n_Index - 1] : glm::vec3(); //Calculate norms with 3 pos, implement the calculation for more precision
		Vertex toOut;
		toOut.Position = vertex;
		toOut.Color = glm::vec4(1.0f);
		toOut.UV = uv;
		toOut.Normal = norm;
		toReturn.push_back(toOut);
	}
	return toReturn;
}


