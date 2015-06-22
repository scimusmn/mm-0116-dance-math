//
//  DMLayout.h
//  danceMath
//
//  Created by Trygve Nordberg on 3/27/15.
//
//

#include "ofxSimpleLayout.h"
#include "ofxTween.h"

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
    
    void draw();
    
    void setupTweens(string viewId);
    ofxTween multitween;
    ofxEasingBack 	easingback;
    ofxEasingBounce easingbounce;
    ofxEasingCirc 	easingcirc;
    ofxEasingCubic 	easingcubic;
    ofxEasingElastic easingelastic;
    ofxEasingExpo 	easingexpo;
    ofxEasingLinear easinglinear;
    ofxEasingQuad 	easingquad;
    ofxEasingQuart 	easingquart;
    ofxEasingQuint 	easingquint;
    ofxEasingSine	easingsine;
    
protected:
    
    
};
