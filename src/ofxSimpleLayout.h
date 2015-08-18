//
//  ofxSimpleLayout.h
//
#pragma once

#include "ofImage.h"
#include "ofGraphics.h"
#include "ofVideoPlayer.h"

#include <map>
#include <set>
#include <string>

namespace ofxSimpleLayout {
    
    enum AssetType{
        TYPE_IMG,
        TYPE_STATE,
        TYPE_VID
    };
    
    class LayoutElement {

    public:
        
        string name;
        string msg;
        string state;
        AssetType type;
        ofRectangle box;
                
    protected:
        
    };

    class Layout {
    public:
        void saveView(string name);
        void setView(string name);
        bool viewExists(string name);
        string currentViewId;
        
        //Add elements
        void image(string name);
        void image(string name, int x, int y);
        void stateObject(string name, int x, int y);
        void button(string name, int x, int y);
        void button(string name, int x, int y, string msg);
        void video(string name, int x, int y);
        void video(string name, int x, int y, bool loop);
        void videoButton(string name, int x, int y, string msg);
        void videoButton(string name, int x, int y, string msg, bool loop);
        
        void setState(string name, string state);
        
        LayoutElement& getByName(string name);
        string getHovered(int x, int y);
        string getSelected(int x, int y);

        void toggleVideos(bool play);

        void update();
        void draw();
        
    protected:
        
        map<string, vector<LayoutElement> > views;
        vector<LayoutElement> currentView;
        
        map<string, ofImage> images;
        map<string, ofVideoPlayer> videos;
        
        ofImage getImage(string name);
        ofVideoPlayer getVideo(string name);
        
        void addLayoutElement(string name, AssetType type, ofRectangle box);
        bool registerImage(string name);
        bool registerVideo(string name, bool loop);
        
        ofImage blankImage;
        ofVideoPlayer blankVideo;
        
        string findFile(string name, vector<string> requiredExtensions, string dataPath);

    };


}