#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofEnableSmoothing();
    ofBackground(30,30,30);
    
    //List out cameras
    vector<ofVideoDevice> devices = camRaw.listDevices();
    
    for(int i = 0; i < devices.size(); i++){
        cout << devices[i].id << ": " << devices[i].deviceName << " : " << devices[i].hardwareName;
        if( devices[i].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl;
        }
    }
    
    //Setup Cameras
    vidRecorder = ofPtr<ofQTKitGrabber>( new ofQTKitGrabber() );
    camRaw.setGrabber(vidRecorder);
    camRaw.setDeviceID(camRawDeviceID);    
    camTracker.setDeviceID(camTrackerDeviceID);
    camTracker.initGrabber(320,180);
    camRaw.initGrabber(1280,720);
    
    printf("a --- camTracker : asked for 320 by 180 - actual size is %i by %i \n", camTracker.width, camTracker.height);
    printf("a --- camRaw: asked for 1280 by 720 - actual size is %i by %i \n", camRaw.width, camRaw.height);
           
    ofAddListener(vidRecorder->videoSavedEvent, this, &ofApp::videoSaved); // Listen for video saved events
    vidRecorder->initRecording();
    
    camRatio = camRaw.height / camTracker.height;
    camOffset = ofVec2f(3.5, 30); // Pixel offset between raw camera and IR camera (before scale-up)
 
    //Setup Tracking
    contourFinder.setMinAreaRadius(0); // Filter small blobs
    contourFinder.setMaxAreaRadius(15); // Filter large blobs
    contourFinder.setThreshold(250); // Filter non-white blobs
    
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
    resetTracking();
    appState = STATE_TRACKING;
    
    //Video recording
    stringstream s;
    s << "temp/xDANCE_" << ofGetUnixTime() << ".mov";
    vidRecorder->startRecording(s.str());
    
    ofLogNotice("initRecording() ") << "Start recording: " << s;
    
}

//--------------------------------------------------------------
void ofApp::resetTracking(){ resetTracking(true); };
void ofApp::resetTracking(bool resetPts){
    if (resetPts) drawPts.clear();
    timeStarted = prevBeatTime = ofGetElapsedTimeMillis();
}


//--------------------------------------------------------------
void ofApp::startDanceCountdown(){
    
    ofSoundStopAll();
    layout.setView(DMLayout::VIEW_DANCE_VIEW);
    
    countdown = 9;
    layout.setState("countdown", ofToString(countdown));
    
    //Wait 5 seconds then count down.
    resetTracking();
    appState = STATE_PRE_COUNTDOWN;
    

}

