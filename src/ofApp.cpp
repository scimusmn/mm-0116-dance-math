#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofEnableSmoothing();
    
    //Setup Cameras
    vidRaw.loadMovie("videos/light720.mov");
    vidTracker.loadMovie("videos/light480_offset.mov");
    vidRaw.play();
    vidTracker.play();
    camRatio = vidRaw.height / vidTracker.height;
    camOffset = ofVec2f(-30, -15); // Pixel offset between raw camera and IR camera (before scale-up)
    
    //Video Playback
    vidRecorder = ofPtr<ofQTKitGrabber>( new ofQTKitGrabber() );
    camRaw.setGrabber(vidRecorder);
    camRaw.initGrabber(1280/2, 720/2);
    ofAddListener(vidRecorder->videoSavedEvent, this, &ofApp::videoSaved); // Listen for video saved events
    vidRecorder->initRecording();
    
    //Setup Tracking
    contourFinder.setMinAreaRadius(5); // Filter small blobs
    contourFinder.setMaxAreaRadius(15); // Filter large blobs
    contourFinder.setThreshold(252); // Filter non-white blobs
    
    //Load Sounds
    groove.loadSound("sounds/groove.wav");
    
    //Load/Setup UI
    layout.setupViews();
    layout.setView(DMLayout::VIEW_PICK_DANCE);
    appState = STATE_NORMAL;
    
}

//--------------------------------------------------------------
void ofApp::resetTracking(){ resetTracking(true); };
void ofApp::resetTracking(bool resetPts){
    if (resetPts) drawPts.clear();
    timeStarted = prevBeatTime = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::playMusic(string song, float rate){
    if (song == "groove") {
        groove.setSpeed(rate);
        groove.play();
    } else if (song == "country") {
        country.setSpeed(rate);
        country.play();
    } else if (song == "waltz") {
        waltz.setSpeed(rate);
        waltz.play();
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    layout.update();
    vidRaw.update();
    vidTracker.update();

    //Update beat timer
    if ((ofGetElapsedTimeMillis() - prevBeatTime) >= (grooveTempo/groove.getSpeed())){
        prevBeatTime = ofGetElapsedTimeMillis();
        isNewBeat = true;
    }

    //Update tracking
    if (appState == STATE_TRACKING && vidTracker.isFrameNew()){
        
        timeElapsed = ofGetElapsedTimeMillis() - timeStarted;
        
        contourFinder.findContours(vidTracker);
        
        float tx = (camOffset.x + contourFinder.getCenter(0).x) * camRatio;
        float ty = (camOffset.y + contourFinder.getCenter(0).y) * camRatio;
        TrackPoint tp = TrackPoint(tx, ty, timeElapsed, isNewBeat);
        drawPts.push_back(tp);
        
        isNewBeat = false;
        
        //End recording
        if (timeElapsed >= SONG_DURATION / currentSpeed) {
            
            appState = STATE_NORMAL;

            //Attempt saving RGB video, and wait for success callback
            if(vidRecorder->isRecording()){
                vidRecorder->stopRecording();
            } else {
                ofLogError("Cannot save video") << "video was still recording: " << vidRecorder->isRecording();
            }

        }

    }
    
    //Loop playback
    if (appState == STATE_PLAYBACK){
        if (vidRaw.getIsMovieDone() == 1){
            vidRaw.firstFrame();
            vidRaw.play();
            resetTracking(false);
        }
    }
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    //Draw Cams/Vids
    vidRaw.draw(0,0);
    vidTracker.draw(0,720);
    ofSetColor(255,100,0);
    contourFinder.draw();

    if (appState != STATE_NORMAL){
        drawTracking();
    }

    //Draw layout
    ofSetColor(255,255,255);
    layout.draw();
    
    //TEMP
    float timeSinceBeat = ofGetElapsedTimeMillis() - prevBeatTime;
    ofCircle(100, 850, 50 - (timeSinceBeat * 0.025));
    ofDrawBitmapString(ofToString(ofGetFrameRate())+"fps", 20, 20);
    
}

//--------------------------------------------------------------
void ofApp::drawTracking(){
    
    if (appState == STATE_TRACKING){

        drawTrackedLine(drawPts, session.getColor(currentSpeed), false);
        
    } else if (appState == STATE_PLAYBACK) {
        
        if(!session.slowPts.empty()) drawTrackedLine(session.slowPts, ofHexToInt("FF0000"), true);
        if(!session.normPts.empty()) drawTrackedLine(session.normPts, ofHexToInt("00FF00"), true);
        if(!session.fastPts.empty()) drawTrackedLine(session.fastPts, ofHexToInt("0000FF"), true);
        
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
    ofSetColor(255,255,255,200);
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
        playMusic(music, 1);
    }
    
    if (btn.substr(0,11) == "chose_music") {
        string music = btn.substr(12);
        ofLogNotice("Music chosen: "+music);
        currentMusic = music;
        layout.setView(DMLayout::VIEW_DANCE_VIEW);
        layout.startCountdown();
    }
    
    if (btn.substr(0,11) == "start_dance") {
        
        string speed = btn.substr(12);
        currentSpeed = ofToFloat(speed);
        ofLogNotice("Speed chosen: "+speed);
        
        //TODO: Wait for countdown.
        
        resetTracking();
        playMusic(currentMusic, currentSpeed);
        appState = STATE_TRACKING;
        
        //Start video recording
        stringstream s; //Time-based filename
        s << "temp/xDANCE_" << ofGetUnixTime() << ".mov";
        vidRecorder->startRecording(s.str());
        
    }
    
    if (btn == "start_over") {
        
        //clear all tracking
        resetTracking();
        appState = STATE_NORMAL;
        
        layout.setView(DMLayout::VIEW_PICK_DANCE);
        vidRaw.setLoopState(OF_LOOP_NORMAL);
        
        //delete all temp files
        clearFiles();
        session.clear();
        
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

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
        vidRaw.close();
        vidRaw.loadMovie(vidPath);
        vidRaw.play();
        vidRaw.setLoopState(OF_LOOP_NONE);
        
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

