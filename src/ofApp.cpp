#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //Set up graphics
    ofEnableSmoothing();
    ofBackground(30,30,30);
    
    //Setup cameras & video recorder
    vidRecorder = ofPtr<ofQTKitGrabber>( new ofQTKitGrabber() );
    cam.setGrabber(vidRecorder);
    cam.setDeviceID(0);//Assumes there is only one camera connected. (otherwise use different device id)
    cam.initGrabber(VID_SIZE_BIG_W, VID_SIZE_BIG_H);
    ofAddListener(vidRecorder->videoSavedEvent, this, &ofApp::videoSaved); // Listen for video saved events
    vidRecorder->initRecording();

    //Load Sounds
    jukebox.autoAddTrack("circle", 15000, 10000, 15000, 25000); // CIRCLE
    jukebox.autoAddTrack("triangle", 5000, 10000, 15000, 25000); // TRIANGLE
    jukebox.autoAddTrack("square", 5000, 10000, 15000, 25000); // SQUARE
    jukebox.autoAddTrack("freestyle1", 5000, 10000, 15000, 25000); // HIP-HOP
    jukebox.autoAddTrack("freestyle2", 5000, 10000, 15000, 25000); // DUBSTEP
    jukebox.autoAddTrack("freestyle3", 5000, 10000, 15000, 25000); // PIANO
    jukebox.autoAddTrack("freestyle4", 5000, 10000, 15000, 25000); // ROCK & ROLL
    
    jukebox.loadSound("btnPress");
    
    //Load/Setup UI
    layout.setupViews();
    currentLanguage = LANGUAGE_SPANISH;
    toggleLanguage();
    layout.setView(DMLayout::VIEW_SELECT);
    appState = STATE_NORMAL;
    
}

//--------------------------------------------------------------
void ofApp::initRecording(){
    
    //New video file in /temp folder
    stringstream s;
    s << "temp/video_" << ofGetUnixTime() << ".mov";
    vidRecorder->startRecording(s.str());
    
}

//--------------------------------------------------------------
void ofApp::resetTimeTracking(){
    
    //Remember current time to count beats after "now"
    timeStarted = ofGetElapsedTimeMillis();
    timeElapsed = 0;
    
}

//--------------------------------------------------------------
void ofApp::resetInactivity(){
    
    inactivityCount = 0;
    
}

