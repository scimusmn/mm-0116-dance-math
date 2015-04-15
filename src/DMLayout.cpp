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
    button("btnPreview", 250, 150, "preview_music_jazz");
    button("btnSong1", 250, 275, "chose_music_jazz");
    button("btnPreview", 550, 150, "preview_music_calliope");
    button("btnSong2", 550, 275, "chose_music_calliope");
    button("btnPreview", 250, 480, "preview_music_waltz");
    button("btnSong3", 250, 605, "chose_music_waltz");
    button("btnPreview", 550, 480, "preview_music_tango");
    button("btnSong4", 550, 605, "chose_music_tango");
    saveView(VIEW_CHOOSE_MUSIC);
    
    //Dance View
    image("sidebar_bg");
    image("txtGetReady");
    stateObject("countdown", 1500, 500);
    saveView(VIEW_DANCE_VIEW);
    
    //Playback View
    image("sidebar_bg");
    image("txtPlayback");
    button("btnHalfSpeed", 1190, 515, "start_dance_0.5");
    button("btnNormalSpeed", 1440, 515, "start_dance_1");
    button("btnDoubleSpeed", 1690, 515, "start_dance_2");
    button("btnStartOver", 1440, 750, "start_over");
//    button("btnOverlayTracking", 1440, 50, "toggle_overlay");
//    button("btnShowAll", 1400, 50, "show_all");
    saveView(VIEW_PLAYBACK);

}


