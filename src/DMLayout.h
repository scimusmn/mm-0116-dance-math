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
    
    static string VIEW_PICK_DANCE;
    static string VIEW_CHOOSE_MUSIC;
    static string VIEW_DANCE_VIEW;
    
    void setupViews();
    void update();
    void startCountdown();
    
protected:
    int countdownStartTime;
    
};
