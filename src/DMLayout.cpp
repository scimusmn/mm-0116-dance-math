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
//TODO: add all permanent screens as constants

void DMLayout::setupViews(){

    //Pick a Dance (currently not in use)
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
    button("btnSong1", 250, 205, "chose_music_jazz");
    button("btnSong2", 580, 205, "chose_music_clicks");
    button("btnSong3", 250, 605, "chose_music_waltz");
    button("btnSong4", 580, 605, "chose_music_tango");
    saveView(VIEW_CHOOSE_MUSIC);
    
    //Dance View
    image("sidebar_bg");
    stateObject("txtGetReady", 0, 0);
    stateObject("countdown", 1500, 500);
    saveView(VIEW_DANCE_VIEW);
    
    //Normal Playback
    image("main_bg");
    image("txtHowDidYouDo");
    button("btnRecordHalfSpeed", 890, 760, "start_dance_0.5");
    saveView("playback_1");
    
    //Duel Playback
    image("main_bg");
    image("txtDidYouKeep");
    button("btnDoublePlaybackSpeed", 1200, 830, "double_playback_speed");
    saveView("playback_2");
    
    //Sped-up Playback
    image("main_bg");
    image("txtDidYouKeep");
    button("btnCombine", 890, 770, "combine");
    saveView("playback_3");
    
    //Combined Playback
    image("main_bg");
    image("txtDoTheyMatch");
    button("btnStartOver", 890, 800, "start_over");
    saveView("playback_4");
    
    //Playback View
    image("sidebar_bg");
    image("txtPlayback");
    button("btnRecordNow", 1490, 180, "start_dance_1");
    button("btnRecordNow", 1490, 421, "start_dance_0.5");
//    button("btnRecordNow", 1490, 662, "start_dance_2");
    button("invisiButton", 1490, 662, "start_dance_2");
    button("btnStartOver", 1600, 915, "start_over");
    button("btnTogglePlayback", 1200, 1000, "toggle_playback");
    saveView(VIEW_PLAYBACK);

}


