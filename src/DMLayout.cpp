//
//  DMLayout.cpp
//  danceMath
//
//  Created by Trygve Nordberg on 3/27/15.
//
//

#include "DMLayout.h"

//View Constants
string DMLayout::VIEW_SELECT = "view_select";
string DMLayout::VIEW_SELECT_TRACK = "view_select_track";
string DMLayout::VIEW_RECORD = "view_record";
string DMLayout::VIEW_PLAYBACK_1 = "view_playback1";
string DMLayout::VIEW_PLAYBACK_2 = "view_playback2";
string DMLayout::VIEW_PLAYBACK_3 = "view_playback3";
string DMLayout::VIEW_SCREENSAVER = "view_screensaver";

void DMLayout::setupViews(){

    //Select Screen
    image("select_bg_en", 0, 0);
    image("select_tabLeft_en", -46, 246);
    image("select_tabRight_en", 1375, 246);
    videoButton("select_btnCircle_en", 478, 407, "chose_pattern_circle");
    videoButton("select_btnTriangle_en", 47, 407, "chose_pattern_triangle");
    videoButton("select_btnSquare_en", 912, 407, "chose_pattern_square");
    videoButton("select_btnFreestyle_en", 1418, 407, "chose_pattern_freestyle");
//    button("btnLang_en", 44, 952, "toggle_language");
    saveView(VIEW_SELECT);

    //Choose your music (Advanced)
    video("selectTrack_bg_en", 0, 0);
    button("selectTrack_tab1_en", 1117, 254, "chose_music_freestyle1");
    button("selectTrack_tab2_en", 1117, 432, "chose_music_freestyle2");
    button("selectTrack_tab3_en", 1117, 610, "chose_music_freestyle3");
    button("selectTrack_tab4_en", 1117, 788, "chose_music_freestyle4");
    saveView(VIEW_SELECT_TRACK);
    
    //Record View (Populated dynamically)
    saveView(VIEW_RECORD);

    //Playback 1 (Diff Speeds)
    video("playback1_bg_en", 0, 0, false);
    image("playback_tabRight1_en", 970, 239);
    image("playback_tabLeft_en", -250, 245);
    button("playback_btnSpeed_en", 970, 911, "double_playback_speed");
    saveView(VIEW_PLAYBACK_1);
    
    //Playback 2 (Sped up to match)
    video("playback2_bg_en", 0, 0, false);
    image("playback_tabRight2_en", 970, 239);
    image("playback_tabLeft_en", -250, 245);
    button("playback_btnCombine_en", 485, 912, "combine");
    saveView(VIEW_PLAYBACK_2);
    
    //Playback 3 (Combined Fullscreen)
    video("playback3_bg_en", 0, 0, false);
    button("btnStartOver_en", 1490, 955, "start_over");
    saveView(VIEW_PLAYBACK_3);
    
    //Screensaver
    video("screensaver", 0, 0);
    saveView(VIEW_SCREENSAVER);

}

void DMLayout::setView(string name){
        
    baseViewId = name;
    string newViewId = "" + baseViewId + languageKey;
    
    //Language specific view.
    if (viewExists(newViewId) == false) {
        //Fallback to language agnostic view.
        newViewId = "" + baseViewId;
    }
    
    ofLogNotice("setView()", newViewId);
    
    ofxSimpleLayout::Layout::setView(newViewId);

}

void DMLayout::draw(){

    Layout::draw();
    
}

void DMLayout::updateLanguage(string lang){
    
    languageKey = lang;
    
    //Refresh current view using language
    if (!this->baseViewId.empty()){
        this->setView(this->baseViewId);
    }
    
}


