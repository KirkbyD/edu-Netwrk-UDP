#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct sData {
	void clear();
	sData();

	size_t getResult();
	size_t getSubsystem();
	size_t getCMD();
	size_t getUniqueID();
	std::string getTarget();
	std::string getSource();
	std::vector<glm::vec4> getVec4Data();
	std::vector<glm::quat> getQuatData();
	std::vector<float> getFloatData();

	bool isEase();
	bool isEaseDistance();
	bool isEaseTime();

	void setResult(size_t result);
	void setSubsystem(size_t subsystem);
	void setCMD(size_t cmd);
	void setUniqueID(size_t id);
	void setTarget(std::string target);
	void setSource(std::string source);

	void addVec4Data(float xr, float yg, float zb, float wa = 1.0f);
	void addVec4Data(glm::vec3 data, float wa = 1.0f);
	void addVec4Data(glm::vec4 data);
	void addQuatData(float x, float y, float z);
	void addQuatData(glm::vec3 xyz);
	void addQuatData(glm::quat data);
	void addFloatData(float f);

	void setEase(bool b, std::string dt);

	void setVec4Data(float xr, float yg, float zb, float wa = 1.0f, int idx = 0);
	void setVec4Data(glm::vec3 data, float wa = 1.0f, int idx = 0);
	void setVec4Data(glm::vec4 data, int idx = 0);
	void setQuatData(float x, float y, float z, int idx = 0);
	void setQuatData(glm::vec3 xyz, int idx = 0);
	void setQuatData(glm::quat data, int idx = 0);

private:
	size_t _result;
	size_t _subsystem;
	size_t _cmd;
	size_t _uniqueID;
	std::string _target;
	std::string _source;
	std::vector<glm::vec4> _v4Data;
	std::vector<glm::quat> _quatData;
	std::vector<float> _fData;
	std::vector<double> _dData;		// CURRENTLY UNUSED
	std::vector<int> _iData;		// CURRENTLY UNUSED
	std::vector<size_t> _uData;		// CURRENTLY UNUSED

	bool _isEase, _isEaseDistance, _isEaseTime;
};
