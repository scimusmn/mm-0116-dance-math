//
//  Jukebox.h
//  danceMath
//
//  Created by Trygve Nordberg on 4/10/15.
//
//
#include "ofSoundPlayer.h"
#include <map>

class Jukebox {
    
public:
    
    class Song {
    public:
        string id;
        string path;
        int duration;
        int intro;
        int tempo;
        ofSoundPlayer player;
    };
    
    void addSong(string id, string path, int duration, int intro, int tempo);
    void switchSong(string id);
    Song getSong(string id);
    
    void play();
    void play(float rate);
    float rate;
    
    map<string, Song> songs;
    
    //These match whatever current song is set by switchSong().
    string id;
    string path;
    int duration;
    int intro;
    int tempo;
    ofSoundPlayer player;
    
    
protected:
    
    
};
