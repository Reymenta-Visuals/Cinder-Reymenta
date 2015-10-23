#include "Logan.h"

using namespace Reymenta;
	
Logan::Logan()
{
	fs::path path;
	path = getAssetPath("") / "log" ;
	auto bLog = log::makeLogger<log::LoggerBreakpoint>();
	bLog->setTriggerLevel(log::LEVEL_ERROR);
	log::makeLogger<log::LoggerFileRotating>(path.string(), "reymenta.%Y.%m.%d.txt", false);
}
