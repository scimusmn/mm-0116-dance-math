#pragma once

#include "ofMain.h"
#include "Jukebox.h"
#include "DMLayout.h"

using namespace ofxSimpleLayout;

const int VID_SIZE_BIG_W = 1920;
const int VID_SIZE_BIG_H = 1080;
const int VID_SIZE_SMALL_W = 640;
const int VID_SIZE_SMALL_H = 360;

const int SCREENSAVER_TIMEOUT = 3600;// 3600 frames, 60 secs

const string LANGUAGE_SPANISH = "_es";
const string LANGUAGE_ENGLISH = "_en";

enum AppState{
    STATE_NORMAL,
    STATE_PRE_RECORD_NORM,
    STATE_RECORD_NORM,
    STATE_PRE_RECORD_HALF,
    STATE_RECORD_HALF,
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
            string slowVid;
            string normVid;
            ofVideoPlayer slowVidPlayer;
            ofVideoPlayer normVidPlayer;
            void saveData(bool halfSpeed, string vid);
            void updateVids();
            void drawVids(bool combine);
            void restartVids();
            void clear();
        };
        Session session;
        void initRecording();
        void resetTimeTracking();
    
        //Music
        Jukebox jukebox;
    
        int timeStarted;
        int timeElapsed;
    
        //UI
        AppState appState;
        DMLayout layout;
        ofVideoPlayer guideVidPlayer;
        void startOver();
        void startRecordSequence();
        void resetInactivity();
        void toggleLanguage();
        string currentLanguage;
        int preRecordDuration;
        int recordDuration;
        int inactivityCount;

        //Util
        void clearFiles();
		
};
