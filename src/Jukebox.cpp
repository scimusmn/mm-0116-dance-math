//
//  Jukebox.cpp
//  danceMath
//
//  Created by Trygve Nordberg on 4/10/15.
//
//

#include "Jukebox.h"

//---------
void Jukebox::autoAddSong(string id, int duration, int intro, int tempo){
    
    string path = "sounds/EN_" + id + "_normal.wav";
    string halfSpeedPath = "sounds/EN_" + id + "_half.wav";
    string introPath = "sounds/EN_" + id + "_intro.wav";
    
    string pathSpanish = "sounds/ES_" + id + "_normal.wav";
    string halfSpeedPathSpanish = "sounds/ES_" + id + "_half.wav";
    string introPathSpanish = "sounds/ES_" + id + "_intro.wav";
    
    this->addSong(id + "_en", path, halfSpeedPath, introPath, duration, intro, tempo);
    this->addSong(id + "_es", pathSpanish, halfSpeedPathSpanish, introPathSpanish, duration, intro, tempo);
    
}

//---------
void Jukebox::addSong(string id, string path, string halfSpeedPath, string introPath, int duration, int intro, int tempo){
    
    Song s;
    s.id = id;
    s.path = path;
    s.duration = duration;
    s.intro = intro;
    s.tempo = tempo;
    
    s.player.loadSound(path);
    s.halfSpeedPlayer.loadSound(halfSpeedPath);
    s.introPlayer.loadSound(introPath);
    
    songs.insert(pair<string, Song>(id, s));
    
}

//---------
Jukebox::Song Jukebox::getSong(string id) {
    
    auto findSong = this->songs.find(id);
    
    if (findSong != this->songs.end()) {
        return findSong->second;
    } else {
        ofLogError("Jukebox ") << "Requested song'" << id << "' not found";
    }
    
}

//---------
bool Jukebox::songExists(string id) {
    
    auto findSong = this->songs.find(id);
    
    if (findSong != this->songs.end()) {
        return true;
    } else {
        return false;
    }
    
}

//---------
void Jukebox::switchSong(string id) {
    
    //Try language specific version first,
    //fallback to language-agnostic version.
    string songId = id + languageKey + "";
    if (songExists(songId) == false) {
        songId = id;
    }
    
    ofLogNotice("switchSong", songId);
    
    Song s = getSong(songId);
    this->id = s.id;
    this->path = s.path;
    this->duration = s.duration;
    this->intro = s.intro;
    this->tempo = s.tempo;
    this->player = s.player;
    this->halfSpeedPlayer = s.halfSpeedPlayer;
    this->introPlayer = s.introPlayer;

}

//---------
void Jukebox::setLanguageKey(string key) {
    
    languageKey = key;
    
}

//---------
void Jukebox::play() {
    this->play(1);
};

void Jukebox::play(float rate) {
    
    ofSoundStopAll();
    this->rate = rate;

    //NOTE: We are no longer programmatically
    //slowing down sounds, but instead choosing
    //different files based on desired playback speed.
    
    if (rate == 0.5) {
        this->halfSpeedPlayer.play();
    } else {
        this->player.play();
    }
    
}

void Jukebox::playIntro() {
    
    ofLogNotice("playIntro", this->id);
    this->introPlayer.play();
    
}


//Sounds are not tied to any specific song.
//Should be used for bi-lingual sound effects
//and other independent sounds.
//--------
void Jukebox::loadSound(string id){
    
    string path = "sounds/EN_" + id + ".wav";
    string pathSpanish = "sounds/ES_" + id + ".wav";
    
    ofSoundPlayer player;
    ofSoundPlayer playerSpanish;
    
    player.loadSound(path);
    playerSpanish.loadSound(pathSpanish);
    
    sounds.insert(pair<string, ofSoundPlayer>(id+"_en", player));
    sounds.insert(pair<string, ofSoundPlayer>(id+"_es", playerSpanish));
    
}

//---------
void Jukebox::playSound(string id) {
    
    //Try language specific version first
    string sndId = id + languageKey + "";
    
    auto findSound = this->sounds.find(sndId);
    
    if (findSound != this->sounds.end()) {
        
        findSound->second.play();
        
    } else {
        
        //Fallback to language-agnostic version
        findSound = this->sounds.find(id);
        if (findSound != this->sounds.end()) {
            
            findSound->second.play();
            
        } else {
            
            ofLogError("Jukebox ") << "Neither sound '" << id << "' or '" << sndId << "' was found.";
            
        }

    }

}

