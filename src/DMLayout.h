//
//  DMLayout.h
//  danceMath
//
//  Created by Trygve Nordberg on 3/27/15.
//
//

#include "ofxSimpleLayout.h"

class DMLayout : public ofxSimpleLayout::Layout{

public:
    
    static string VIEW_SELECT;
    static string VIEW_SELECT_TRACK;
    static string VIEW_RECORD;
    static string VIEW_PLAYBACK_1;
    static string VIEW_PLAYBACK_2;
    static string VIEW_PLAYBACK_3;
    static string VIEW_SCREENSAVER;

    void setupViews();
    void setView(string name);
    
    void updateLanguage(string lang);
    string languageKey;
    string baseViewId;
    
protected:
    
    
};
