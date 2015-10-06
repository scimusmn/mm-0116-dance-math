#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //Set up file logging (for long-term installation, only log fatal errors)
    ofLogToFile("log.txt", true);
    ofSetLogLevel(OF_LOG_FATAL_ERROR);
    ofLogError("Setup", ofGetTimestampString("%w, %h:%M%a"));

    //Hide cursor and turn off simulation mode
    //See keyPressed events to toggle
    ofHideCursor();
    simulationMode = false;

    //Setup graphics
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofBackground(30,30,30);

    //Setup cameras & video recorder
    initCamera();
    
    //Setup track videos (English)
    jukebox.autoAddTrack("circle_en", 9909, 17283, 28194, 42942); // CIRCLE
    jukebox.autoAddTrack("triangle_en", 9909, 17283, 28228, 42942); // TRIANGLE
    jukebox.autoAddTrack("square_en", 9909, 17283, 28161, 42909); // SQUARE
    jukebox.autoAddTrack("freestyle1_en", 5505, 11044, 21788, 32866); // HIP and HOPPY
    jukebox.autoAddTrack("freestyle2_en", 5372, 10777, 21755, 32599); // WALKIN THE KEYS
    jukebox.autoAddTrack("freestyle3_en", 5105, 10243, 20720, 30997); // STOMP AND SHUFFLE
    jukebox.autoAddTrack("freestyle4_en", 4571,  9175, 18685, 27894); // ROCK & ROLL
    
    ///Setup track videos (Spanish)
    jukebox.autoAddTrack("circle_es", 10310, 17684, 29763, 44511); // CIRCLE
    jukebox.autoAddTrack("triangle_es", 9876, 17250, 29262, 44010); // TRIANGLE
    jukebox.autoAddTrack("square_es", 9876, 17250, 28928, 43710); // SQUARE
    jukebox.autoAddTrack("freestyle1_es", 5839, 11378, 22222, 33299); // HIP and HOPPY
    jukebox.autoAddTrack("freestyle2_es", 5405, 10777, 21554, 32265); // WALKIN THE KEYS
    jukebox.autoAddTrack("freestyle3_es", 5138, 10243, 20954, 31164); // STOMP AND SHUFFLE
    jukebox.autoAddTrack("freestyle4_es", 4604, 9209, 18385, 27560); // ROCK & ROLL

    //Load other sounds
    jukebox.loadSound("btnPress");
    jukebox.loadSound("select_en");
    jukebox.loadSound("select_es");
    jukebox.loadSound("selectTrack_en");
    jukebox.loadSound("selectTrack_es");
    jukebox.loadSound("playback1_en");
    jukebox.loadSound("playback2_en");
    jukebox.loadSound("playback3_en");
    jukebox.loadSound("playback1_es");
    jukebox.loadSound("playback2_es");
    jukebox.loadSound("playback3_es");
    
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
    
    ofLogError("startRecordSequence()", ofToString(jukebox.current.id) + ", " + ofGetTimestampString("%w, %h:%M%a") );

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
        if (layout.baseViewId == DMLayout::VIEW_PLAYBACK_1) {
            session.updateVids(false);
        } else {
            session.updateVids(true);
        }
    }
    
    //Draw camera feed unless on last two playback screens or screensaver
    if (appState != STATE_PLAYBACK && appState != STATE_SCREENSAVER) {
        
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
        
        initRecording();
        appState = STATE_RECORD_NORM;

    }
    //End normal-speed recording
    else if (appState == STATE_RECORD_NORM && timeElapsed >= jukebox.current.normRecordDuration){
        
        if(vidRecorder.isRecording()){
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
        
        initRecording();
        appState = STATE_RECORD_HALF;
        
    }
    //End half-speed recording
    else if (appState == STATE_RECORD_HALF && timeElapsed >= jukebox.current.halfRecordDuration){
        
        if(vidRecorder.isRecording()){
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
                jukebox.playSound("playback1");
                
            }
            
        }
        
    }
    

    //Loop playback
    if (appState == STATE_PLAYBACK && session.normVidPlayer.getIsMovieDone() == true){
        session.restartVids();
    }
    //Auto-advance playback screens
    if (appState == STATE_PLAYBACK && inactivityCount > ceil(SCREENSAVER_TIMEOUT * 0.5)) {
        
        if (layout.baseViewId == DMLayout::VIEW_PLAYBACK_1) {
            ofLogWarning("Auto-advance from", ofToString(layout.baseViewId) );
            mousePressed(975, 916, 1);
            resetInactivity();
        }
        else if (layout.baseViewId == DMLayout::VIEW_PLAYBACK_2) {
            ofLogWarning("Auto-advance from", ofToString(layout.baseViewId) );
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
        if (simulationMode == true && inactivityCount > 5){
            //Simulate mouse press at random point on screen
            mousePressed(ofRandomWidth(), ofRandomHeight(), 1);
            inactivityCount = 0;
        }
        
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
            jukebox.playSound("selectTrack");
            
        }

    }
    
    if (btn.substr(0,11) == "chose_music") {
        string music = btn.substr(12);
        
        jukebox.switchTrack(music);
        startRecordSequence();

    }
    
    //Playback buttons
    if (btn == "double_playback_speed"){
        //change view
        session.restartVids();
        layout.setView(DMLayout::VIEW_PLAYBACK_2);
        jukebox.playSound("playback2");
    } else if (btn == "combine") {
        layout.setView(DMLayout::VIEW_PLAYBACK_3);
        jukebox.playSound("playback3");
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

void ofApp::keyPressed  (int key){
    
    if (key == 'f'){
        ofToggleFullscreen();
    }
    
    if (key == 'c'){
        ofShowCursor();
    }
    
    if (key == 's'){
        simulationMode = true;
    }

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
    jukebox.playSound("select");
    
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
    
    //delete video pipe file
    string videoPipePath = ofFilePath::getAbsolutePath("ofxvrpipe0");
    string command = "rm -f " + videoPipePath;
    system(command.c_str());
    
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
    
    if (layout.baseViewId == DMLayout::VIEW_SELECT) {
        jukebox.playSound("select");
    }
    
    ofLogNotice("Language Change", currentLanguage);
    
}

//--------------------------------------------------------------
void ofApp::videoSaved(){
    
    ofLogNotice("Success: videoSaved()", ofToString(currentVidPath));
    
    if (appState == STATE_PRE_RECORD_HALF) {
        
        session.saveData(false, currentVidPath);
        
        //Second recording is underway...
        
    } else if (appState == STATE_PRE_PLAYBACK || appState == STATE_PLAYBACK) {
        
        session.saveData(true, currentVidPath);

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
        
        if (syncVids == false) {
            slowVidPlayer.update();
        } else {
            int syncFrame = normVidPlayer.getCurrentFrame();
            if (syncFrame %3==0){
                slowVidPlayer.setFrame(syncFrame);
            }
        }
        
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

