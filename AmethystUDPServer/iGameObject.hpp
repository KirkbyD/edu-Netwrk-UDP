#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <map>

class iGameObject {
public:
	unsigned int friendlyIDNumber;
	std::string friendlyName;

	bool isVisible;
	bool isControl;
	
	iGameObject* parentObject;

	virtual unsigned int getUniqueID(void) = 0;

	virtual glm::vec3 getPosition(void) = 0;

protected:
	static unsigned int next_uniqueID;
	unsigned int m_uniqueID;
};

