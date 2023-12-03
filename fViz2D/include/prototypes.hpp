#pragma once

#include "miscStdHeaders.h"

namespace GUI { 
	typedef struct menuDefinition_st menuDefinition_t; 
	typedef std::string filenameCallback_func(int steps, bool calledFromGui);
}

namespace F_V2 {
	void defaultSynchCallback();
}