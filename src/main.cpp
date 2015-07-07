#include "ofMain.h"
#include "ofApp.h"
#include "ofxWatchdog.h"
#include "ofxVideoRecorder.h"

//========================================================================
int main( ){
    
    // ofxWatchdog::watch(msec, reboot, override, verbose)
    // If a process takes more than 7 secs, ofxWatchdog will restart app
    ofxWatchdog::watch(7000, true, true, true);
    
	ofSetupOpenGL(1920,1080,OF_FULLSCREEN);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
