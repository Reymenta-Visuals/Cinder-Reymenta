#include "VDParams.h"

using namespace videodromm;

VDParamsRef VDParams::PARAMS;
VDParamsRef	VDParams::create() {
	if (PARAMS.use_count() == 0) {
		PARAMS = VDParamsRef(new VDParams());
	}
	return PARAMS;
}

