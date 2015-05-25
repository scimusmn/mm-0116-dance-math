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
    cam.initGrabber(VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
    ofAddListener(vidRecorder->videoSavedEvent, this, &ofApp::videoSaved); // Listen for video saved events
    vidRecorder->initRecording();

    //Load Sounds
    jukebox.addSong("circle", "sounds/circle.wav", "sounds/startCircle.wav", 14567, 7107, 943);
    jukebox.addSong("triangle", "sounds/triangle.wav", "sounds/startTriangle.wav", 14567, 7107, 943);
    jukebox.addSong("square", "sounds/square.wav", "sounds/startSquare.wav", 14567, 7107, 943);
    jukebox.addSong("fs_song1", "sounds/song1.wav", "sounds/startFreestyle.wav", 10769, 5103, 700); // HIP-HOP
    jukebox.addSong("fs_song2", "sounds/song2.wav", "sounds/startFreestyle.wav", 10629, 5374, 672); // PIANO
    jukebox.addSong("fs_song3", "sounds/song3.wav", "sounds/startFreestyle.wav", 9999, 5177, 642); // DUBSTEP
    jukebox.addSong("fs_song4", "sounds/song4.wav", "sounds/startFreestyle.wav", 8650, 4513, 567); // ROCK & ROLL
    greatJobSnd.loadSound("sounds/great.wav");
    
    //Load/Setup UI
    layout.setupViews();
    layout.setView(DMLayout::VIEW_CHOOSE_PATTERN);
    appState = STATE_NORMAL;
    
}

//--------------------------------------------------------------
void ofApp::initRecording(){
    
    //Switch app state
    resetBeatTracking();
    appState = STATE_RECORDING;
    
    //New video file in /temp folder
    stringstream s;
    s << "temp/video_" << ofGetUnixTime() << ".mov";
    vidRecorder->startRecording(s.str());
    
}

//--------------------------------------------------------------
void ofApp::resetBeatTracking(){
    
    //Remember current time to count beats after "now"
    timeStarted = prevBeatTime = ofGetElapsedTimeMillis();
    timeElapsed = recordProgress = 0;
    
}

//--------------------------------------------------------------
void ofApp::resetInactivity(){
    
    inactivityCount = 0;
    
}

