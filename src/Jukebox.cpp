//
//  Jukebox.cpp
//  danceMath
//
//  Created by Trygve Nordberg on 4/10/15.
//
//

#include "Jukebox.h"

//---------
void Jukebox::addSong(string id, string path, string introPath, int duration, int intro, int tempo){
    
    Song s;
    s.id = id;
    s.path = path;
    s.duration = duration;
    s.intro = intro;
    s.tempo = tempo;
    
    s.player.loadSound(path);
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
void Jukebox::switchSong(string id) {
    
    Song s = getSong(id);
    this->id = s.id;
    this->path = s.path;
    this->duration = s.duration;
    this->intro = s.intro;
    this->tempo = s.tempo;
    this->player = s.player;
    this->introPlayer = s.introPlayer;

}

//---------
void Jukebox::play() {
    this->play(1);
};

void Jukebox::play(float rate) {
    
    ofSoundStopAll();
    this->rate = rate;
    this->player.setSpeed(rate);
    this->player.play();
    
}

void Jukebox::playIntro() {
    
    this->introPlayer.play();
    
}
