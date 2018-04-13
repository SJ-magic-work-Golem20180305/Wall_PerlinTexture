/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxGui.h"

#include "sjCommon.h"
#include "sj_OSC.h"

/************************************************************
************************************************************/

class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	enum BOOTMODE{
		BOOTMODE_MOUSE,
		BOOTMODE_MOCAP,
	};
	enum{
		WIDTH = 1280,
		HEIGHT = 720,
	};
	enum TEXTURE_TYPE{
		TEXTURE_WOOD,
		TEXTURE_MARBLE,
		TEXTURE_RANDOMLINE,
		TEXTURE_RANDOMLINE_FINE,
		
		NUM_TEXTURE_TYPES,
	};
	
	/****************************************
	****************************************/
	BOOTMODE BootMode;
	
	ofxSyphonServer SyphonTexture[NUM_TEXTURE_TYPES];
	ofFbo fbo[NUM_TEXTURE_TYPES];
	ofShader shader[NUM_TEXTURE_TYPES];
	
	OSC_TARGET Osc;
	float MocapPos_x;
	float MocapPos_y;
	float MocapPos_z;
	
	TEXTURE_TYPE DispTextureType;
	
	/********************
	********************/
	ofxPanel gui;
	ofxColorSlider guiColor_Wood;
	ofxColorSlider guiColor_Marble;
	ofxColorSlider guiColor_RandomLine;
	ofxColorSlider guiColor_RandomLine_Fine;
	


public:
	/****************************************
	****************************************/
	ofApp(int _BootMode);
	~ofApp();
	void exit();
	
	void setup();
	void update();
	void draw();

	void map_mocapPos_to_MousePos();
	void setup_gui();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
