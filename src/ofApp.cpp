#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofEnableSmoothing();
    
    //Load sounds
    groove.loadSound("sounds/groove.wav");
    
    //Load/Setup UI
    layout.setupViews();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    layout.update();
    
    bpmRadius-=1;
    if ((ofGetElapsedTimeMillis()-prevT) >= grooveTempo ){
        bpmRadius = 100;
        prevT = ofGetElapsedTimeMillis();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    layout.draw();
    
    ofCircle(100, 100, bpmRadius);
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
    ofLogNotice(""+btn);
    
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
