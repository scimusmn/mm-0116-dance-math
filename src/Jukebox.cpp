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

    string path = "video/record_" + id + ".mov";
    this->addTrack(id, path, normPre, normDur, halfPre, halfDur);

}

//---------
void Jukebox::addTrack(string id, string path, int normPre, int normDur, int halfPre, int halfDur){

    Track s;
    s.id = id;
    s.path = path;
    s.normPreRecordDuration = normPre;
    s.normRecordDuration = normDur;
    s.halfPreRecordDuration = halfPre;
    s.halfRecordDuration = halfDur;

    //NOTE: Currently loading ALL "guide" videos up front,
    //which is going to eat a lot of RAM. If we run into memory issues,
    //we should consider loading on the fly when the track is changed. -tn

    s.player.setPixelFormat(OF_PIXELS_RGBA); // Allow alpha channel
//    s.player.loadMovie(path);
    s.player.setLoopState(OF_LOOP_NONE);

    tracks.insert(pair<string, Track>(id, s));

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
void Jukebox::playFromStart() {

    if (trackLoaded == true){
        this->current.player.firstFrame();
        this->current.player.play();
        this->current.player.setLoopState(OF_LOOP_NONE);
    }else {
        ofLogError("playFromStart(): No track loaded.");
    }

}

//---------
void Jukebox::switchTrack(string id) {
    
    //Unload video from previous track
    if (current.player.isLoaded() == true){
        current.player.close();
        ofLogNotice("Unloading track: ", current.path);
        ofSleepMillis(40);
    }

    //Try language specific version first,
    //fallback to language-agnostic version.
    string trackId = id + languageKey + "";

    ofLogNotice("switchTrack", trackId);

    if (trackExists(trackId) == false) {
        trackId = id;
        ofLogNotice("switchTrack fallback: ", trackId);
    }
    
    current = getTrack(trackId);
    current.player.loadMovie(current.path);
    ofLogNotice("Loading track: ", current.path);
    trackLoaded = true;

}

//---------
void Jukebox::clearTrack() {

    this->current.player.stop();
    trackLoaded = false;
    
}

//---------
void Jukebox::setLanguageKey(string key) {

    languageKey = key;

}

//Sounds are not tied to any specific track.
//Should be used for bi-lingual sound effects
//and other independent sounds.
//--------
void Jukebox::loadSound(string id){

    string path = "audio/" + id + ".wav";
    ofSoundPlayer player;
    player.loadSound(path);
    
    sounds.insert(pair<string, ofSoundPlayer>(id, player));

}

//---------
void Jukebox::playSound(string id) {

    //Try language specific version first
    string sndId = id + languageKey + "";

    auto findSound = this->sounds.find(sndId);

    if (findSound != this->sounds.end()) {
        
        ofLogNotice("Jukebox ") << "Playing language("<< languageKey << ") sound: '" << sndId << "'. '" ;
        findSound->second.play();

    } else {

        //Fallback to language-agnostic version
        findSound = this->sounds.find(id);
        if (findSound != this->sounds.end()) {
            
            ofLogNotice("Jukebox ") << "Playing non-language sound: '" << id << "'. '" ;
            findSound->second.play();

        } else {

            ofLogError("Jukebox ") << "Neither sound '" << id << "' or '" << sndId << "' was found.";

        }

    }

}

