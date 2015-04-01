//
//  DMLayout.cpp
//  danceMath
//
//  Created by Trygve Nordberg on 3/27/15.
//
//

#include "DMLayout.h"

void DMLayout::setupViews(){
    
//    button("button", 760, 0, "dance_slow");
//    button("button", 760, 80, "dance_normal");
//    button("button", 760, 160, "dance_fast");
    
    //Pick a Dance
    image("main_bg");
    image("sidebar_bg");
    image("txtPickADance");
    videoButton("chooseDanceA", 50, 50, "chose_dance_a");
    videoButton("chooseDanceB", 550, 50, "chose_dance_b");
    videoButton("chooseDanceC", 50, 450, "chose_dance_c");
    videoButton("chooseDanceD", 550, 450, "chose_dance_d");
    saveView("pick_a_dance");
    
    //Choose your music
    image("main_bg");
    image("sidebar_bg");
    image("txtChooseYourMusic");
    button("button", 250, 150, "preview_music_groove");
    button("button", 250, 250, "chose_music_groove");
    saveView("choose_music");
    
    
    
    //Default View
    setView("choose_music");
    
}