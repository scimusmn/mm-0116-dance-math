#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofLogToFile("log.txt", true);
    ofSetLogLevel(OF_LOG_WARNING);
    ofLogWarning("Setup", ofGetTimestampString("%w, %h:%M%a"));
    
    //Hide cursor (comment out if not on touch screen)
    ofHideCursor();

    //Set up graphics
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofBackground(30,30,30);

    //Setup cameras & video recorder
    initCamera();
    
    //Load Sounds
    jukebox.autoAddTrack("circle", 9911, 18386, 28096, 45046); // CIRCLE
    jukebox.autoAddTrack("triangle", 9911, 18386, 28096, 45046); // TRIANGLE
    jukebox.autoAddTrack("square", 9951, 18286, 28063, 44812); // SQUARE
    jukebox.autoAddTrack("freestyle1", 5507, 11612, 21790, 34002); // HIP and HOPPY
    jukebox.autoAddTrack("freestyle2", 5340, 11629, 21690, 34269); // WALKIN THE KEYS
    jukebox.autoAddTrack("freestyle3", 5107, 10962, 20722, 32433); // STOMP AND SHUFFLE
    jukebox.autoAddTrack("freestyle4", 4639,  9728, 18753, 28930); // ROCK & ROLL
    jukebox.loadSound("btnPress");
    jukebox.loadSound("select_en");
    jukebox.loadSound("selectTrack_en");
    jukebox.loadSound("playback1_en");
    jukebox.loadSound("playback2_en");
    jukebox.loadSound("playback3_en");
    
    //Load/Setup UI
    layout.setupViews();
    currentLanguage = LANGUAGE_SPANISH;
    toggleLanguage();
    layout.setView(DMLayout::VIEW_SELECT);
    appState = STATE_NORMAL;
    
    startOver();

}