//--------------------------------------------------------------
void ofApp::startDanceCountdown(){
    
    //Show live camera feed and countdown panel
    layout.setView(DMLayout::VIEW_DANCE_VIEW);
    countdown = 9;
    layout.setState("countdown", ofToString(countdown));
    
    //Display intro message based on which pattern is chosen
    string stId = "0";
    if (jukebox.id.substr(0,2) == "fs") {
        stId = "0";
    } else if (jukebox.id == "circle") {
        stId = "3";
    } else if (jukebox.id == "square") {
        stId = "4";
    } else if (jukebox.id == "triangle") {
        stId = "5";
    }
    layout.setState("txtGetReady", stId);

    //Pre-countdown sequence.
    preCountdownDuration = 6250;
    if (currentSpeed == 1) {
        
        //Longer intro for freestyle
        if (jukebox.id.substr(0,2) == "fs") {
            preCountdownDuration = 9250;
        }
        
       jukebox.playIntro();
        
    } else {
        
        //NOT first recording, play "great job" audio.
        greatJobSnd.play();
        layout.setView(DMLayout::VIEW_GREAT_JOB);
        
    }
    resetBeatTracking();
    appState = STATE_PRE_COUNTDOWN;
    
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

    //Update beat timer
    if ((ofGetElapsedTimeMillis() - prevBeatTime) >= (jukebox.tempo / jukebox.rate)){
        prevBeatTime = ofGetElapsedTimeMillis();
        isNewBeat = true;
    }
    timeElapsed = ofGetElapsedTimeMillis() - timeStarted;
    recordProgress = timeElapsed / ((jukebox.duration - jukebox.intro) / jukebox.rate);

    //Update recording
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
        
        if (timeElapsed > preCountdownDuration){
            isNewBeat = true;
            appState = STATE_COUNTDOWN;
            layout.setView(DMLayout::VIEW_DANCE_VIEW);
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
    if (appState == STATE_PLAYBACK && session.normVidPlayer.getIsMovieDone() == true){
        
        session.restartVids();
        resetBeatTracking();
        
    }
    
    //Screensaver
    if (appState == STATE_PRE_COUNTDOWN || appState == STATE_COUNTDOWN || appState == STATE_RECORDING) {
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
    ofTranslate(-85, 0); //**--> Drawing is SHIFTED
    
    if (appState != STATE_PLAYBACK){
        
        cam.draw(0,0,1280,720);
        
        if (appState == STATE_RECORDING) {
            session.drawProgress(0, 1280, 720, recordProgress, session.getColor(currentSpeed), 32);
        }
        
    }

    ofPopMatrix(); //**--> Drawing is UN-SHIFTED
    
    //Draw layout
    ofSetColor(255,255,255);
    layout.draw();
    
    if (appState == STATE_PLAYBACK){
        if (layout.currentViewId == DMLayout::VIEW_PLAYBACK_3) {
            //Combine videos
            session.drawRecordedVids(true);
        } else {
            //Separate videos
            session.drawRecordedVids(false);
        }
    }

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    string btn = layout.getSelected(x, y);

    if (btn.substr(0,13) == "chose_pattern") {
        
        string pattern = btn.substr(14);
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
    
    if (btn == "start_over") {
        
        startOver();
        
    }
    
    if( appState == STATE_SCREENSAVER ) {
        //Awake from screensaver
        startOver();
    }
    
    resetInactivity();
    
}

//--------------------------------------------------------------
void ofApp::startOver(){
    
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
            currentSpeed = 0.5;
            startDanceCountdown();

        } else {
            
            //Finished second recording ...
            layout.setView(DMLayout::VIEW_PLAYBACK_1);
            
        }
        
        //Default to original speeds
        session.slowVidPlayer.setSpeed(1);
        session.normVidPlayer.setSpeed(1);
        
        session.restartVids();
        
        resetInactivity();
        
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
        
        slowVidPlayer.stop();
        slowVidPlayer.update();
        ofSleepMillis(50);
        slowVidPlayer.close();
        slowVidPlayer.loadMovie(slowVid);
        slowVidPlayer.play();
        
    } else if (speed == 1) {
        normVid = vid;
        
        normVidPlayer.stop();
        normVidPlayer.update();
        ofSleepMillis(50);
        normVidPlayer.close();
        normVidPlayer.loadMovie(normVid);
        normVidPlayer.play();
        
    } else {
        ofLogError("Session") << "Can not save session. Unrecognized speed: " << speed;
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
void ofApp::Session::drawRecordedVids(bool combine){
    
    ofSetColor(255,255,255,255);
    
    if (combine == true){
        
        //Combined
        ofSetColor(255,255,255,255);
        normVidPlayer.draw(680, 300, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        drawProgress(680, 680+VID_SIZE_SMALL_W, 300 + VID_SIZE_SMALL_H, normVidPlayer.getPosition(), getColor(1));
        
        ofSetColor(255, 255, 255, 140);
        slowVidPlayer.draw(680, 300, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        drawProgress(680, 680+VID_SIZE_SMALL_W, 300 + VID_SIZE_SMALL_H, slowVidPlayer.getPosition(), getColor(1));
        
    } else {
        
        //Separated
        normVidPlayer.draw(300, 300, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        drawProgress(300, 300+VID_SIZE_SMALL_W, 300 + VID_SIZE_SMALL_H, normVidPlayer.getPosition(), getColor(1));
        
        slowVidPlayer.draw(1000, 300, VID_SIZE_SMALL_W, VID_SIZE_SMALL_H);
        drawProgress(1000, 1000+VID_SIZE_SMALL_W, 300 + VID_SIZE_SMALL_H, slowVidPlayer.getPosition(), getColor(0.5));
        
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
int ofApp::Session::getColor(float speed){
    
    //Note: Leaving this function if we want
    //to add color-coding later. -tnordberg
    return ofHexToInt("FF0000");

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

