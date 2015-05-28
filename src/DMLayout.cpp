//
//  DMLayout.cpp
//  danceMath
//
//  Created by Trygve Nordberg on 3/27/15.
//
//

#include "DMLayout.h"

//View Constants
string DMLayout::VIEW_CHOOSE_PATTERN = "view_choose_pattern";
string DMLayout::VIEW_CHOOSE_PATTERN_SPANISH = "view_choose_pattern_es";
string DMLayout::VIEW_CHOOSE_MUSIC = "view_choose_music";
string DMLayout::VIEW_DANCE_VIEW = "view_dance_view";
string DMLayout::VIEW_GREAT_JOB = "view_great_job";
string DMLayout::VIEW_PLAYBACK_1 = "view_playback_1";
string DMLayout::VIEW_PLAYBACK_2 = "view_playback_2";
string DMLayout::VIEW_PLAYBACK_3 = "view_playback_3";
string DMLayout::VIEW_SCREENSAVER = "view_screensaver";

void DMLayout::setupViews(){

    //Choose a pattern
    image("main_bg");
    image("txtSelectionScreen");
    videoButton("circle", 682, 233, "chose_pattern_circle");
    videoButton("triangle", 206, 636, "chose_pattern_triangle");
    videoButton("square", 682, 636, "chose_pattern_square");
    videoButton("freestyle", 1255, 636, "chose_pattern_freestyle");
    button("btnLanguageToggle", 1680, 972, "toggle_language");
    saveView(VIEW_CHOOSE_PATTERN);
    
    //Choose a pattern (Spanish)
    image("main_bg");
    image("txtSelectionScreen_es");
    videoButton("circle", 682, 233, "chose_pattern_circle");
    videoButton("triangle", 206, 636, "chose_pattern_triangle");
    videoButton("square", 682, 636, "chose_pattern_square");
    videoButton("freestyle", 1255, 636, "chose_pattern_freestyle");
    button("btnLanguageToggle", 1680, 972, "toggle_language");
    saveView(VIEW_CHOOSE_PATTERN_SPANISH);
    
    //Choose your music (Advanced)
    image("main_bg");
    image("txtAdvancedMusicSelection");
    button("btnSong1", 520, 300, "chose_music_fs_song1");
    button("btnSong2", 520, 480, "chose_music_fs_song2");
    button("btnSong3", 520, 660, "chose_music_fs_song3");
    button("btnSong4", 520, 840, "chose_music_fs_song4");
    button("btnStartOverSticky", 1813, 972, "start_over");
    saveView(VIEW_CHOOSE_MUSIC);
    
    //Dance View
    image("sidebar_bg");
    stateObject("txtGetReady", 0, 0);
    stateObject("countdown", 1500, 500);
    saveView(VIEW_DANCE_VIEW);
    
    //Interstitial (Great job)
    image("main_bg");
    image("txtHowDidYouDo");
    button("btnStartOverSticky", 1813, 972, "start_over");
    saveView(VIEW_GREAT_JOB);
    
    //Duel Playback
    image("main_bg");
    image("txtDidYouKeep");
    button("btnDoublePlaybackSpeed", 1225, 830, "double_playback_speed");
    button("btnStartOverSticky", 1813, 972, "start_over");
    saveView(VIEW_PLAYBACK_1);
    
    //Sped-up Playback
    image("main_bg");
    image("txtDoTheyMatch1");
    button("btnCombine", 880, 840, "combine");
    button("btnStartOverSticky", 1813, 972, "start_over");
    saveView(VIEW_PLAYBACK_2);
    
    //Combined Playback
    image("main_bg");
    image("txtDoTheyMatch2");
    button("btnStartOver", 890, 800, "start_over");
    saveView(VIEW_PLAYBACK_3);
    
    //Screensaver
    video("screensaver", 0, 0);
    saveView(VIEW_SCREENSAVER);

}

void DMLayout::setView(string name){
    
    baseViewId = name;
    string newViewId = "" + baseViewId + languageKey;
    
    //Try using language specific view,
    //fallback to language agnostic view.
    if (viewExists(newViewId) == false) {
        newViewId = "" + baseViewId;
    }
    
    ofxSimpleLayout::Layout::setView(newViewId);
    
}

void DMLayout::updateLanguage(string lang){
    
    languageKey = lang;
    
    //Refresh current view using language
    this->setView(this->baseViewId);
    
}


