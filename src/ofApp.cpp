#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofEnableSmoothing();
    ofBackground(30,30,30);
    
    //Setup Cameras
    vidRecorder = ofPtr<ofQTKitGrabber>( new ofQTKitGrabber() );
    camRaw.setGrabber(vidRecorder);
    camRaw.setDeviceID(0);
    camRaw.initGrabber(1280,720);

    ofAddListener(vidRecorder->videoSavedEvent, this, &ofApp::videoSaved); // Listen for video saved events
    vidRecorder->initRecording();

    //Load Sounds
    jukebox.addSong("jazz", "sounds/jazz.wav", 10801, 3707, 466);
    jukebox.addSong("waltz", "sounds/waltz.wav", 14567, 7107, 943);
    jukebox.addSong("calliope", "sounds/calliope.wav", 11493, 5517, 766);
    jukebox.addSong("tango", "sounds/tango.wav", 10236, 3783, 466);

    //Load/Setup UI
    layout.setupViews();
    layout.setView(DMLayout::VIEW_CHOOSE_MUSIC);
    appState = STATE_NORMAL;
    
}

//--------------------------------------------------------------
void ofApp::initRecording(){
    
    //Tracking
    resetBeatTracking();
    appState = STATE_TRACKING;
    
    //Video recording
    stringstream s;
    s << "temp/xDANCE_" << ofGetUnixTime() << ".mov";
    vidRecorder->startRecording(s.str());
    
    ofLogNotice("initRecording() ") << "Start recording: " << s;
    
}

//--------------------------------------------------------------
void ofApp::resetBeatTracking(){
    timeStarted = prevBeatTime = ofGetElapsedTimeMillis();
}