//--------------------------------------------------------------
void ofApp::initRecording(){
    
    //Start recording new video file in /temp folder (video only)
    currentVidPath = "temp/video_"+ofGetTimestampString()+".mov";
    
    //Setup to record 24fps, with no sound, and use the system clock to sync videos
    if (appState == STATE_PRE_RECORD_NORM || appState == STATE_RECORD_NORM){
        vidRecorder.setup(currentVidPath, vidGrabber.getWidth(), vidGrabber.getHeight(), 24, 0, 0, true, true);
    } else {
        vidRecorder.setup(currentVidPath, vidGrabber.getWidth(), vidGrabber.getHeight(), 12, 0, 0, true, true);
    }
    
    vidRecorder.start();
    
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
    
    ofLogWarning("startRecordSequence()", ofToString(jukebox.current.id) + ", " + ofToString(ofGetTimestampString()) );

    //Show live camera feed and overlay video guide
    layout.setView(DMLayout::VIEW_RECORD);
    appState = STATE_PRE_RECORD_NORM;
    
    //Start guide overlay video
    jukebox.playFromStart();
    resetTimeTracking();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //Update all layout elements
    layout.update();
    
    //Update time elapsed since last reset.
    timeElapsed = ofGetElapsedTimeMillis() - timeStarted;
    
    //Update videos or camera-feeds
    if (appState == STATE_PLAYBACK) {
        if (layout.currentViewId == DMLayout::VIEW_PLAYBACK_1) {
            session.updateVids(false);
        } else {
            session.updateVids(true);
        }
    }
    
    //Draw camera feed unless on last playback screen or screensaver
    if (layout.baseViewId != DMLayout::VIEW_PLAYBACK_3 && appState != STATE_SCREENSAVER) {
        
        //Update during recording
        vidGrabber.update();
        
        //Add frame if we are currently recording.
        if(vidGrabber.isFrameNew() && vidRecorder.isRecording()){
            
            vidRecorder.addFrame(vidGrabber.getPixelsRef());
            
        }
        
    }
    
    //Update guide video during recording
    if (jukebox.trackLoaded == true){
        jukebox.current.player.update();
    }
    
    //Start normal-speed recording
    if (appState == STATE_PRE_RECORD_NORM && timeElapsed >= jukebox.current.normPreRecordDuration){
        
        ofLogNotice("Start normal-speed recording", ofToString(timeElapsed));
        initRecording();
        appState = STATE_RECORD_NORM;

    }
    //End normal-speed recording
    else if (appState == STATE_RECORD_NORM && timeElapsed >= jukebox.current.normRecordDuration){
        
        ofLogNotice("End normal-speed recording", ofToString(timeElapsed));
        
        if(vidRecorder.isRecording()){
            ofLogWarning("NORM RECORDED frames", ofToString(vidRecorder.getNumVideoFramesRecorded()));
            vidRecorder.close();
            appState = STATE_PRE_RECORD_HALF;
            this->videoSaved();
        } else {
            ofLogError("Cannot save video") << "video was still recording: " << vidRecorder.isRecording();
        }
    
        appState = STATE_PRE_RECORD_HALF;

    }
    //Start half-speed recording
    else if (appState == STATE_PRE_RECORD_HALF && timeElapsed >= jukebox.current.halfPreRecordDuration){
        
        ofLogNotice("Start half-speed recording", ofToString(timeElapsed));
        initRecording();
        appState = STATE_RECORD_HALF;
        
    }
    //End half-speed recording
    else if (appState == STATE_RECORD_HALF && timeElapsed >= jukebox.current.halfRecordDuration){
        
        ofLogNotice("End half-speed recording", ofToString(timeElapsed));
        if(vidRecorder.isRecording()){
            ofLogWarning("HALF RECORDED frames", ofToString(vidRecorder.getNumVideoFramesRecorded()));
            vidRecorder.close();
            appState = STATE_PRE_PLAYBACK;
            this->videoSaved();
        } else {
            ofLogError("Cannot save video") << "video was still recording: " << vidRecorder.isRecording();
        }
        
        appState = STATE_PRE_PLAYBACK;
        
    }
    //Half-speed recording has finished. Wait for guide video to end
    else if (appState == STATE_PRE_PLAYBACK) {
        
        if (vidRecorder.isRecording() == false){
            
            if (jukebox.current.player.getIsMovieDone() == true) {
                
                //Clear jukebox
                jukebox.clearTrack();
                
                //Show playback screen
                //(recorded vids may or may not already be loaded)
                appState = STATE_PLAYBACK;
                layout.setView(DMLayout::VIEW_PLAYBACK_1);
                jukebox.playSound("playback1_en");
                
            }
            
        }
        
    }
    

    //Loop playback
    if (appState == STATE_PLAYBACK && session.normVidPlayer.getIsMovieDone() == true){
        session.restartVids();
    }
    //Auto-advance playback screens
    if (appState == STATE_PLAYBACK && inactivityCount > ceil(SCREENSAVER_TIMEOUT * 0.5)) {
        
        if (layout.currentViewId == DMLayout::VIEW_PLAYBACK_1) {
            ofLogWarning("Auto-advance from", ofToString(layout.currentViewId) );
            mousePressed(975, 916, 1);
            resetInactivity();
        }
        else if (layout.currentViewId == DMLayout::VIEW_PLAYBACK_2) {
            ofLogWarning("Auto-advance from", ofToString(layout.currentViewId) );
            mousePressed(490, 917, 1);
            resetInactivity();
        }
        
    }

    //Screensaver
    if (appState == STATE_RECORD_NORM || appState == STATE_RECORD_HALF || appState == STATE_SCREENSAVER) {
        resetInactivity();
    }
    if (inactivityCount > SCREENSAVER_TIMEOUT){

        ofLogWarning("Display Screensaver", ofToString(ofGetTimestampString()) );
        appState = STATE_SCREENSAVER;
        layout.setView(DMLayout::VIEW_SCREENSAVER);
        resetInactivity();
        clearData();
        
    } else {
        inactivityCount++;
        
        //Uncomment to simulate user input for debugging.
        /*
        if (inactivityCount > 15){
            //Simulate mouse press at random point on screen
            mousePressed(ofRandomWidth(), ofRandomHeight(), 1);
            inactivityCount = 0;
        }
        */
        
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    //Draw cams/Vids except on last playback screen && screensaver
    if (layout.baseViewId != DMLayout::VIEW_PLAYBACK_3 && appState != STATE_SCREENSAVER) {
        vidGrabber.draw(0, 0, VID_SIZE_BIG_W, VID_SIZE_BIG_H);
    }
    
    //Draw combined videos on final playback screen
    if (appState == STATE_PLAYBACK && layout.baseViewId == DMLayout::VIEW_PLAYBACK_3){
        session.drawVids(true);
    }
    
    //Draw layout
    ofSetColor(255,255,255);
    layout.draw();
    
    //Update guide video during recording
    if (jukebox.trackLoaded == true){
        jukebox.current.player.draw(0, 0, VID_SIZE_BIG_W, VID_SIZE_BIG_H);
    }
    
    //Draw seperated videos on first two playback screens
    if (appState == STATE_PLAYBACK && layout.baseViewId != DMLayout::VIEW_PLAYBACK_3){
        session.drawVids(false);
    }

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    string btn = layout.getSelected(x, y);
    
    //trigger generic button sound
    if (btn != ""){
        ofSoundStopAll();
        jukebox.playSound("btnPress");        
    }

    if (btn.substr(0,13) == "chose_pattern") {
        
        string pattern = btn.substr(14);
        if (pattern != "freestyle") {
            jukebox.switchTrack(pattern);
            startRecordSequence();
        } else {
            //Show freestyle music selection screen
            layout.setView(DMLayout::VIEW_SELECT_TRACK);
            jukebox.playSound("selectTrack_en");
            
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
        jukebox.playSound("playback2_en");
    } else if (btn == "combine") {
        layout.setView(DMLayout::VIEW_PLAYBACK_3);
        jukebox.playSound("playback3_en");
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
void ofApp::exit() {
    vidRecorder.close();
}

//--------------------------------------------------------------
void ofApp::startOver(){
    
    //clear all tracking
    resetTimeTracking();
    appState = STATE_NORMAL;
    
    layout.setView(DMLayout::VIEW_SELECT);
    jukebox.playSound("select_en");
    
    clearData();
    
}

//--------------------------------------------------------------
void ofApp::clearData(){
    
    //delete all temp files
    clearFiles();
    
    //clear session data
    session.clear();
    
    //Reset jukebox track
    jukebox.clearTrack();
    
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
void ofApp::videoSaved(){
    
    ofLogNotice("Success: videoSaved()", ofToString(currentVidPath));
    
    if (appState == STATE_PRE_RECORD_HALF) {
        
        session.saveData(false, currentVidPath);
        
        //Second recording is underway...
        
    } else if (appState == STATE_PRE_PLAYBACK || appState == STATE_PLAYBACK) {
        
        session.saveData(true, currentVidPath);
        
        //Default to original speeds
        session.slowVidPlayer.setSpeed(1);
        session.normVidPlayer.setSpeed(1);
        
        session.restartVids();
        
    } else {
        
        ofLogError("videoSaved") << "Appstate unexpected: " << appState;
        
    }
    
}

//--------------------------------------------------------------
bool ofApp::listCamDevices() {
    
    //we can now get back a list of devices.
    vector<ofVideoDevice> devices = vidGrabber.listDevices();
    
    ofLogWarning("List Camera Devices");
    
    for(int i = 0; i < devices.size(); i++){
        if( devices[i].bAvailable ){
            ofLogWarning(ofToString(devices[i].id), ofToString(devices[i].deviceName + " - available "));
        }else{
            ofLogWarning(ofToString(devices[i].id), ofToString(devices[i].deviceName + " - unavailable "));
        }
    }
    
    if (devices.size() > 0){
        return true;
    } else {
        return false;
    }
    
}

void ofApp::initCamera() {
    
    ofLogWarning("initCamera(). isInitialized", ofToString(vidGrabber.isInitialized()));
    
    if (vidGrabber.isInitialized() == true) {
        ofLogWarning("Attempting to reconnect camera feed.", ofToString(ofGetTimestampString()));
        vidGrabber.close();
        vidRecorder.close();
        ofSleepMillis(150);
    }
    
    vidGrabber.setDesiredFrameRate(24);
    vidGrabber.setDeviceID(0);//Assumes there is only one camera connected.
    vidGrabber.initGrabber(VID_SIZE_BIG_W, VID_SIZE_BIG_H);
    vidRecorder.setVideoCodec("mpeg4");//default is "mpeg4"
    vidRecorder.setVideoBitrate("4400k");//default is "2000k"
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

    if (dir.doesDirectoryExist("temp") == false) {
        dir.createDirectory("temp");
    }

}

//--------------------------------------------------------------
void ofApp::Session::saveData(bool halfSpeed, string vid){
    
    if (halfSpeed == true) {
        slowVid = vid;
        
        slowVidPlayer.stop();
        slowVidPlayer.update(); // This update throws error, but do not remove
        ofSleepMillis(50);
        slowVidPlayer.close();
        slowVidPlayer.setSynchronousSeeking(false);
        slowVidPlayer.loadMovie(slowVid, OF_QTKIT_DECODE_TEXTURE_ONLY);
        slowVidPlayer.setLoopState(OF_LOOP_NONE);
        slowVidPlayer.play();
        
        
    } else if (halfSpeed == false) {
        normVid = vid;
        
        normVidPlayer.stop();
        normVidPlayer.update(); // This update throws error, but do not remove
        ofSleepMillis(50);
        normVidPlayer.close();
        normVidPlayer.setSynchronousSeeking(false);
        normVidPlayer.loadMovie(normVid, OF_QTKIT_DECODE_TEXTURE_ONLY);
        normVidPlayer.setLoopState(OF_LOOP_NONE);
        normVidPlayer.play();
        
    } else {
        ofLogError("Session") << "Can not save session. Unrecognized state: " << halfSpeed;
    }
    
}

//--------------------------------------------------------------
void ofApp::Session::updateVids(bool syncVids){
    
    if(!normVid.empty()) {
        normVidPlayer.update();
    }
    
    if(!slowVid.empty()) {
        
//        if (syncVids == false) {
            slowVidPlayer.update();
//        } else {
//            int syncFrame = normVidPlayer.getCurrentFrame();
//            if (syncFrame > slowVidPlayer.getTotalNumFrames()) syncFrame = slowVidPlayer.getTotalNumFrames();
//            slowVidPlayer.setFrame(syncFrame);
//        }

    }
    
}

//--------------------------------------------------------------
void ofApp::Session::drawVids(bool combine){
    
    ofSetColor(255,255,255,255);
    
    if (combine == true){
        
        //Combined (mirrored)
        ofSetColor(255,255,255,255);
        normVidPlayer.draw(0, 0, VID_SIZE_BIG_W, VID_SIZE_BIG_H);
        
        ofSetColor(255, 255, 255, 140);
        slowVidPlayer.draw(0, 0, VID_SIZE_BIG_W, VID_SIZE_BIG_H);
        
    } else {
        
        //Separated (mirrored)
        normVidPlayer.draw(42, 381, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        slowVidPlayer.draw(1016, 381, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        
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
     * which happens rarely but consistently by chance,
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

