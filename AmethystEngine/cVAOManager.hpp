#pragma once
#include "GLCommon.hpp"
#include <string>
#include <map>
#include "cMesh.hpp"

// The vertex structure that's ON THE GPU (eventually) So dictated from THE SHADER
struct sVertex {
	float x, y, z, w;			// w coordinate	
	float r, g, b, a;	// a = alpha (transparency)
	float nx, ny, nz, nw;
	float u0, v0, u1, v1;
};


struct sModelDrawInfo {
	sModelDrawInfo();

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// The "local" (i.e. "CPU side" temporary array)
	sVertex* pVertices;	//  = 0;
	// The index buffer (CPU side)
	unsigned int* pIndices;

	// You could store the max and min values of the vertices here (determined when you load them):
	float maxX, maxY, maxZ;
	float minX, minY, minZ;

	//	scale = 5.0/maxExtent;		-> 5x5x5
	float maxExtent;
	float extentX, extentY, extentZ;

	void CalcExtents(void);
};


class cVAOManager {
public:

	// Takes a cMesh object and copies it into the GPU (as a VOA)
	bool LoadModelIntoVAO(std::string fileName, cMesh& theMesh, sModelDrawInfo& drawInfo, unsigned int shaderProgramID);

	// We don't want to return an int, likely
	bool FindDrawInfoByModelName(std::string filename, sModelDrawInfo& drawInfo);

	std::string getLastError(bool bAndClear = true);

private:
	std::map< std::string /*model name*/, sModelDrawInfo /* info needed to draw*/ > m_map_ModelName_to_VAOID;
};
