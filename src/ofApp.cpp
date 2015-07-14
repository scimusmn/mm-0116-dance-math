#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofLogToFile("log.txt", true);
    ofSetLogLevel(OF_LOG_WARNING);
    ofLogWarning("Setup", ofGetTimestampString("%w, %h:%M%a"));
    
    //Hide cursor (comment out if not on touch screen)
    ofHideCursor();

    //Set up graphics
    ofSetFrameRate(30);
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
    
    ofLogWarning("startRecordSequence()", ofToString(jukebox.id) + ", " + ofToString(ofGetTimestampString()) );

    //Show live camera feed and overlay guide video
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
    
    //Update videos or camera-feeds
    if (appState == STATE_PLAYBACK) {
        session.updateVids();
    }
    
    //Draw camera feed unless on last playback screen or screensaver
    if (layout.baseViewId != DMLayout::VIEW_PLAYBACK_3 && appState != STATE_SCREENSAVER) {
        cam.update();
    }
    
    //Update guide video during recording
    if (!jukebox.id.empty()){
        jukebox.player.update();
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
        
        appState = STATE_PRE_PLAYBACK;
        
    }
    //Half-speed recording has finished. Wait for guide video to end
    else if (appState == STATE_PRE_PLAYBACK) {
        
        if (vidRecorder->isRecording() == false){
            
            if (jukebox.player.getIsMovieDone() == true) {
                
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
        cam.draw(0, 0, VID_SIZE_BIG_W, VID_SIZE_BIG_H);
    }
    
    //Draw combined videos on final playback screen
    if (appState == STATE_PLAYBACK && layout.baseViewId == DMLayout::VIEW_PLAYBACK_3){
        session.drawVids(true);
    }
    
    //Draw layout
    ofSetColor(255,255,255);
    layout.draw();
    
    //Update guide video during recording
    if (!jukebox.id.empty()){
        jukebox.player.draw(0, 0, VID_SIZE_BIG_W, VID_SIZE_BIG_H);
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
void ofApp::videoSaved(ofVideoSavedEventArgs& e){
    
    // ofQTKitGrabber sends a message with the file name and any errors when the video is done recording
    if(e.error.empty()){
        
        string vidPath = e.videoPath;
        
        ofLogNotice("Success: videoSaved()", ofToString(vidPath));
        
        if (appState == STATE_PRE_RECORD_HALF) {
            
            session.saveData(false, vidPath);
            
            //Second recording is underway...
            
        } else if (appState == STATE_PRE_PLAYBACK || appState == STATE_PLAYBACK) {
            
            session.saveData(true, vidPath);
           
            //Default to original speeds
            session.slowVidPlayer.setSpeed(1);
            session.normVidPlayer.setSpeed(1);
            
            session.restartVids();

        } else {
            
            ofLogError("videoSaved") << "Appstate unexpected: " << appState;
            
        }

    } else {
        
        ofLogError("videoSavedEvent") << "Video save error: " << e.error;
        
        //We may have received an error here
        //because the camera disconnected, so we
        //will attempt to reconnect if there
        //is one available.
        if (listCamDevices() == true) {
            initCamera();
        };
        
    }
    
}

//--------------------------------------------------------------
bool ofApp::listCamDevices() {
    
    //we can now get back a list of devices.
    vector<ofVideoDevice> devices = cam.listDevices();
    
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
    
    ofLogWarning("initCamera(). isInitialized", ofToString(cam.isInitialized()));
    
    if (cam.isInitialized() == true) {
        ofLogWarning("Attempting to reconnect camera feed.", ofToString(ofGetTimestampString()));
        cam.close();
        ofSleepMillis(150);
    }
    
    vidRecorder = ofPtr<ofQTKitGrabber>( new ofQTKitGrabber() );
    
    cam.setGrabber(vidRecorder);
    cam.setDeviceID(0);//Assumes there is only one camera connected. (otherwise use different device id)
    cam.initGrabber(VID_SIZE_BIG_W, VID_SIZE_BIG_H);
    
    ofAddListener(vidRecorder->videoSavedEvent, this, &ofApp::videoSaved); // Listen for video saved events
    vidRecorder->initRecording();
    
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
        slowVidPlayer.loadMovie(slowVid);
        slowVidPlayer.setLoopState(OF_LOOP_NONE);
        slowVidPlayer.play();
        
    } else if (halfSpeed == false) {
        normVid = vid;
        
        normVidPlayer.stop();
        normVidPlayer.update(); // This update throws error, but do not remove
        ofSleepMillis(50);
        normVidPlayer.close();
        normVidPlayer.loadMovie(normVid);
        normVidPlayer.setLoopState(OF_LOOP_NONE);
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

