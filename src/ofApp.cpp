#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofEnableSmoothing();
    
    //Setup Cameras
    vidRaw.loadMovie("videos/light720.mov");
    vidTracker.loadMovie("videos/light480.mov");
    vidRaw.play();
    vidTracker.play();
    
    //Setup Tracking
    contourFinder.setMinAreaRadius(5); // Filter small blobs
    contourFinder.setMaxAreaRadius(15); // Filter large blobs
    contourFinder.setThreshold(254); // Filter non-white blobs
    
    //Load Sounds
    groove.loadSound("sounds/groove.wav");
    
    //Load/Setup UI
    layout.setupViews();
        
}

//--------------------------------------------------------------
void ofApp::resetTracking(){
    drawPts.clear();
    timeStarted = prevBeatTime = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::update(){
    layout.update();
    
    if ((ofGetElapsedTimeMillis() - prevBeatTime) >= (grooveTempo/groove.getSpeed())){
        prevBeatTime = ofGetElapsedTimeMillis();
        isNewBeat = true;
    }
    
    vidRaw.update();
    vidTracker.update();
    if (vidTracker.isFrameNew()){
        
        contourFinder.findContours(vidTracker);
        
        timeElapsed = ofGetElapsedTimeMillis() - timeStarted;
        TrackPoint tp = TrackPoint(contourFinder.getCenter(0).x, contourFinder.getCenter(0).y, timeElapsed, isNewBeat);
        drawPts.push_back(tp);
        isNewBeat = false;
        
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //Draw Cams/Vids
//    vidRaw.draw(0,0);
    vidTracker.draw(0,0);
    ofSetColor(255,100,0);
//    contourFinder.draw();
    
    
    //Draw current tracking
    ofPolyline drawLine;
    ofSetColor(255,0,100);
    float beatWeight = ofMap(ofGetElapsedTimeMillis() - prevBeatTime, 0, (grooveTempo/groove.getSpeed()), 1, 10);
    for(int i = 0; i < drawPts.size(); i++){
        drawLine.curveTo(drawPts[i].x, drawPts[i].y);
        ofSetLineWidth(beatWeight);
    }
    drawLine.draw();
    ofSetColor(255,160,220);
    int beatCount = 0;
    for(int i = 0; i < drawPts.size(); i++){
        if (drawPts[i].beat == true){
            beatCount++;
            float timeSinceCreation = timeElapsed - drawPts[i].time;
            ofCircle(drawPts[i].x, drawPts[i].y, 5);
//            ofDrawBitmapString(ofToString(beatCount), drawPts[i].x - 5, drawPts[i].y + 5);
        }
    }
    ofSetLineWidth(1);
    ofSetColor(255,255,255);

    //Draw layout
    layout.draw();
    
    //TEMP
    float timeSinceBeat = ofGetElapsedTimeMillis() - prevBeatTime;
    ofCircle(100, 450, 50-(timeSinceBeat*.025));
    
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
    
    if (btn == "dance_slow") {
        groove.setSpeed(0.5);
        groove.play();
    } else if (btn =="dance_normal") {
        groove.setSpeed(1);
        groove.play();
    } else if (btn == "dance_fast") {
        groove.setSpeed(2);
        groove.play();
    }
    
    resetTracking();
    
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
