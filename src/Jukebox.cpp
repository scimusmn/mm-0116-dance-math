//
//  Jukebox.cpp
//  danceMath
//
//  Created by Trygve Nordberg on 4/10/15.
//
//

#include "Jukebox.h"

//---------
void Jukebox::autoAddTrack(string id, int normPre, int normDur, int halfPre, int halfDur){
    
    string path = "video/record_" + id + "_en.mov";
    string halfSpeedPath = "video/record_" + id + "_half_en.mov";
    string introPath = "video/record_" + id + "_intro_en.mov";
    
    string pathSpanish = "video/record_" + id + "_es.mov";
    string halfSpeedPathSpanish = "video/record_" + id + "_half_es.mov";
    string introPathSpanish = "video/record_" + id + "_intro_es.mov";
    
    this->addTrack(id + "_en", path, halfSpeedPath, introPath, normPre, normDur, halfPre, halfDur);
    this->addTrack(id + "_es", pathSpanish, halfSpeedPathSpanish, introPathSpanish, normPre, normDur, halfPre, halfDur);
    
}

//---------
void Jukebox::addTrack(string id, string path, string halfSpeedPath, string introPath, int normPre, int normDur, int halfPre, int halfDur){
    
    Track s;
    s.id = id;
    s.path = path;
    s.normPreRecordDuration = normPre;
    s.normRecordDuration = normDur;
    s.halfPreRecordDuration = halfPre;
    s.halfRecordDuration = halfDur;
    
    s.player.setPixelFormat(OF_PIXELS_RGBA); // Allow alpha channel
    s.player.loadMovie(path);

    tracks.insert(pair<string, Track>(id, s));
    ofLogNotice("addTrack: ", id);
}

//---------
Jukebox::Track Jukebox::getTrack(string id) {
    
    auto findTrack = this->tracks.find(id);
    
    if (findTrack != this->tracks.end()) {
        return findTrack->second;
    } else {
        ofLogError("Jukebox ") << "Requested track'" << id << "' not found";
    }
    
}

//---------
bool Jukebox::trackExists(string id) {
    
    auto findTrack = this->tracks.find(id);
    
    if (findTrack != this->tracks.end()) {
        return true;
    } else {
        return false;
    }
    
}

//---------
void Jukebox::switchTrack(string id) {
    
    //Try language specific version first,
    //fallback to language-agnostic version.
    string trackId = id + languageKey + "";
    
    ofLogNotice("switchTrack", trackId);
    
    if (trackExists(trackId) == false) {
        trackId = id;
        ofLogNotice("switchTrack fallback: ", trackId);
    }
    
    
    Track s = getTrack(trackId);
    this->id = s.id;
    this->path = s.path;
    this->normPreRecordDuration = s.normPreRecordDuration;
    this->normRecordDuration = s.normRecordDuration;
    this->halfPreRecordDuration = s.halfPreRecordDuration;
    this->halfRecordDuration = s.halfRecordDuration;
    this->player = s.player;

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
    this->player.play();
    
}


//Sounds are not tied to any specific track.
//Should be used for bi-lingual sound effects
//and other independent sounds.
//--------
void Jukebox::loadSound(string id){
    
    string path = "audio/" + id + ".wav";
    string pathSpanish = "audio/" + id + ".wav";
    
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

