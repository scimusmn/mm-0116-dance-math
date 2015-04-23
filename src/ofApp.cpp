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
    /*
    jukebox.addSong("jazz", "sounds/jazz.wav", 10801, 3707, 466);
    jukebox.addSong("clicks", "sounds/clicks.wav", 14567, 7107, 943);
    jukebox.addSong("waltz", "sounds/waltz.wav", 14567, 7107, 943);
    jukebox.addSong("tango", "sounds/tango.wav", 10236, 3783, 466);
     */
    jukebox.addSong("jazz", "sounds/circle.wav", 14567, 7107, 943);
    jukebox.addSong("clicks", "sounds/triangle.wav", 14567, 7107, 943);
    jukebox.addSong("waltz", "sounds/square.wav", 14567, 7107, 943);
    jukebox.addSong("tango", "sounds/clicks.wav", 14567, 7107, 943);

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
    timeElapsed = recordProgress = 0;
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
    recordProgress = timeElapsed / ((jukebox.duration - jukebox.intro) / jukebox.rate);

    //Update tracking
    if (appState == STATE_TRACKING){

        isNewBeat = false;
        
        //End recording
        if (recordProgress >= 1) {
            
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
                layout.setState("txtGetReady", "1");
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
    ofTranslate(-85, 0); //**--> Drawing is SHIFTED
    
    if (appState == STATE_PLAYBACK){
        session.drawFeatureVids();
        if(!session.slowVid.empty()) {
            session.drawProgress(0, VID_SIZE_BIG_W, VID_SIZE_BIG_H+0, session.normPlayer.getPosition(), session.getColor(0.5));
        }
        if(!session.normVid.empty()) {
            session.drawProgress(0, VID_SIZE_BIG_W, VID_SIZE_BIG_H+8, session.normPlayer.getPosition(), session.getColor(1));
        }
        if(!session.fastVid.empty()) {
            session.drawProgress(0, VID_SIZE_BIG_W, VID_SIZE_BIG_H+16, session.normPlayer.getPosition(), session.getColor(2));
        }
    } else {
        camRaw.draw(0,0,1280,720);
        if (appState == STATE_TRACKING) {
            session.drawProgress(0, 1280, 720, recordProgress, session.getColor(currentSpeed), 32);
        }
    }

    ofPopMatrix(); //**--> Drawing is UN-SHIFTED
    
    //Draw layout
    ofSetColor(255,255,255);
    layout.draw();
    
    if (appState == STATE_PLAYBACK){
        session.drawButtonVids();
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
    
    if (btn == "toggle_playback") {
        
        if (session.slowBtnPlayer.getSpeed()==2 || session.fastBtnPlayer.getSpeed()==0.5){
            //Show original speed
            session.slowBtnPlayer.setSpeed(1);
            session.normBtnPlayer.setSpeed(1);
            session.fastBtnPlayer.setSpeed(1);
        } else {
            //Show adjusted speed
            session.slowBtnPlayer.setSpeed(2);
            session.normBtnPlayer.setSpeed(1);
            session.fastBtnPlayer.setSpeed(0.5);
        }
        
        //restart videos
        session.slowPlayer.firstFrame();
        session.normPlayer.firstFrame();
        session.fastPlayer.firstFrame();
        session.slowBtnPlayer.firstFrame();
        session.normBtnPlayer.firstFrame();
        session.fastBtnPlayer.firstFrame();
        
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
        
        //Default to adjusted speed
        session.slowBtnPlayer.setSpeed(2);
        session.normBtnPlayer.setSpeed(1);
        session.fastBtnPlayer.setSpeed(0.5);
        
        //Restart all video buttons
        session.fastBtnPlayer.firstFrame();
        session.normBtnPlayer.firstFrame();
        session.slowBtnPlayer.firstFrame();

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
void ofApp::Session::drawProgress(int startX, int endX, int y, float prog, int color) {
    drawProgress(startX, endX, y, prog, color, 8);
}

//--------------------------------------------------------------
void ofApp::Session::drawProgress(int startX, int endX, int y, float prog, int color, float barHeight) {
    
    ofSetHexColor(color);
    float progX = ofMap(prog, 1, 0, startX, endX);
    progX = (endX-startX) * prog;
    ofRect(startX, y, progX, barHeight);
    
    //draw ticks
//    ofSetColor(99,99,99,150);
//    ofSetLineWidth(2);
//    ofLine(startX, y, startX, y + barHeight + 4); // start
//    ofLine(endX, y, endX, y + barHeight + 4); // end
//    ofLine((startX+endX)/2, y, (startX+endX)/2, y + barHeight + 4); // half
//    ofLine((startX+endX)*.25, y, (startX+endX)*.25, y + barHeight + 4); // 1/4
//    ofLine((startX+endX)*.75, y, (startX+endX)*.75, y + barHeight + 4); // 3/4
    
    ofSetColor(255,255,255);
    
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
        normPlayer.draw(0, 0, VID_SIZE_BIG_W, VID_SIZE_BIG_H);
    }
    
    if(!slowVid.empty()) {
        //Change opacity based on how many vids are displaying
        if(fastVid.empty()) {
            ofSetColor(255,255,255,140);
        } else {
            ofSetColor(255,255,255,100);
        }
        slowPlayer.draw(0, 0, VID_SIZE_BIG_W, VID_SIZE_BIG_H);
    }
    
    if(!fastVid.empty()) {
        //Change opacity based on how many vids are displaying
        if(slowVid.empty()) {
            ofSetColor(255,255,255,140);
        } else {
            ofSetColor(255,255,255,100);
        }
        fastPlayer.draw(0,0, VID_SIZE_BIG_W, VID_SIZE_BIG_H);
    }

    ofSetColor(255,255,255);
    
}

//--------------------------------------------------------------
void ofApp::Session::drawButtonVids(){
    
    ofSetColor(255,255,255,255);
    
    if(!slowVid.empty()) {
        slowBtnPlayer.draw(1490,421, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        drawProgress(1490, 1490+VID_SIZE_SMALL_W, 421 + VID_SIZE_SMALL_H, slowBtnPlayer.getPosition(), getColor(0.5));
    }
    
    if(!normVid.empty()) {
        normBtnPlayer.draw(1490,180, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        drawProgress(1490, 1490+VID_SIZE_SMALL_W, 180 + VID_SIZE_SMALL_H, normBtnPlayer.getPosition(), getColor(1));
    }

    if(!fastVid.empty()) {
//        fastBtnPlayer.draw(1490,662, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
//        drawProgress(1490, 1490+VID_SIZE_SMALL_W, 662 + VID_SIZE_SMALL_H, fastBtnPlayer.getPosition(), getColor(2));
    }
    
}

//--------------------------------------------------------------
void ofApp::Session::restartVids(){
    
    if(!slowVid.empty()) {
        slowPlayer.firstFrame();
        slowPlayer.play();
    }
    
    if(!normVid.empty()) {
        normPlayer.firstFrame();
        normPlayer.play();
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

