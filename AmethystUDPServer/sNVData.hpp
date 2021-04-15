#pragma once
#include <string>
#include <vector>

class cModelObject;
class cLightObject;
struct sNVData {
	// CLEAR FUNCTION
	void clear();

	// CTORS
	sNVData();

	// VARIABLES ( MORE TO BE ADDED )
	std::string subsystem;
	std::string cmd;
	std::string target;
	std::string state;
	std::string mesh;
	std::string name;
	std::string status;

	cModelObject* pmo;
	cLightObject* plo;
	std::vector<cModelObject*> vcmo;
	std::vector<cLightObject*> vclo;

	// FILE OUTPUT DATA
	std::string _models_filename;
	std::string _lights_filename;
	std::string _complex_filename;

};
