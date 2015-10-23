#include "Logan.h"

using namespace Reymenta;
	
Logan::Logan()
{
	fs::path path;
	path = getAssetPath("") / "log" ;
	auto bLog = log::makeLogger<log::LoggerBreakpoint>();
	bLog->setTriggerLevel(log::LEVEL_ERROR);
	log::makeLogger<log::LoggerFileRotating>(path.string(), "reymenta.%Y.%m.%d.log", false);
}

Logan::~Logan()
{
	logString("logger quit");
}

void Logan::logString(std::string msg)
{
	CI_LOG_V(msg);
}

