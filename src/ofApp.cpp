#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofEnableSmoothing();
    ofBackground(30,30,30);
    
    //Setup Cameras
    vidRecorder = ofPtr<ofQTKitGrabber>( new ofQTKitGrabber() );
    camRaw.setGrabber(vidRecorder);
    camRaw.setDeviceID(0);
    camRaw.initGrabber(1280/2,720/2);

    ofAddListener(vidRecorder->videoSavedEvent, this, &ofApp::videoSaved); // Listen for video saved events
    vidRecorder->initRecording();

    //Load Sounds
    jukebox.addSong("circle", "sounds/circle.wav", "sounds/startCircle.wav", 14567, 7107, 943);
    jukebox.addSong("triangle", "sounds/triangle.wav", "sounds/startTriangle.wav", 14567, 7107, 943);
    jukebox.addSong("square", "sounds/square.wav", "sounds/startSquare.wav", 14567, 7107, 943);
    jukebox.addSong("freestyle", "sounds/freestyle.wav", "sounds/startFreestyle.wav", 10769, 5103, 700);
    greatJobSnd.loadSound("sounds/great.wav");
    
    //Load/Setup UI
    layout.setupViews();
    layout.setView(DMLayout::VIEW_CHOOSE_PATTERN);
    appState = STATE_NORMAL;
    session.tempCombine = false;
    
}

