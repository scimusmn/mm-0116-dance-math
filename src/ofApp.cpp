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
    layout.setView(DMLayout::VIEW_PICK_DANCE);
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
            ofLogNotice("Countdown beat:" + ofToString(countdown));
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
    ofTranslate(1280, 0, 0);
    ofScale(-1, 1); //**--> Drawing is MIRRORED
    
    if (appState == STATE_PLAYBACK){
        
        session.drawVids();

    } else {
        camRaw.draw(0,0,1280,960);
    }
    
    #ifdef DEBUG_HELPERS
        ofPushMatrix();
        ofTranslate(0, 720);
        ofSetColor(255,100,0);
        float timeSinceBeat = ofGetElapsedTimeMillis() - prevBeatTime;
        ofCircle(750, 250, 50 - (timeSinceBeat * 0.025));
        ofDrawBitmapString(ofToString(ofGetFrameRate())+"fps", 20, 30);
        ofPopMatrix();
    #endif
    
    ofPopMatrix(); //**--> Drawing is UN-MIRRORED
    
    //Draw layout
    ofSetColor(255,255,255);
    layout.draw();
    
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
    
    ofLogNotice("btnmsg: "+btn);
    
    if (btn.substr(0,11) == "chose_dance") {
        string dance = btn.substr(12);
        ofLogNotice("Dance chosen: "+dance);
        layout.setView(DMLayout::VIEW_CHOOSE_MUSIC);
    }
    
    if (btn.substr(0,13) == "preview_music") {
        string music = btn.substr(14);
        ofLogNotice("Preview music: "+music);
        jukebox.switchSong(music);
        jukebox.play();
    }
    
    if (btn.substr(0,11) == "chose_music") {
        string music = btn.substr(12);
        ofLogNotice("Music chosen: "+music);
        
        jukebox.switchSong(music);
        
        //Start initial dance
        currentSpeed = 1;
        startDanceCountdown();
    }
    
    if (btn.substr(0,11) == "start_dance") {
        
        string speed = btn.substr(12);
        currentSpeed = ofToFloat(speed);
        ofLogNotice("Speed chosen: "+speed);
        
        startDanceCountdown();
        
    }
    
    if (btn == "start_over") {
        
        //clear all tracking
        resetBeatTracking();
        appState = STATE_NORMAL;
        
        layout.setView(DMLayout::VIEW_PICK_DANCE);
        vidPlayback.setLoopState(OF_LOOP_NORMAL);
        
        //delete all temp files
        clearFiles();
        session.clear();
        
        //reset base time
        ofResetElapsedTimeCounter();
        
    }
    
    if (btn == "toggle_overlay"){
        showOverlay = !showOverlay;
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
    dir.listDir("temp");
    vector< ofFile > files = dir.getFiles();
    for (int i = 0; i<files.size(); i++) {
        files[i].remove();
    }

}

//--------------------------------------------------------------
void ofApp::Session::saveData(float speed, string vid){
    if (speed == 0.5) {
        slowVid = vid;
        slowPlayer.close();
        slowPlayer.loadMovie(slowVid);
        slowPlayer.play();
        slowPlayer.setSpeed(2);
        slowPlayer.setLoopState(OF_LOOP_NONE);
    } else if (speed == 1) {
        normVid = vid;
        normPlayer.close();
        normPlayer.loadMovie(normVid);
        normPlayer.play();
        normPlayer.setSpeed(1);
        normPlayer.setLoopState(OF_LOOP_NONE);
    } else if (speed == 2) {
        fastVid = vid;
        fastPlayer.close();
        fastPlayer.loadMovie(fastVid);
        fastPlayer.play();
        fastPlayer.setSpeed(0.5);
        fastPlayer.setLoopState(OF_LOOP_NONE);
    } else {
        ofLogError("Session") << "Can not save session. Unrecognized speed: " << speed;
    }
}

//--------------------------------------------------------------
void ofApp::Session::updateVids(){
    
    if(!normVid.empty()) {
        normPlayer.update();
    }
    
    if(!slowVid.empty()) {
        slowPlayer.update();
    }
    
    if(!fastVid.empty()) {
        fastPlayer.update();
    }
    
}

//--------------------------------------------------------------
void ofApp::Session::drawVids(){
    
    if(!normVid.empty()) {
        ofSetColor(255,255,255,255);
        normPlayer.draw(0,0,1280,960);
    }
    
//    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    if(!slowVid.empty()) {
        ofSetColor(255,200,200,85);
        slowPlayer.draw(0,0,1280,960);
    }
    
    if(!fastVid.empty()) {
        ofSetColor(200,200,255,85);
        fastPlayer.draw(0,0,1280,960);
    }
    
//    ofDisableBlendMode();
    ofSetColor(255,255,255);
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
    slowPlayer.close();
    normPlayer.close();
    fastPlayer.close();
}

