#pragma once

#include "cinder/app/AppNative.h"
#include "OscListener.h"
#include "OSCSender.h"
#include "Resources.h"
#include "ParameterBag.h"
//#include "WindowsManager.h"
#include "Shaders.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class OSC> OSCRef;

	class OSC 
	{

	public:
		OSC( ParameterBagRef aParameterBag, ShadersRef aShadersRef );
		static	OSCRef create( ParameterBagRef aParameterBag, ShadersRef aShadersRef );

		void	update();

		void	sendOSCIntMessage( string controlType, int controlName, int controlValue0, int controlValue1 );
		void	sendOSCStringMessage( string controlType, string controlString );
		void	rotaryChange( int name, float newValue );
		void	toggleChange(int name, float newValue);
		Vec4i	skeleton[20];
	private:

		void oscProcessMessage( int controlName, int arg0, int arg1 );
		// parameters
		ParameterBagRef mParameterBag;
		// Shaders
		ShadersRef					mShaders;

		osc::Listener 				mOSCReceiver;
		osc::Sender					mOSCSender;


	};
}