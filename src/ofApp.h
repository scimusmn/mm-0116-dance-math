#pragma once

#include "ofMain.h"
#include "Jukebox.h"
#include "DMLayout.h"

//#define DEBUG_HELPERS		// uncomment to draw debug helpers

using namespace ofxSimpleLayout;

const int VID_SIZE_BIG_W = 1280;
const int VID_SIZE_BIG_H = 720;
const int VID_SIZE_SMALL_W = 640;
const int VID_SIZE_SMALL_H = 360;

const int SCREENSAVER_TIMEOUT = 3600;// 3600 frames, 60 secs

enum AppState{
    STATE_NORMAL,
    STATE_PRE_COUNTDOWN,
    STATE_COUNTDOWN,
    STATE_RECORDING,
    STATE_PLAYBACK,
    STATE_SCREENSAVER
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
            string title;
            string slowVid;
            string normVid;
            string fastVid;
            ofVideoPlayer slowVidPlayer;
            ofVideoPlayer normVidPlayer;
            ofVideoPlayer fastVidPlayer;
            void saveData(float speed, string vid);
            void updateVids();
            void drawRecordedVids(bool combine);
            void restartVids();
            void drawProgress(int startX, int endX, int y, float prog, int color, float barHeight);
            void drawProgress(int startX, int endX, int y, float prog, int color);
            int getColor(float speed);
            void clear();
        };
        Session session;
        void initRecording();
        void resetBeatTracking();
    
        //Music
        Jukebox jukebox;
        float currentSpeed;
        int timeStarted;
        int timeElapsed;
        float recordProgress;
        int prevBeatTime;
        bool isNewBeat;
    
        //UI
        AppState appState;
        int countdown;
        int preCountdownDuration;
        ofVideoPlayer vidPlayback;
        int inactivityCount;
    
        void startDanceCountdown();
        void startOver();
        DMLayout layout;
        ofSoundPlayer greatJobSnd;
    
        //Util
        void clearFiles();
    
        //Temp/Debug
        bool showOverlay;
        int bpmRadius;
        int prevT;
		
};
