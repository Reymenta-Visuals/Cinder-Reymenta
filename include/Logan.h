#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/DataSource.h"
#include "cinder/Utilities.h"
// log
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta
{
	// stores the pointer to the Logan instance
	typedef std::shared_ptr<class Logan> LoganRef;

	class Logan {
	public:		
		Logan();
		virtual				~Logan();

		void				logString(std::string msg);

		static LoganRef	create()
		{
			return shared_ptr<Logan>(new Logan());
		}

	private:

	};


}