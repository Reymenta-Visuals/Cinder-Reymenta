#pragma once

#include "cinder/app/App.h"
#include "Resources.h"
#include "ParameterBag.h"
// textures
#include "Textures.h"
// shaders
#include "Shaders.h"

#include "cinder/Json.h"
// MIDI
#include "CinderMidi.h"

// OSC
#include "OscListener.h"
#include "OSCSender.h"

// WebSockets
#include "WebSocketClient.h"
#include "WebSocketServer.h"
// imgui
#include "CinderImGui.h"
// lz4 
#include "lz4.h"
//#include <stdio.h>
//#include <vector>

#define IMGUI_REMOTE_KEY_FRAME    60  // send keyframe every 30 frames
#define IMGUI_REMOTE_INPUT_FRAMES 120 // input valid during 120 frames

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class MessageRouter> MessageRouterRef;
	// midi
	typedef std::shared_ptr<class MIDI> MIDIRef;
	struct midiInput
	{
		string			portName;
		bool			isConnected;
	};
	// imgui
	//------------------
	// ImGuiRemoteInput
	// - a structure to store input received from remote imgui, so you can use it on your whole app (keys, mouse) or just in imgui engine
	// - use GetImGuiRemoteInput to read input data safely (valid for IMGUI_REMOTE_INPUT_FRAMES)
	//------------------
	struct RemoteInput
	{
		ImVec2	MousePos;
		int		MouseButtons;
		int		MouseWheel;
		bool	KeyCtrl;
		bool	KeyShift;
		bool	KeysDown[256];
	};
	struct Cmd
	{
		int   vtx_count;
		float clip_rect[4];
		void Set(const ImDrawCmd &draw_cmd)
		{
			vtx_count = draw_cmd.ElemCount;
			clip_rect[0] = draw_cmd.ClipRect.x;
			clip_rect[1] = draw_cmd.ClipRect.y;
			clip_rect[2] = draw_cmd.ClipRect.z;
			clip_rect[3] = draw_cmd.ClipRect.w;
			//printf("DrawCmd: %d ( %.2f, %.2f, %.2f, %.2f )\n", vtx_count, clip_rect[0], clip_rect[1], clip_rect[2], clip_rect[3]);
		}
	};
	struct Vtx
	{
		short x, y; // 16 short
		short u, v; // 16 fixed point
		unsigned char r, g, b, a; // 8*4
		void Set(const ImDrawVert &vtx)
		{
			x = (short)(vtx.pos.x);
			y = (short)(vtx.pos.y);
			u = (short)(vtx.uv.x * 32767.f);
			v = (short)(vtx.uv.y * 32767.f);
			r = (vtx.col >> 0) & 0xff;
			g = (vtx.col >> 8) & 0xff;
			b = (vtx.col >> 16) & 0xff;
			a = (vtx.col >> 24) & 0xff;
		}
	};
	class MessageRouter
	{

	public:
		MessageRouter(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef);
		static	MessageRouterRef create(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef);
		void						update();
		// messages
		void						sendJSON(string params);
		void						updateParams(int iarg0, float farg1);
		// MIDI
		void						midiSetup();
		int							getMidiInPortsCount() { return mMidiInputs.size(); };
		string						getMidiInPortName(int i) { return (i<mMidiInputs.size()) ? mMidiInputs[i].portName : "No midi in ports"; };
		bool						isMidiInConnected(int i) { return (i<mMidiInputs.size()) ? mMidiInputs[i].isConnected : false; };
		void						openMidiInPort(int i);
		void						closeMidiInPort(int i);
		void						shutdown();
		// OSC
		void						setupOSCSender();
		void						sendOSCIntMessage(string controlType, int iarg0 = 0, int iarg1 = 0, int iarg2 = 0, int iarg3 = 0, int iarg4 = 0, int iarg5 = 0);
		void						sendOSCColorMessage(string controlType, float val);
		void						sendOSCFloatMessage(string controlType, int iarg0 = 0, float farg1 = 0.0, float farg2 = 0.0, float farg3 = 0.0, float farg4 = 0.0, float farg5 = 0.0);
		void						updateAndSendOSCFloatMessage(string controlType, int iarg0 = 0, float farg1 = 0.0, float farg2 = 0.0, float farg3 = 0.0, float farg4 = 0.0, float farg5 = 0.0);
		void						sendOSCStringMessage(string controlType, int iarg0 = 0, string sarg1 = "", string sarg2 = "", string sarg3 = "", string sarg4 = "", string sarg5 = "");
		ivec4						skeleton[20];
		string						getTrack(int i) { return tracks[min(i, MAX)]; };//TODO
		// WebSockets
		void						wsWrite(std::string msg);
		void						wsWriteBinary(const void *data, int size);
		void						wsConnect();
		void						wsPing();
		bool						isWsClientConnected() { return clientConnected; };
		void						colorWrite();
		// WebSockets for remoteimgui
		void						PreparePacketFrame(unsigned int cmd_count, unsigned int vtx_count);
		void						Write(Cmd const &cmd);
		void						Write(Vtx const &vtx);
		void						SendPacket();
		bool						isRemoteClientActive() { return remoteClientActive; };
		// utils
		void						selectShader(bool left, int index);
	private:
		// parameters
		ParameterBagRef				mParameterBag;
		// Shaders
		ShadersRef					mShaders;
		// Textures
		TexturesRef					mTextures;
		// MIDI
		vector<midiInput>			mMidiInputs;
		void						midiListener(midi::MidiMessage msg);
		string						midiControlType;
		int							midiControl;
		int							midiPitch;
		int							midiVelocity;
		float						midiNormalizedValue;
		int							midiValue;
		int							midiChannel;
		// midi inputs: couldn't make a vector
		midi::MidiInput				mMidiIn0;
		midi::MidiInput				mMidiIn1;
		midi::MidiInput				mMidiIn2;
		midi::MidiInput				mMidiIn3;
		// WebSockets
		// Web socket client
		void						wsClientDisconnect();
		WebSocketClient				mClient;
		void						wsClientConnect();
		void						onWsConnect();
		void						onWsDisconnect();
		void						onWsError(std::string err);
		void						onWsInterrupt();
		void						onWsPing(std::string msg);
		void						onWsRead(std::string msg);
		bool						clientConnected;
		// Web socket  server
		WebSocketServer				mServer;
		void						serverConnect();
		void						serverDisconnect();
		double						mPingTime;
		// osc
		osc::Listener 				mOSCReceiver;
		osc::Sender					mOSCSender;
		osc::Sender					mOSCSender2;
		static const int			MAX = 16;
		int							iargs[MAX];
		float						fargs[MAX];
		string						sargs[MAX];
		// ableton liveOSC
		string						tracks[MAX];

		// remoteImGui
		bool						remoteClientActive;
		int							Frame;
		int 						FrameReceived;
		int 						PrevPacketSize;
		bool 						IsKeyFrame;
		bool 						ForceKeyFrame;
		std::vector<unsigned char>	Packet;
		std::vector<unsigned char>	PrevPacket;
		RemoteInput					Input;
		void						Write(unsigned char c);
		void						Write(unsigned int i);
		void						Write(const void *data, int size);
		void						WriteDiff(const void *data, int size);

		void						PreparePacket(unsigned char data_type, unsigned int data_size);
		void						PreparePacketTexFont(const void *data, unsigned int w, unsigned int h);
		int							mouseX;
		int							mouseY;

		// packet types
		enum { TEX_FONT = 255, FRAME_KEY = 254, FRAME_DIFF = 253 };

	};
}
