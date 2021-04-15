#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

struct sPlyVertexXYZ_N_UV {
	float x, y, z;
	float nx, ny, nz;
	float u, v;				// NOW with TEXTURE COORDINATES!!!
};

struct sPlyTriangle {
	unsigned int vert_index_1;
	unsigned int vert_index_2;
	unsigned int vert_index_3;
};

class cMesh {
public:
	std::vector<sPlyVertexXYZ_N_UV> vecVertices;
	std::vector<sPlyTriangle> vecTriangles;
};
