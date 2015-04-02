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
    camOffset = ofVec2f(-30, -15); // Pixel offset from raw camera before scale-up
    
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
void ofApp::resetTracking(){
    drawPts.clear();
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
    if (vidTracker.isFrameNew()){
        
        timeElapsed = ofGetElapsedTimeMillis() - timeStarted;
        
        contourFinder.findContours(vidTracker);
        
        float tx = (camOffset.x + contourFinder.getCenter(0).x) * camRatio;
        float ty = (camOffset.y + contourFinder.getCenter(0).y) * camRatio;
        TrackPoint tp = TrackPoint(tx, ty, timeElapsed, isNewBeat);
        drawPts.push_back(tp);
        
        isNewBeat = false;

    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    //Draw Cams/Vids
    vidRaw.draw(0,0);
    vidTracker.draw(0,720);
    ofSetColor(255,100,0);
    contourFinder.draw();

    if (appState == STATE_TRACKING){
        drawTracking();
    }

    //Draw layout
    ofSetColor(255,255,255);
    layout.draw();
    
    //TEMP
    float timeSinceBeat = ofGetElapsedTimeMillis() - prevBeatTime;
    ofCircle(100, 850, 50 - (timeSinceBeat * 0.025));
        
}

//--------------------------------------------------------------
void ofApp::drawTracking(){
    
    //Draw current tracking
    drawLine.clear();
    ofSetColor(255,0,100);
    float beatWeight = ofMap(ofGetElapsedTimeMillis() - prevBeatTime, 0, (grooveTempo/groove.getSpeed()), 1, 10);
    for(int i = 0; i < drawPts.size(); i++){
        drawLine.curveTo(drawPts[i].x, drawPts[i].y);
        ofSetLineWidth(beatWeight);
    }
    drawLine.draw();
    ofSetColor(255,160,220);
    for(int i = 0; i < drawPts.size(); i++){
        if (drawPts[i].beat == true){
            float timeSinceCreation = timeElapsed - drawPts[i].time;
            ofCircle(drawPts[i].x, drawPts[i].y, 5);
        }
    }
    
    ofSetLineWidth(1);

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
    
    if (btn.substr(0,5) == "dance") {
        string speed = btn.substr(6);
        ofLogNotice("Speed chosen: "+speed);
        if (speed == "slow") {
            playMusic(currentMusic, 0.5);
        } else if (speed =="normal") {
            playMusic(currentMusic, 1);
        } else if (speed == "fast") {
            playMusic(currentMusic, 2);
        }
        resetTracking();
        appState = STATE_TRACKING;
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
ofApp::TrackPoint::TrackPoint(float x, float y, int time, bool beat){
    this->x = x;
    this->y = y;
    this->time = time;
    this->beat = beat;
}
