#include "sNVData.hpp"

void sNVData::clear() {
	this->subsystem = "";
	this->cmd = "";
	this->target = "";
	this->state = "";
	this->mesh = "";
	this->name = "";
	this->status = "";
	this->pmo = nullptr;
	this->plo = nullptr;

	// FILE OUTPUT DATA RESET
	this->_models_filename = "";
	this->_lights_filename = "";
	this->_complex_filename = "";

	return;
}

sNVData::sNVData() {
	this->clear();
	return;
}
