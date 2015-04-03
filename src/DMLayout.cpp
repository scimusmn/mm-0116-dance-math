//
//  DMLayout.cpp
//  danceMath
//
//  Created by Trygve Nordberg on 3/27/15.
//
//

#include "DMLayout.h"

//View Constants
string DMLayout::VIEW_PICK_DANCE = "view_pick_dance";
string DMLayout::VIEW_CHOOSE_MUSIC = "view_choose_music";
string DMLayout::VIEW_DANCE_VIEW = "view_dance_view";
string DMLayout::VIEW_PLAYBACK = "view_playback";

void DMLayout::setupViews(){

    //Pick a Dance
    image("main_bg");
    image("sidebar_bg");
    image("txtPickADance");
    videoButton("chooseDanceA", 50, 50, "chose_dance_a");
    videoButton("chooseDanceB", 550, 50, "chose_dance_b");
    videoButton("chooseDanceC", 50, 450, "chose_dance_c");
    videoButton("chooseDanceD", 550, 450, "chose_dance_d");
    saveView(VIEW_PICK_DANCE);
    
    //Choose your music
    image("main_bg");
    image("sidebar_bg");
    image("txtChooseYourMusic");
    button("button", 250, 150, "preview_music_groove");
    button("button", 250, 250, "chose_music_groove");
    saveView(VIEW_CHOOSE_MUSIC);
    
    //Dance View
    image("sidebar_bg");
    image("txtGetReady");
    stateObject("countdown", 1500, 500);
    button("button", 1420, 600, "start_dance_1");
    saveView(VIEW_DANCE_VIEW);
    
    //Playback View
    image("sidebar_bg");
    image("txtPlayback");
    button("button", 1250, 575, "start_dance_0.5");
    button("button", 1550, 575, "start_dance_2");
    button("button", 1400, 750, "start_over");
    saveView(VIEW_PLAYBACK);

}

void DMLayout::startCountdown(){
    countdownStartTime = ofGetElapsedTimef();
}

void DMLayout::update(){
    
    /*
    if (countdownStartTime > 0) {
        int secs = 5 - floor(ofGetElapsedTimef() - countdownStartTime);
        if (secs >= 0) {
            setState("countdown", ofToString(secs));
        } else {
            countdownStartTime = -1;
        }
    }
    */
    
    Layout::update();
    
}

