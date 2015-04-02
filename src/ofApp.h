#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "DMLayout.h"

using namespace ofxSimpleLayout;

enum AppState{
    STATE_NORMAL,
    STATE_TRACKING,
    STATE_PLAYBACK
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        //Cameras
        ofVideoGrabber camRaw;
        ofVideoGrabber camTracker;
        ofVideoPlayer vidRaw;
        ofVideoPlayer vidTracker;
    
        //Tracking
        class TrackPoint : public ofVec2f {
        public:
            TrackPoint(float x, float y, int time, bool beat);
            int time;
            bool beat;
        };
        ofxCv::ContourFinder contourFinder;
        vector<TrackPoint> drawPts;
        vector<TrackPoint> playbackPts;
        void resetTracking();
        float camRatio;
        ofVec2f camOffset;
        ofPolyline drawLine;
    
        //Music
        void playMusic(string song, float rate);
        string currentMusic;
        ofSoundPlayer groove;
        ofSoundPlayer country;
        ofSoundPlayer waltz;
        int grooveTempo = 465;
        int countryTempo = 800;
        int waltzTempo = 1200;
    
        int timeStarted;
        int timeElapsed;
        int prevBeatTime;
        bool isNewBeat;
    
        //UI
        AppState appState;
        void drawTracking();
        DMLayout layout;
    
        //Temp/Debug
        int bpmRadius;
        int prevT;
		
};
