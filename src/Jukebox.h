//
//  Jukebox.h
//  danceMath
//
//  Created by Trygve Nordberg on 4/10/15.
//
//
#include "ofSoundPlayer.h"
#include "ofVideoPlayer.h"
#include <map>

class Jukebox {
    
public:
    
    class Track {
    public:
        string id;
        string path;
        int normPreRecordDuration;
        int normRecordDuration;
        int halfPreRecordDuration;
        int halfRecordDuration;
        ofVideoPlayer player;
    };
    
    void autoAddTrack(string id, int normPre, int normDur, int halfPre, int halfDur);
    void addTrack(string id, string path, string halfSpeedPath, string introPath, int normPre, int normDur, int halfPre, int halfDur);
    void switchTrack(string id);
    Track getTrack(string id);
    bool trackExists(string id);

    void play();
    void play(float rate);
    void playIntro();
    float rate;
    map<string, Track> tracks;
    
    void loadSound(string id);
    void playSound(string id);
    map<string, ofSoundPlayer> sounds;
    
    void setLanguageKey(string key);
    string languageKey = "";
    
    //These match whatever current track is set by switchTrack().
    string id;
    string path;
    int normPreRecordDuration;
    int normRecordDuration;
    int halfPreRecordDuration;
    int halfRecordDuration;
    ofVideoPlayer player;
    
protected:
    
    
};
