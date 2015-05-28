#pragma once

#include "ofMain.h"
#include "Jukebox.h"
#include "DMLayout.h"

using namespace ofxSimpleLayout;

const int VID_SIZE_BIG_W = 1280;
const int VID_SIZE_BIG_H = 720;
const int VID_SIZE_SMALL_W = 640;
const int VID_SIZE_SMALL_H = 360;

const int SCREENSAVER_TIMEOUT = 3600;// 3600 frames, 60 secs

const string LANGUAGE_SPANISH = "_es";
const string LANGUAGE_ENGLISH = "_en";

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
		void mousePressed(int x, int y, int button);
    
        //Cameras
        ofVideoGrabber cam;
        ofPtr<ofQTKitGrabber> vidRecorder;
        void videoSaved(ofVideoSavedEventArgs& e);

        //Session
        class Session {
        public:
            string title;
            string slowVid;
            string normVid;
            ofVideoPlayer slowVidPlayer;
            ofVideoPlayer normVidPlayer;
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
        ofSoundPlayer greatJobSnd;
        int timeStarted;
        int timeElapsed;
        int prevBeatTime;
        float currentSpeed;
        float recordProgress;
        bool isNewBeat;
    
        //UI
        AppState appState;
        DMLayout layout;
        ofVideoPlayer vidPlayback;
        void startDanceCountdown();
        void startOver();
        void resetInactivity();
        void toggleLanguage();
        string currentLanguage;
        int countdown;
        int preCountdownDuration;
        int inactivityCount;

        //Util
        void clearFiles();
		
};
