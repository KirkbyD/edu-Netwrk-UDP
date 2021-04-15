#pragma once
#include "iCommand.hpp"
#include "cCommandManager.hpp"

class cRotate : public iCommand {
	bool _UpdateHasBeenCalled;
	bool _isRelative;
	size_t _subsystem;
	std::string _target;
	glm::quat _start_orientation;
	glm::quat _current_orientation;
	glm::quat _end_orientation;
	glm::quat _rotational_speed;
	float _time;
	double _time_elapsed;
	float _easeIn, _easeOut;

	float Clamp(float x);

public:
	// move time
	cRotate() {
		_UpdateHasBeenCalled = false;
		_isRelative = true;
	}

	virtual void Init(sData data);
	virtual bool Update(double deltatime);
	virtual float SmootherStep(float startdist, float enddist, float currdist);
};