//--------------------------------------------------------------
void ofApp::startDanceCountdown(){
    
    ofSoundStopAll();
    layout.setView(DMLayout::VIEW_DANCE_VIEW);
    
    countdown = 9;
    layout.setState("countdown", ofToString(countdown));
    
    //Wait 5 seconds then count down.
    resetBeatTracking();
    appState = STATE_PRE_COUNTDOWN;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    layout.update();
    
    if (appState == STATE_PLAYBACK) {
        session.updateVids();
    } else {
        camRaw.update();
    }

    //Update beat timer
    if ((ofGetElapsedTimeMillis() - prevBeatTime) >= (jukebox.tempo / jukebox.rate)){
        prevBeatTime = ofGetElapsedTimeMillis();
        isNewBeat = true;
    }
    
    timeElapsed = ofGetElapsedTimeMillis() - timeStarted;

    //Update tracking
    if (appState == STATE_TRACKING){

        isNewBeat = false;
        
        //End recording
        if (timeElapsed >= ((jukebox.duration - jukebox.intro) / jukebox.rate)) {
            
            appState = STATE_NORMAL;

            //Attempt saving RGB video, and wait for success callback
            if(vidRecorder->isRecording()){
                vidRecorder->stopRecording();
            } else {
                ofLogError("Cannot save video") << "video was still recording: " << vidRecorder->isRecording();
            }

        }

    }
    
    //Wait for countdown
    if (appState == STATE_PRE_COUNTDOWN){
        
        if (timeElapsed > 1000){
            isNewBeat = true;
            appState = STATE_COUNTDOWN;
            resetBeatTracking();
            jukebox.play(currentSpeed);
        }
        
    } else if (appState == STATE_COUNTDOWN){
        
        if (isNewBeat == true){
            
            countdown--;
            isNewBeat = false;
            layout.setState("countdown", ofToString(countdown));
            
            if (countdown <= 0) {
                initRecording();                
            }
            
        }

    }
    
    //Loop playback
    if (appState == STATE_PLAYBACK && session.normPlayer.getIsMovieDone() == true){
        
        session.restartVids();
        resetBeatTracking();
        
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    //Draw Cams/Vids
    
    ofPushMatrix();
    ofTranslate(1280, 0);
    ofScale(-1, 1); //**--> Drawing is MIRRORED
    
    if (appState == STATE_PLAYBACK){
        session.drawFeatureVids();
    } else {
        camRaw.draw(0,0,1280,720);
    }

    ofPopMatrix(); //**--> Drawing is UN-MIRRORED
    
    //Draw layout
    ofSetColor(255,255,255);
    layout.draw();
    
    if (appState == STATE_PLAYBACK){
        
        ofPushMatrix();
        ofTranslate(1810, 0);
        ofScale(-1, 1); //**--> Drawing is MIRRORED
        session.drawButtonVids();
        ofPopMatrix(); //**--> Drawing is UN-MIRRORED
        
    }
    
    #ifdef DEBUG_HELPERS
        ofPushMatrix();
        ofTranslate(0, 1000);
        ofSetColor(255,100,0);
        float timeSinceBeat = ofGetElapsedTimeMillis() - prevBeatTime;
        ofCircle(200, 0, 50 - (timeSinceBeat * 0.025));
        ofDrawBitmapString(ofToString(ofGetFrameRate())+"fps", 20, 30);
        ofPopMatrix();
        ofSetColor(255,255,255);
    #endif
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    string btn = layout.getSelected(x, y);
    
    if (btn.substr(0,11) == "chose_dance") {
        string dance = btn.substr(12);
        layout.setView(DMLayout::VIEW_CHOOSE_MUSIC);
    }
    
    if (btn.substr(0,13) == "preview_music") {
        string music = btn.substr(14);
        jukebox.switchSong(music);
        jukebox.play();
    }
    
    if (btn.substr(0,11) == "chose_music") {
        string music = btn.substr(12);
        jukebox.switchSong(music);
        
        //Start first dance
        currentSpeed = 1;
        startDanceCountdown();
    }
    
    if (btn.substr(0,11) == "start_dance") {
        
        string speed = btn.substr(12);
        currentSpeed = ofToFloat(speed);
        startDanceCountdown();
        
    }
    
    if (btn == "start_over") {
        
        //clear all tracking
        resetBeatTracking();
        appState = STATE_NORMAL;
        
        layout.setView(DMLayout::VIEW_CHOOSE_MUSIC);
        vidPlayback.setLoopState(OF_LOOP_NORMAL);
        
        //delete all temp files
        clearFiles();
        
        //clear session data
        session.clear();
        
        //reset base time
        ofResetElapsedTimeCounter();
        
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::videoSaved(ofVideoSavedEventArgs& e){
    
    // ofQTKitGrabber sends a message with the file name and any errors when the video is done recording
    if(e.error.empty()){
        
        string vidPath = e.videoPath;
        session.saveData(currentSpeed, vidPath);
        
        resetBeatTracking();
        appState = STATE_PLAYBACK;
        
        //Which playback pts to draw
        layout.setView(DMLayout::VIEW_PLAYBACK);

    } else {
        ofLogError("videoSavedEvent") << "Video save error: " << e.error;
    }
}

//----------
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
void ofApp::Session::saveData(float speed, string vid){
    if (speed == 0.5) {
        slowVid = vid;
        slowPlayer.stop();
        slowPlayer.close();
        slowPlayer.loadMovie(slowVid);
        slowPlayer.play();
        slowPlayer.setSpeed(2);
        slowPlayer.setLoopState(OF_LOOP_NONE);
        slowBtnPlayer.stop();
        slowBtnPlayer.close();
        slowBtnPlayer.loadMovie(slowVid);
        slowBtnPlayer.play();
    } else if (speed == 1) {
        normVid = vid;
        normPlayer.stop();
        normPlayer.close();
        normPlayer.loadMovie(normVid);
        normPlayer.play();
        normPlayer.setSpeed(1);
        normPlayer.setLoopState(OF_LOOP_NONE);
        normBtnPlayer.stop();
        normBtnPlayer.close();
        normBtnPlayer.loadMovie(normVid);
        normBtnPlayer.play();
    } else if (speed == 2) {
        fastVid = vid;
        fastPlayer.stop();
        fastPlayer.close();
        fastPlayer.loadMovie(fastVid);
        fastPlayer.play();
        fastPlayer.setSpeed(0.5);
        fastPlayer.setLoopState(OF_LOOP_NONE);
        fastBtnPlayer.stop();
        fastBtnPlayer.close();
        fastBtnPlayer.loadMovie(fastVid);
        fastBtnPlayer.play();
    } else {
        ofLogError("Session") << "Can not save session. Unrecognized speed: " << speed;
    }
}

//--------------------------------------------------------------
void ofApp::Session::updateVids(){
    
    if(!normVid.empty()) {
        normPlayer.update();
        normBtnPlayer.update();
    }
    
    if(!slowVid.empty()) {
        slowPlayer.update();
        slowBtnPlayer.update();
    }
    
    if(!fastVid.empty()) {
        fastPlayer.update();
        fastBtnPlayer.update();
    }
    
}

//--------------------------------------------------------------
void ofApp::Session::drawFeatureVids(){
    
    if(!normVid.empty()) {
        ofSetColor(255,255,255,255);
        normPlayer.draw(0,0,1280,720);
    }
    
    if(!slowVid.empty()) {
        if(fastVid.empty()) {
            ofSetColor(255,255,255,127);
        } else {
            ofSetColor(255,255,255,85);
        }
        slowPlayer.draw(0,0,1280,720);
    }
    
    if(!fastVid.empty()) {
        if(slowVid.empty()) {
            ofSetColor(255,255,255,127);
        } else {
            ofSetColor(255,255,255,85);
        }
        fastPlayer.draw(0,0,1280,720);
    }

    ofSetColor(255,255,255);
    
}

//--------------------------------------------------------------
void ofApp::Session::drawButtonVids(){
    
    ofSetColor(255,255,255,255);
    
    if(!normVid.empty()) {
        normBtnPlayer.draw(0,180,1280/4,720/4);
    }
    
    if(!slowVid.empty()) {
        slowBtnPlayer.draw(0,421,1280/4,720/4);
    }
    
    if(!fastVid.empty()) {
        fastBtnPlayer.draw(0,662,1280/4,720/4);
    }
    
}

//--------------------------------------------------------------
void ofApp::Session::restartVids(){
    
    if(!normVid.empty()) {
        normPlayer.firstFrame();
        normPlayer.play();
    }
    
    if(!slowVid.empty()) {
        slowPlayer.firstFrame();
        slowPlayer.play();
    }
    
    if(!fastVid.empty()) {
        fastPlayer.firstFrame();
        fastPlayer.play();
    }
    
}



//--------------------------------------------------------------
int ofApp::Session::getColor(float speed){
    if (speed == 0.5) {
        return ofHexToInt("FF0000");
    } else if (speed == 1) {
        return ofHexToInt("00FF00");
    } else if (speed == 2) {
        return ofHexToInt("0000FF");
    } else {
        ofLogError("Session") << "Can not save session. Unrecognized speed: " << speed;
    }
}

//--------------------------------------------------------------
void ofApp::Session::clear(){
    
    slowVid = normVid = fastVid = "";
    
    //Note: Due to an OF bug, you cannot close an
    //ofVideoPlayer when current position is 0 so we
    //stop all players before closing.
    //see http://goo.gl/Vz8zdx
    
    slowPlayer.stop();
    normPlayer.stop();
    fastPlayer.stop();
    
    slowBtnPlayer.stop();
    normBtnPlayer.stop();
    fastBtnPlayer.stop();

    slowPlayer.close();
    normPlayer.close();
    fastPlayer.close();
    
    slowBtnPlayer.close();
    normBtnPlayer.close();
    fastBtnPlayer.close();
    
}

