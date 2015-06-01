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
        ofSoundPlayer halfSpeedPlayer;
        ofSoundPlayer introPlayer;
    };
    
    void autoAddSong(string id, int duration, int intro, int tempo);
    void addSong(string id, string path, string halfSpeedPath, string introPath, int duration, int intro, int tempo);
    void switchSong(string id);
    Song getSong(string id);
    bool songExists(string id);

    void play();
    void play(float rate);
    void playIntro();
    float rate;
    map<string, Song> songs;
    
    void loadSound(string id);
    void playSound(string id);
    map<string, ofSoundPlayer> sounds;
    
    void setLanguageKey(string key);
    string languageKey = "";
    
    //These match whatever current song is set by switchSong().
    string id;
    string path;
    int duration;
    int intro;
    int tempo;
    ofSoundPlayer player;
    ofSoundPlayer halfSpeedPlayer;
    ofSoundPlayer introPlayer;
    
protected:
    
    
};
