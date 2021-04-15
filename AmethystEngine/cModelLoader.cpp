#include "cModelLoader.hpp"

#include <iostream>
#include <fstream>

bool cModelLoader::LoadPlyModel (std::string filename, cMesh& theMesh) {
	std::ifstream theFile(filename.c_str());
	if (!theFile.is_open())
		return false;

	std::string temp;

	while (theFile >> temp) {
		if (temp == "vertex")
			break;
	}

	unsigned int numberOfVertices;
	theFile >> numberOfVertices;

	while (theFile >> temp) {
		if (temp == "face")
			break;
	}

	unsigned int numberOfTriangles;
	theFile >> numberOfTriangles;

	while (theFile >> temp) {
		if (temp == "end_header")
			break;
	}

	for (unsigned int index = 0; index != numberOfVertices; index++) {
		sPlyVertexXYZ_N_UV tempVertex;

		theFile >> tempVertex.x >> tempVertex.y >> tempVertex.z;

		theFile >> tempVertex.nx >> tempVertex.ny >> tempVertex.nz;

		// Also load the texture coordinates
		theFile >> tempVertex.u >> tempVertex.v;

		theMesh.vecVertices.push_back(tempVertex);
	}

	for (unsigned int index = 0; index != numberOfTriangles; index++) {
		int discardThis;
		sPlyTriangle tempTriangle;

		theFile >> discardThis
			>> tempTriangle.vert_index_1
			>> tempTriangle.vert_index_2
			>> tempTriangle.vert_index_3;

		theMesh.vecTriangles.push_back(tempTriangle);
	}
	return true;
}
