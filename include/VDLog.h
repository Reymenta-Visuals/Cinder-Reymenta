/*
	VDLog
	Handles logging to the filesystem and console.
*/
// rewriten as a singleton

#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/DataSource.h"
#include "cinder/Utilities.h"
// log
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;

namespace videodromm
{
	// stores the pointer to the VDLog instance
	typedef std::shared_ptr<class VDLog> VDLogRef;

	class VDLog {
	public:		

		static VDLogRef	create();

	private:
		static VDLogRef LOGGER;
		VDLog();
	};


}
