#include "cCallbacks.hpp"
#include <glm/glm.hpp>

void error_callback(int error, const char* description) {
	// CHANGE TO CALL THE ERROR PIMPL
	fprintf(stderr, "Error: %s\n", description);
}