/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int _BootMode)
: BootMode(BOOTMODE(_BootMode))
, Osc("127.0.0.1", 12348, 12347)
, MocapPos_x(0)
, MocapPos_y(0)
, MocapPos_z(0)
, DispTextureType(TEXTURE_WOOD)
{
}

/******************************
******************************/
ofApp::~ofApp()
{
}

/******************************
******************************/
void ofApp::exit()
{
	printf("> Exit\n");
}


//--------------------------------------------------------------
void ofApp::setup(){
	/********************
	********************/
	ofSetWindowTitle("Wall:Perlin");
	ofSetVerticalSync(true);
	ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.
	ofSetWindowShape(WIDTH, HEIGHT);
	ofSetEscapeQuitsApp(false);
	
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	// ofEnableSmoothing();
	
	ofSetCircleResolution(50);
	
	/********************
	********************/
	setup_gui();
	
	/********************
	********************/
	shader[TEXTURE_WOOD].load( "Wood.vert", "Wood.frag" );
	SyphonTexture[TEXTURE_WOOD].setName("Screen Output Wood");
	
	shader[TEXTURE_MARBLE].load( "Marble.vert", "Marble.frag" );
	SyphonTexture[TEXTURE_MARBLE].setName("Screen Output Marble");
	
	shader[TEXTURE_RANDOMLINE].load( "RandomLine.vert", "RandomLine.frag" );
	SyphonTexture[TEXTURE_RANDOMLINE].setName("Screen Output RandomLine");
	
	for(int i = 0; i < NUM_TEXTURE_TYPES; i++){
		fbo[i].allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	}
}

/******************************
******************************/
void ofApp::setup_gui()
{
	/********************
	********************/
	gui.setup();
	{
		ofColor initColor = ofColor(0, 0, 255, 255);
		ofColor minColor = ofColor(0, 0, 0, 0);
		ofColor maxColor = ofColor(255, 255, 255, 255);
		
		gui.add(guiColor_Wood.setup("Wood", initColor, minColor, maxColor));
		gui.add(guiColor_Marble.setup("Marble", initColor, minColor, maxColor));
		gui.add(guiColor_RandomLine.setup("RandomLine", initColor, minColor, maxColor));
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	/********************
	********************/
	bool b_OscReceived = false;
	
	/********************
	********************/
	while(Osc.OscReceive.hasWaitingMessages()){
		ofxOscMessage m_receive;
		Osc.OscReceive.getNextMessage(&m_receive);
		
		if(m_receive.getAddress() == "/HumanPose"){
			MocapPos_x = m_receive.getArgAsFloat(0);
			MocapPos_y = m_receive.getArgAsFloat(1);
			MocapPos_z = m_receive.getArgAsFloat(2);
			
			b_OscReceived = true;
		}
	}
	
	/********************
	********************/
	ofxOscMessage m;
	m.setAddress("/HumanPose");
	m.addFloatArg(MocapPos_x);
	m.addFloatArg(MocapPos_y);
	m.addFloatArg(MocapPos_z);
	
	Osc.OscSend.sendMessage(m);
	
	/********************
	********************/
	if(b_OscReceived)	map_mocapPos_to_MousePos();
}

/******************************
description
	MocapPos_xをmappingし直す.
	
Warning
	OSC message を受け取っていないのに、本関数を呼ぶと、
	mappingが繰り返され、誤った値にズレこんでいく -> プルプルする.
	ので注意.
******************************/
void ofApp::map_mocapPos_to_MousePos()
{
	const float Range_x = 1.5;
	const float Range_z = 1.5;
	
	MocapPos_x = ofMap(MocapPos_x, -Range_x, Range_x, 0, ofGetWidth());
	MocapPos_z = ofMap(MocapPos_z, -Range_z, Range_z, 0, ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::draw(){
	/********************
	********************/
	// Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// ofClear(0, 0, 0, 0);
	
	/********************
	********************/
	ofDisableAlphaBlending();
	for(int i = 0; i < NUM_TEXTURE_TYPES; i++){
		fbo[i].begin();
		shader[i].begin();
		
			/* */
			ofClear(0, 0, 0, 0);
			
			/* */
			shader[i].setUniform2f( "u_resolution", ofGetWidth(), ofGetHeight() );
			
			if(BootMode == BOOTMODE_MOUSE)	shader[i].setUniform2f( "u_mouse", mouseX, mouseY );
			else							shader[i].setUniform2f( "u_mouse", MocapPos_x, MocapPos_z );
			
			shader[i].setUniform1f( "u_time", ofGetElapsedTimef() );
			
			ofColor BaseColor;
			switch(i){
				case TEXTURE_WOOD:
					BaseColor = guiColor_Wood;
					break;
					
				case TEXTURE_MARBLE:
					BaseColor = guiColor_Marble;
					break;
					
				case TEXTURE_RANDOMLINE:
					BaseColor = guiColor_RandomLine;
					break;
					
				default:
					BaseColor = guiColor_Wood;
					break;
			}
			shader[i].setUniform4f( "BaseColor",  float(BaseColor.r)/255, float(BaseColor.g)/255, float(BaseColor.b)/255, float(BaseColor.a)/255);
			
			/* */
			ofSetColor( 255, 255, 255 );
			ofFill();
			ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
		
		shader[i].end();
		fbo[i].end();
	}

	/********************
	********************/
	fbo[DispTextureType].draw(0, 0, ofGetWidth(), ofGetHeight());
	
	/********************
	Syphon Stuff
	********************/
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	for(int i = 0; i < NUM_TEXTURE_TYPES; i++){
		SyphonTexture[i].publishTexture(&fbo[i].getTexture());
	}
	
	
	/********************
	********************/
	/*
	ofSetColor(255, 0, 0, 255);
	
	char buf[BUF_SIZE];
	sprintf(buf, "%7.4f", video.getPosition());
	ofDrawBitmapString(buf, 100, 50);
	
	if(BootMode == BOOTMODE_PLAY){
		float MousePos = ofMap(mouseX, 0, ofGetWidth(), 0, 1);
		sprintf(buf, "%7.4f", MousePos);
		ofDrawBitmapString(buf, 100, 70);
	}
	*/
	
	/********************
	********************/
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if(key - '0' < NUM_TEXTURE_TYPES){
				DispTextureType = TEXTURE_TYPE(key - '0');
				printf("DispTexture = %d\n", DispTextureType);
			}
			break;
			
		case ' ':
			ofSaveScreen("image.png");
			printf("image saved\n");
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
