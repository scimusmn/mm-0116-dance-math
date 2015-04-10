#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "Jukebox.h"
#include "DMLayout.h"

#define DEBUG_HELPERS		// uncomment to draw debug helpers

using namespace ofxSimpleLayout;

enum AppState{
    STATE_NORMAL,
    STATE_COUNTDOWN,
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
    
        //Cameras
        ofVideoGrabber camRaw;
        ofVideoGrabber camTracker;
        int camRawDeviceID = 0;
        int camTrackerDeviceID = 2;
        ofPtr<ofQTKitGrabber> vidRecorder;
        void videoSaved(ofVideoSavedEventArgs& e);

        //Tracking
        class TrackPoint : public ofVec2f {
        public:
            TrackPoint(float x, float y, int time, bool beat);
            int time;
            bool beat;
        };
    
        class Session {
        public:
            vector<TrackPoint> slowPts;
            vector<TrackPoint> normPts;
            vector<TrackPoint> fastPts;
            string slowVid;
            string normVid;
            string fastVid;
            void saveData(float speed, vector<TrackPoint> pts, string vid);
            int getColor(float speed);
            void clear();
        };
    
        Session session;
        ofxCv::ContourFinder contourFinder;
        vector<TrackPoint> drawPts;
        void resetTracking();
        void resetTracking(bool);
        void drawTrackedLine(vector<TrackPoint> pts, int color, bool useTime);
        float camRatio;
        ofVec2f camOffset;
        ofPolyline drawLine;
    
        //Music
        Jukebox jukebox;
    
        float currentSpeed;
        int timeStarted;
        int timeElapsed;
        int prevBeatTime;
        bool isNewBeat;
    
        //UI
        AppState appState;
        ofVideoPlayer vidPlayback;
        void drawTracking();
        void startDanceCountdown();
        DMLayout layout;
    
        //Util
        void clearFiles();
    
        //Temp/Debug
        int bpmRadius;
        int prevT;
		
};
