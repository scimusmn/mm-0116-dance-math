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
    image("select_bg_en");
    image("select_tabLeft_en");
    image("select_tabRight_en");
    videoButton("select_btnCircle_en", 682, 233, "chose_pattern_circle");
    videoButton("select_btnTriangle_en", 206, 636, "chose_pattern_triangle");
    videoButton("select_btnSquare_en", 682, 636, "chose_pattern_square");
    videoButton("select_btnFreestyle_en", 1255, 636, "chose_pattern_freestyle");
    button("btnLang_en", 80, 972, "toggle_language");
    saveView(VIEW_SELECT);

    //Choose your music (Advanced)
    video("selectTrack_bg_en", 0, 0);
    button("selectTrack_tab1_en", 520, 300, "chose_music_freestyle_song1");
    button("selectTrack_tab2_en", 520, 480, "chose_music_freestyle_song2");
    button("selectTrack_tab3_en", 520, 660, "chose_music_freestyle_song3");
    button("selectTrack_tab4_en", 520, 840, "chose_music_freestyle_song4");
    saveView(VIEW_SELECT_TRACK);
    
    //Record View (Populated dynamically)
    saveView(VIEW_RECORD);

    //Playback 1 (Diff Speeds)
    image("playback1_bg_en");
    image("playback_tabRight1_en", 0, 0);
    image("playback_tabLeft1_en", 800, 0);
    button("playback_btnSpeed_en", 1225, 830, "double_playback_speed");
    saveView(VIEW_PLAYBACK_1);
    
    //Playback 2 (Sped up to match)
    image("playback2_bg_en");
    image("playback_tabRight2_en", 0, 0);
    image("playback_tabLeft2_en", 800, 0);
    button("playback_btnCombine_en", 1225, 830, "combine");
    saveView(VIEW_PLAYBACK_2);
    
    //Playback 3 (Combined Fullscreen)
    button("btnLang_en", 80, 972, "toggle_language");
    button("btnStartOver_en", 980, 972, "start_over");
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
    
    ofxSimpleLayout::Layout::setView(newViewId);
    
}

void DMLayout::updateLanguage(string lang){
    
    languageKey = lang;
    
    //Refresh current view using language
    if (!this->baseViewId.empty()){
        this->setView(this->baseViewId);
    }
    
}


