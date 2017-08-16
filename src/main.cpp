#include "ofMain.h"
#include "ofApp.h"
//#include "ofxWatchDog.h"
//========================================================================
int main( ){
	ofSetupOpenGL(500,768,OF_WINDOW);			// <-------- setup the GL context
    //ofxWatchdog::watch(2000, false, true, true);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