//--------------------------------------------------------------
void ofApp::initRecording(){
    
    //Tracking
    resetBeatTracking();
    appState = STATE_RECORDING;
    
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
    
    //Wait 9 seconds then count down.
    resetBeatTracking();
    appState = STATE_PRE_COUNTDOWN;
    if (currentSpeed == 1)jukebox.playIntro();
    
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
    if (appState == STATE_RECORDING){

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
        
        if (timeElapsed > 6000){
            isNewBeat = true;
            appState = STATE_COUNTDOWN;
            layout.setView(DMLayout::VIEW_DANCE_VIEW); // TEMP: this shouldn't be necessary once interstital GetReady screen is its own thing
            layout.setState("txtGetReady", "1");
            resetBeatTracking();
            jukebox.play(currentSpeed);
        }
        
    } else if (appState == STATE_COUNTDOWN){
        
        if (isNewBeat == true){
            
            countdown--;
            isNewBeat = false;
            layout.setState("countdown", ofToString(countdown));
            
            if (countdown <= 0) {
                layout.setState("txtGetReady", "2");
                initRecording();                
            }
            
        }

    }
    
    //Loop playback
    if (appState == STATE_PLAYBACK && session.normBtnPlayer.getIsMovieDone() == true){
        
        session.restartVids();
        resetBeatTracking();
        
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    //Draw Cams/Vids
    ofPushMatrix();
    ofTranslate(-85, 0); //**--> Drawing is SHIFTED
    
    if (appState != STATE_PLAYBACK){
        
        camRaw.draw(0,0,1280,720);
        
        if (appState == STATE_RECORDING) {
            session.drawProgress(0, 1280, 720, recordProgress, session.getColor(currentSpeed), 32);
        }
        
    }

    ofPopMatrix(); //**--> Drawing is UN-SHIFTED
    
    //Draw layout
    ofSetColor(255,255,255);
    layout.draw();
    
    if (appState == STATE_PLAYBACK){
        session.drawRecordedVids();
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
    
    //Temp segragated playback
    if (btn == "double_playback_speed"){
        //speed up second vid
        session.slowBtnPlayer.setSpeed(2);
        session.slowBtnPlayer.firstFrame();
        session.normBtnPlayer.firstFrame();
        layout.setView(DMLayout::VIEW_PLAYBACK_2);
    } else if (btn == "combine") {
        //TODO set opacity and overlay
        session.tempCombine = true;
        layout.setView(DMLayout::VIEW_PLAYBACK_3);
    }

    
    if (btn.substr(0,13) == "chose_pattern") {
        string pattern = btn.substr(14);
        ofLogNotice("pattern"+pattern);
        if (pattern != "freestyle") {
            jukebox.switchSong(pattern);
            currentSpeed = 1;
            startDanceCountdown();
        } else {
            //Show freestyle music selection screen
            layout.setView(DMLayout::VIEW_CHOOSE_MUSIC);
        }
    }
    
    if (btn.substr(0,11) == "chose_music") {
        string music = btn.substr(12);
        
        jukebox.switchSong(music);
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
        
        session.restartVids();
        
    }
    
    if (btn == "start_over") {
        
        //clear all tracking
        resetBeatTracking();
        appState = STATE_NORMAL;
        
        layout.setView(DMLayout::VIEW_CHOOSE_PATTERN);
        vidPlayback.setLoopState(OF_LOOP_NORMAL);
        
        //delete all temp files
        clearFiles();
        
        //clear session data
        session.clear();
        
        //reset base time
        ofResetElapsedTimeCounter();
        
        session.tempCombine = false;
        
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
        
        //Which playback
        if (currentSpeed == 1) {
            //Finished first recording...
            
            //TEMP: should create separate view and countdown for second countdown?
            currentSpeed = 0.5;
            startDanceCountdown();
            //
            
            greatJobSnd.play();
            layout.setView(DMLayout::VIEW_GREAT_JOB);
        } else {
            //Finished second recording ...
            layout.setView(DMLayout::VIEW_PLAYBACK_1);
        }
        
        //Default to orignal speeds
        session.slowBtnPlayer.setSpeed(1);
        session.normBtnPlayer.setSpeed(1);
        session.fastBtnPlayer.setSpeed(1);
        
        session.restartVids();

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
    ofSetColor(255,255,255);
    
}

//--------------------------------------------------------------
void ofApp::Session::saveData(float speed, string vid){
    if (speed == 0.5) {
        slowVid = vid;
        
        slowBtnPlayer.stop();
        slowBtnPlayer.update();
        ofSleepMillis(50);
        slowBtnPlayer.close();
        slowBtnPlayer.loadMovie(slowVid);
        slowBtnPlayer.play();
        
    } else if (speed == 1) {
        normVid = vid;
        
        normBtnPlayer.stop();
        normBtnPlayer.update();
        ofSleepMillis(50);
        normBtnPlayer.close();
        normBtnPlayer.loadMovie(normVid);
        normBtnPlayer.play();
        
    } else if (speed == 2) {
        fastVid = vid;
        
        fastBtnPlayer.stop();
        fastBtnPlayer.update();
        ofSleepMillis(50);
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
        normBtnPlayer.update();
    }
    
    if(!slowVid.empty()) {
        slowBtnPlayer.update();
    }
    
    if(!fastVid.empty()) {
        fastBtnPlayer.update();
    }
    
}

//--------------------------------------------------------------
void ofApp::Session::drawRecordedVids(){
    
    ofSetColor(255,255,255,255);
    
    //Draw combined
    if (tempCombine==true){
        normBtnPlayer.draw(680,300, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        drawProgress(680, 680+VID_SIZE_SMALL_W, 300 + VID_SIZE_SMALL_H, normBtnPlayer.getPosition(), getColor(1));
        ofSetColor(255,255,255,140);
        slowBtnPlayer.draw(680,300, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        drawProgress(680, 680+VID_SIZE_SMALL_W, 300 + VID_SIZE_SMALL_H, slowBtnPlayer.getPosition(), getColor(1));
        return;
    }
    
    if(!slowVid.empty()) {
        slowBtnPlayer.draw(1000,300, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        drawProgress(1000, 1000+VID_SIZE_SMALL_W, 300 + VID_SIZE_SMALL_H, slowBtnPlayer.getPosition(), getColor(0.5));
    }
    
    if(!normVid.empty()) {
        if (slowVid.empty()) {
            normBtnPlayer.draw(680,300, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
            drawProgress(680, 680+VID_SIZE_SMALL_W, 300 + VID_SIZE_SMALL_H, normBtnPlayer.getPosition(), getColor(1));
        }else{
            normBtnPlayer.draw(300,300, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
            drawProgress(300, 300+VID_SIZE_SMALL_W, 300 + VID_SIZE_SMALL_H, normBtnPlayer.getPosition(), getColor(1));
        }
    }

    if(!fastVid.empty()) {
//        fastBtnPlayer.draw(1490,662, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
//        drawProgress(1490, 1490+VID_SIZE_SMALL_W, 662 + VID_SIZE_SMALL_H, fastBtnPlayer.getPosition(), getColor(2));
    }
    
}

//--------------------------------------------------------------
void ofApp::Session::restartVids(){

    if(!slowVid.empty()) {
        slowBtnPlayer.firstFrame();
        slowBtnPlayer.play();
    }
    
    if(!normVid.empty()) {
        normBtnPlayer.firstFrame();
        normBtnPlayer.play();
    }

    if(!fastVid.empty()) {
        fastBtnPlayer.firstFrame();
        fastBtnPlayer.play();
    }
    
}

//--------------------------------------------------------------
int ofApp::Session::getColor(float speed){
    
    //temp
    return ofHexToInt("FF0000");
    
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
    //stop all players before closing,
    //and sleep to allow any related callbacks
    //see http://goo.gl/Vz8zdx
    //also http://goo.gl/WJRc8O

    slowBtnPlayer.stop();
    normBtnPlayer.stop();
    fastBtnPlayer.stop();

    slowBtnPlayer.update();
    normBtnPlayer.update();
    fastBtnPlayer.update();
    
    ofSleepMillis(50);

    slowBtnPlayer.close();
    normBtnPlayer.close();
    fastBtnPlayer.close();
    
}

