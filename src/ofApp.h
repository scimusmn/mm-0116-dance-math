#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "Jukebox.h"
#include "DMLayout.h"

#define DEBUG_HELPERS		// uncomment to draw debug helpers

using namespace ofxSimpleLayout;

enum AppState{
    STATE_NORMAL,
    STATE_PRE_COUNTDOWN,
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
        ofPtr<ofQTKitGrabber> vidRecorder;
        void videoSaved(ofVideoSavedEventArgs& e);

        //Session
        class Session {
        public:
            string slowVid;
            string normVid;
            string fastVid;
            ofVideoPlayer slowPlayer;
            ofVideoPlayer normPlayer;
            ofVideoPlayer fastPlayer;
            ofVideoPlayer slowBtnPlayer;
            ofVideoPlayer normBtnPlayer;
            ofVideoPlayer fastBtnPlayer;
            void saveData(float speed, string vid);
            void updateVids();
            void drawFeatureVids();
            void drawButtonVids();
            void restartVids();
            int getColor(float speed);
            void clear();
        };
    
        Session session;
        ofxCv::ContourFinder contourFinder;
        void initRecording();
        void resetBeatTracking();
    
        //Music
        Jukebox jukebox;
        float currentSpeed;
        int timeStarted;
        int timeElapsed;
        int prevBeatTime;
        bool isNewBeat;
    
        //UI
        AppState appState;
        int countdown;
        ofVideoPlayer vidPlayback;
        void startDanceCountdown();
        DMLayout layout;
    
        //Util
        void clearFiles();
    
        //Temp/Debug
        bool showOverlay;
        int bpmRadius;
        int prevT;
		
};