//--------------------------------------------------------------
void ofApp::update(){
    
    layout.update();
    
    if (appState == STATE_PLAYBACK) {
        vidPlayback.update();
    } else {
        camRaw.update();
        camTracker.update();
    }

    //Update beat timer
    if ((ofGetElapsedTimeMillis() - prevBeatTime) >= (jukebox.tempo / jukebox.rate)){
        prevBeatTime = ofGetElapsedTimeMillis();
        isNewBeat = true;
    }
    
    timeElapsed = ofGetElapsedTimeMillis() - timeStarted;

    //Update tracking
    if (appState == STATE_TRACKING && camTracker.isFrameNew()){
        
        contourFinder.findContours(camTracker);
        
        if (contourFinder.size() > 0){
            float tx = (camOffset.x + contourFinder.getCenter(0).x) * camRatio;
            float ty = (camOffset.y + contourFinder.getCenter(0).y) * camRatio;
            TrackPoint tp = TrackPoint(tx, ty, timeElapsed, isNewBeat);
            drawPts.push_back(tp);
        }

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
            resetTracking();
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
    if (appState == STATE_PLAYBACK && vidPlayback.getIsMovieDone() == true){
        
        vidPlayback.firstFrame();
        vidPlayback.play();
        resetTracking(false);
        
    }
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    //Draw Cams/Vids
    
    ofPushMatrix();
    ofTranslate(1280, 0, 0);
    ofScale(-1, 1); //**--> Drawing is MIRRORED
    
    if (appState == STATE_PLAYBACK){
        vidPlayback.draw(0,0);
    } else {
        camRaw.draw(0,0,1280,960);
    }

    if (appState != STATE_NORMAL){
        drawTracking();
    }
    
    #ifdef DEBUG_HELPERS
        ofPushMatrix();
        ofTranslate(0, 720);
        ofSetColor(255,255,255);
        camTracker.draw(0,0);
        ofSetColor(255,100,0);
        contourFinder.draw();
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
void ofApp::drawTracking(){
    
    if (appState == STATE_TRACKING){

        drawTrackedLine(drawPts, session.getColor(currentSpeed), false);
        
    } else if (appState == STATE_PLAYBACK) {
        
        if(!session.slowPts.empty()) drawTrackedLine(session.slowPts, session.getColor(0.5), true);
        if(!session.normPts.empty()) drawTrackedLine(session.normPts, session.getColor(1), true);
        if(!session.fastPts.empty()) drawTrackedLine(session.fastPts, session.getColor(2), true);
        
    }
    
    ofSetLineWidth(1);

}

//--------------------------------------------------------------
void ofApp::drawTrackedLine(vector<TrackPoint> pts, int color, bool useTime){
    
    drawLine.clear();
    
    //Line
    ofSetLineWidth(4);
    ofSetHexColor(color);
    if (!pts.empty()) drawLine.addVertex(pts[0].x, pts[0].y);
    for(int i = 0; i < pts.size(); i++){
        if (useTime == false || pts[i].time <= ofGetElapsedTimeMillis() - timeStarted){
            drawLine.curveTo(pts[i].x, pts[i].y);
        } else {
            drawLine.curveTo(pts[i].x, pts[i].y);
            break;
        }
    }
    if (useTime == false && !pts.empty()) drawLine.addVertex(pts[pts.size()-1].x, pts[pts.size()-1].y);
    drawLine.draw();
    
    //Dots
    ofSetColor(255,255,255);
    for(int i = 0; i < pts.size(); i++){
        if (pts[i].beat == true){
            if (useTime == false || pts[i].time <= ofGetElapsedTimeMillis() - timeStarted){
                ofCircle(pts[i].x, pts[i].y, 7);
            }
        }
    }
        
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //Change raw cam source
    if(key == 'r') {
        
        int w = camRaw.width;
        int h = camRaw.height;
        camRaw.close();
        camRawDeviceID = (camRawDeviceID + 1) % camRaw.listDevices().size();
        camRaw.setGrabber(vidRecorder);
        camRaw.setDeviceID(camRawDeviceID);
        camRaw.initGrabber(w,h);
        
        printf("r--- camRaw: new device id: %i \n", camRawDeviceID);
        printf("r--- camRaw: asked for %i by %i - actual size is %i by %i \n", w, h, camRaw.width, camRaw.height);
        printf("r--- camTracker current size: %i by %i \n", camTracker.width, camTracker.height);
        
    }
    //Change IR cam source
    if(key == 'i') {
        
        int w = camTracker.width;
        int h = camTracker.height;
        camTracker.close();
        camTrackerDeviceID = (camTrackerDeviceID + 1) % camTracker.listDevices().size();
        camTracker.setDeviceID(camTrackerDeviceID);
        camTracker.initGrabber(w,h);
        
        printf("i--- camTracker: new device id: %i \n", camTrackerDeviceID);
        printf("i--- camTracker : asked for %i by %i - actual size is %i by %i \n", w, h, camTracker.width, camTracker.height);
        printf("i--- camRaw current size: %i by %i \n", camRaw.width, camRaw.height);
        
    }
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
        resetTracking();
        appState = STATE_NORMAL;
        
        layout.setView(DMLayout::VIEW_PICK_DANCE);
        vidPlayback.setLoopState(OF_LOOP_NORMAL);
        
        //delete all temp files
        clearFiles();
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
        ofLogNotice("vidPath: "+vidPath);
        
        //Save to session
        session.saveData(currentSpeed, drawPts, vidPath);
        
        //Start large playback of most recently recorded video
        vidPlayback.close();
        vidPlayback.loadMovie(vidPath);
        vidPlayback.play();
        vidPlayback.setLoopState(OF_LOOP_NONE);
        
        resetTracking(false);
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
ofApp::TrackPoint::TrackPoint(float x, float y, int time, bool beat){
    this->x = x;
    this->y = y;
    this->time = time;
    this->beat = beat;
}

//--------------------------------------------------------------
void ofApp::Session::saveData(float speed, vector<TrackPoint> pts, string vid){
    if (speed == 0.5) {
        slowPts = pts;
        slowVid = vid;
    } else if (speed == 1) {
        normPts = pts;
        normVid = vid;
    } else if (speed == 2) {
        fastPts = pts;
        fastVid = vid;
    } else {
        ofLogError("Session") << "Can not save session. Unrecognized speed: " << speed;
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
    slowPts.clear();
    normPts.clear();
    fastPts.clear();
    slowVid = normVid = fastVid = "";
}

