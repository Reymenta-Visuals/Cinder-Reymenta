#include "VDLog.h"

using namespace videodromm;

VDLogRef VDLog::LOGGER;
VDLogRef	VDLog::create() {
	if (LOGGER.use_count() == 0) {
		LOGGER = VDLogRef(new VDLog());
	}
	return LOGGER;
}
VDLog::VDLog()
{
	auto sysLogger = log::makeLogger<log::LoggerSystem>();
	//sysLogger->setLoggingLevel(log::LEVEL_WARNING);

	// only log to file in debug mode
#ifdef _DEBUG
	log::makeLogger<log::LoggerFileRotating>("/tmp/vd", "vd.%Y.%m.%d.txt", false);
#endif  // _DEBUG
}