//--------------------------------------------------------------
void ofApp::startRecordSequence(){
    
    ofLogNotice("startRecordSequence()");
    //Show live camera feed and overlay guide video
    layout.setView(DMLayout::VIEW_RECORD);
    appState = STATE_PRE_RECORD_NORM;
    resetTimeTracking();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //Update all layout elements
    layout.update();
    
    //Update videos or camera-feeds
    if (appState == STATE_PLAYBACK) {
        session.updateVids();
    } else {
        cam.update();
    }

    timeElapsed = ofGetElapsedTimeMillis() - timeStarted;
    
    
    //Start normal-speed recording
    if (appState == STATE_PRE_RECORD_NORM && timeElapsed >= jukebox.normPreRecordDuration){
        
        ofLogNotice("Start normal-speed recording", ofToString(timeElapsed));
        initRecording();
        appState = STATE_RECORD_NORM;

    }
    //End normal-speed recording
    else if (appState == STATE_RECORD_NORM && timeElapsed >= jukebox.normRecordDuration){
        
        ofLogNotice("End normal-speed recording", ofToString(timeElapsed));
        
        if(vidRecorder->isRecording()){
            vidRecorder->stopRecording();
        } else {
            ofLogError("Cannot save video") << "video was still recording: " << vidRecorder->isRecording();
        }
    
        appState = STATE_PRE_RECORD_HALF;

    }
    //Start half-speed recording
    else if (appState == STATE_PRE_RECORD_HALF && timeElapsed >= jukebox.halfPreRecordDuration){
        
        ofLogNotice("Start half-speed recording", ofToString(timeElapsed));
        initRecording();
        appState = STATE_RECORD_HALF;
        
    }
    //End half-speed recording
    else if (appState == STATE_RECORD_HALF && timeElapsed >= jukebox.halfRecordDuration){
        
        ofLogNotice("End half-speed recording", ofToString(timeElapsed));
        if(vidRecorder->isRecording()){
            vidRecorder->stopRecording();
        } else {
            ofLogError("Cannot save video") << "video was still recording: " << vidRecorder->isRecording();
        }
        
        appState = STATE_PLAYBACK;
        
    }

    //Loop playback
    if (appState == STATE_PLAYBACK && session.normVidPlayer.getIsMovieDone() == true){
        
        session.restartVids();
        
    }
    
    //Screensaver
    if (appState == STATE_RECORD_NORM || appState == STATE_RECORD_HALF) {
        resetInactivity();
    }
    if (inactivityCount > SCREENSAVER_TIMEOUT){
        appState = STATE_SCREENSAVER;
        layout.setView(DMLayout::VIEW_SCREENSAVER);
        resetInactivity();
    } else {
        inactivityCount++;
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    //Draw cams/Vids
    ofPushMatrix();
    //ofScale(-1, 1); //**--> Drawing is MIRRORED
    ofTranslate(-85, 0); //**--> Drawing is SHIFTED
    
    if (appState != STATE_PLAYBACK){
        
        cam.draw(0,0,1920,1080);
        
    }

    ofPopMatrix(); //**--> Drawing is UN-SHIFTED
    
    //Draw layout
    ofSetColor(255,255,255);
    layout.draw();
    
    if (appState == STATE_PLAYBACK){
        if (layout.baseViewId == DMLayout::VIEW_PLAYBACK_3) {
            //Combine videos
            session.drawVids(true);
        } else {
            //Separate videos
            session.drawVids(false);
        }
    }

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    string btn = layout.getSelected(x, y);

    if (btn.substr(0,13) == "chose_pattern") {
        
        string pattern = btn.substr(14);
        if (pattern != "freestyle") {
            jukebox.switchTrack(pattern);
            startRecordSequence();
        } else {
            //Show freestyle music selection screen
            layout.setView(DMLayout::VIEW_SELECT_TRACK);
        }
        
    }
    
    if (btn.substr(0,11) == "chose_music") {
        string music = btn.substr(12);
        
        jukebox.switchTrack(music);
        startRecordSequence();

    }
    
    //Playback buttons
    if (btn == "double_playback_speed"){
        //Speed up second vid
        session.slowVidPlayer.setSpeed(2);
        session.slowVidPlayer.firstFrame();
        session.normVidPlayer.firstFrame();
        layout.setView(DMLayout::VIEW_PLAYBACK_2);
    } else if (btn == "combine") {
        layout.setView(DMLayout::VIEW_PLAYBACK_3);
    }
    
    if (btn == "toggle_language") {
        
        toggleLanguage();
        
    }
    
    if (btn == "start_over") {
        
        startOver();
        
    }
    
    if( appState == STATE_SCREENSAVER ) {
        
        //Revert language
        currentLanguage = LANGUAGE_SPANISH;
        toggleLanguage();
        
        //Awake from screensaver
        startOver();
    }
    
    resetInactivity();
    
}

//--------------------------------------------------------------
void ofApp::startOver(){
    
    //clear all tracking
    resetTimeTracking();
    appState = STATE_NORMAL;
    
    layout.setView(DMLayout::VIEW_SELECT_TRACK);
    
    //delete all temp files
    clearFiles();
    
    //clear session data
    session.clear();
    
    //reset base time
    ofResetElapsedTimeCounter();
    
}

//--------------------------------------------------------------
void ofApp::toggleLanguage(){
    
    if (currentLanguage == LANGUAGE_ENGLISH) {
        currentLanguage = LANGUAGE_SPANISH;
    } else if (currentLanguage == LANGUAGE_SPANISH) {
        currentLanguage = LANGUAGE_ENGLISH;
    } else {
        currentLanguage = LANGUAGE_ENGLISH;
    }
    
    jukebox.setLanguageKey(currentLanguage);
    layout.updateLanguage(currentLanguage);
    
}

//--------------------------------------------------------------
void ofApp::videoSaved(ofVideoSavedEventArgs& e){
    
    // ofQTKitGrabber sends a message with the file name and any errors when the video is done recording
    if(e.error.empty()){
        
        string vidPath = e.videoPath;
        session.saveData(appState, vidPath);
        
        ofLogNotice("Success: videoSaved()", ofToString(vidPath));
        
        if (appState == STATE_PRE_RECORD_HALF) {
            
            //Second recording is underway...
            
        } else if (appState == STATE_PLAYBACK) {
            
            //Default to original speeds
            session.slowVidPlayer.setSpeed(1);
            session.normVidPlayer.setSpeed(1);
            
            session.restartVids();
            
        }

    } else {
        ofLogError("videoSavedEvent") << "Video save error: " << e.error;
    }
    
}

//--------------------------------------------------------------
void ofApp::clearFiles() {
    
    ofDirectory dir;
    dir.allowExt("mov");
    dir.listDir("temp");
    vector< ofFile > files = dir.getFiles();
    
    for (int i = 0; i<files.size(); i++) {
        
        if (files[i].exists()) {
            files[i].remove();
        } else{
            ofLogWarning("Trying to delete non-existant file: " + ofToString(files[i].getBaseName()));
        }
        
    }

}

//--------------------------------------------------------------
void ofApp::Session::saveData(bool halfSpeed, string vid){
    if (halfSpeed == true) {
        slowVid = vid;
        
        slowVidPlayer.stop();
        slowVidPlayer.update();
        ofSleepMillis(50);
        slowVidPlayer.close();
        slowVidPlayer.loadMovie(slowVid);
        slowVidPlayer.play();
        
    } else if (halfSpeed == false) {
        normVid = vid;
        
        normVidPlayer.stop();
        normVidPlayer.update();
        ofSleepMillis(50);
        normVidPlayer.close();
        normVidPlayer.loadMovie(normVid);
        normVidPlayer.play();
        
    } else {
        ofLogError("Session") << "Can not save session. Unrecognized state: " << halfSpeed;
    }
}

//--------------------------------------------------------------
void ofApp::Session::updateVids(){
    
    if(!normVid.empty()) {
        normVidPlayer.update();
    }
    
    if(!slowVid.empty()) {
        slowVidPlayer.update();
    }
    
}

//--------------------------------------------------------------
void ofApp::Session::drawVids(bool combine){
    
    ofSetColor(255,255,255,255);
    
    if (combine == true){
        
        //Combined
        ofSetColor(255,255,255,255);
        normVidPlayer.draw(-111, -111, VID_SIZE_BIG_W, VID_SIZE_BIG_H);
        
        ofSetColor(255, 255, 255, 140);
        slowVidPlayer.draw(-111, -111, VID_SIZE_BIG_W, VID_SIZE_BIG_H);
        
    } else {
        
        //Separated
        normVidPlayer.draw(300, 300, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        slowVidPlayer.draw(1000, 300, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        
    }
    
}

//--------------------------------------------------------------
void ofApp::Session::restartVids(){

    if(!slowVid.empty()) {
        slowVidPlayer.firstFrame();
        slowVidPlayer.play();
    }
    
    if(!normVid.empty()) {
        normVidPlayer.firstFrame();
        normVidPlayer.play();
    }

}

//--------------------------------------------------------------
void ofApp::Session::clear(){
    
    slowVid = normVid = "";

    /**
     * Due to an OF bug, you cannot close
     * ofVideoPlayer when current position == 0,
     * which happens occasionally by chance,
     * so we stop all players before closing,
     * and sleep to allow any related callbacks
     * see http://goo.gl/Vz8zdx
     * also http://goo.gl/WJRc8O
     */

    slowVidPlayer.stop();
    normVidPlayer.stop();

    slowVidPlayer.update();
    normVidPlayer.update();
    
    ofSleepMillis(50);

    slowVidPlayer.close();
    normVidPlayer.close();
    
}

