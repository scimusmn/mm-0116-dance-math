//
//  ofxSimpleLayout.cpp
//

#include "ofxSimpleLayout.h"

namespace ofxSimpleLayout {
#pragma mark public

    //----------
    void Layout::saveView(string name){
        
        //Save registered elements to a new view
        this->views.insert(pair<string, vector<LayoutElement> >(name, this->currentView));
        this->currentView.clear();
        
    }
    
    //----------
    void Layout::setView(string name){
        
        this->toggleVideos(false);
        
        //Set visible view
        auto findView = this->views.find(name);
        if (findView != this->views.end()) {
            this->currentView = findView->second;
            currentViewId = name;
        } else {
            ofLogError("ofxSimpleLayout") << "Requested view'" << name << "' not found";
        }
        
        this->toggleVideos(true);

    }
    
    //----------
    bool Layout::viewExists(string name){

        auto findView = this->views.find(name);
        if (findView != this->views.end()) {
            return true;
        } else {
            return false;
        }
        
    }
    
    //----------
    void Layout::image(string name){ this->image(name,0,0); };
    void Layout::image(string name, int x, int y){
        
        this->registerImage(name);
        this->addLayoutElement(name, TYPE_IMG, ofRectangle(x, y, getImage(name).getWidth(), getImage(name).getHeight()));
        
    }
    
    //----------
    void Layout::button(string name, int x, int y){
        this->button(name, x, y, name);
    }
    void Layout::button(string name, int x, int y, string msg){
        
        this->registerImage(name);
        this->addLayoutElement(name, TYPE_IMG, ofRectangle(x, y, getImage(name).getWidth(), getImage(name).getHeight()));
        
        //Set button message
        this->currentView.back().msg = msg;
        
    }
    
    //----------
    void Layout::video(string name, int x, int y){
        this->video(name, x, y, true);
    }
    
    //----------
    void Layout::video(string name, int x, int y, bool loop){
        
        this->registerVideo(name, loop);
        this->addLayoutElement(name, TYPE_VID, ofRectangle(x, y, getVideo(name).getWidth(), getVideo(name).getHeight()));
        
    }
    
    //----------
    void Layout::videoButton(string name, int x, int y, string msg){
        this->videoButton(name, x, y, msg, true);
    }
    
    //----------
    void Layout::videoButton(string name, int x, int y, string msg, bool loop){
        
        this->registerVideo(name, loop);
        this->addLayoutElement(name, TYPE_VID, ofRectangle(x, y, getVideo(name).getWidth(), getVideo(name).getHeight()));
        
        //Set button message
        this->currentView.back().msg = msg;
        
    }
    
    //----------
    void Layout::stateObject(string name, int x, int y){
        
        //Find and load state images
        //Naming convention: "stateObj_0.png", "stateObj_1.png", "stateObj_2.png" ... "stateObj_15.png"
        int stateCount = 0;
        bool foundImg = true;
        while (foundImg == true){
            foundImg = this->registerImage(name + "_" + ofToString(stateCount));
            stateCount++;
        }
        
        this->addLayoutElement(name, TYPE_STATE, ofRectangle(x, y, getImage(name + "_0").getWidth(), getImage(name + "_0").getHeight()));
        
        //Default to 0 state
        this->currentView.back().state = "0";
        
    }
    
    //----------
    void Layout::addLayoutElement(string name, AssetType type, ofRectangle box){
        
        LayoutElement le;
        le.name = name;
        le.type = type;
        le.box = box;
        this->currentView.push_back(le);
        
    }
    
    //----------
    void Layout::update(){
        
        for (int i = 0; i<this->currentView.size(); i++) {
            if (this->currentView[i].type == TYPE_VID){
                getVideo(this->currentView[i].name).update();
            };
        }
        
    }
    
    //----------
    void Layout::draw(){

        for (int i = 0; i<currentView.size(); i++) {

            LayoutElement & le = currentView[i];
            
            switch(le.type){
                case TYPE_IMG:
                    getImage(le.name).draw(le.box.x, le.box.y);
                    break;
                case TYPE_STATE:
                    getImage(le.name+"_"+le.state).draw(le.box.x, le.box.y);
                    break;
                case TYPE_VID:
                    getVideo(le.name).draw(le.box.x, le.box.y, le.box.width, le.box.height);
                    break;
                default:
                    ofLogError("ofxSimpleLayout") << "AssetType '" << le.type << "' not recogized.";
                    break;
            }
            
        }
        
    }
    
    //----------
    LayoutElement& Layout::getByName(string name) {
        
        for (int i = 0; i<currentView.size(); i++) {
            
            if (currentView[i].name == name){
                
                return currentView[i];
                
            }
            
        }
        
    }
    
    //----------
    void Layout::setState(string name, string state) {

        this->getByName(name).state = state;
        
    }

    //----------
    string Layout::getHovered(int x, int y) {
        
        return this->getSelected(x, y);
        
    }
    
    //----------
    string Layout::getSelected(int x, int y) {
        
        //Check buttons in current view to see if any are under mouse
        LayoutElement le;
        for (int i = 0; i<currentView.size(); i++) {
            le = currentView[i];
            if (!le.msg.empty()) {
                if (le.box.inside(x, y) == true){
                    if (!le.msg.empty()) {
                        return le.msg;
                    }else{
                       return le.name;
                    }
                }
            }
        }
        
        return "";
        
    }

    
#pragma mark protected
    
    //---------
    bool Layout::registerImage(string name) {
        
        bool hasImage = this->images.find(name) != this->images.end();
        if (hasImage == true) return true;
       
        vector<string> imgExtensions;
        imgExtensions.push_back("png");
        imgExtensions.push_back("jpg");
        imgExtensions.push_back("jpeg");
        string filepath = this->findFile(name, imgExtensions, "");
        
        if (filepath == "") return false;
        
        this->images.insert(pair<string, ofImage>(name, ofImage()));
        this->images[name].loadImage(filepath);
                
//        ofLogNotice("ofxSimpleLayout") << "Registered image: '" << filepath << "'" << endl;
        
        return true;
        
    }

    //---------
    ofImage Layout::getImage(string name) {
        
        auto findImage = this->images.find(name);
        if (findImage != this->images.end()) {
            return findImage->second;
        } else {
            return  this->blankImage;
        }
    }
    
    //---------
    bool Layout::registerVideo(string name, bool loop) {
        
        bool hasVideo = this->videos.find(name) != this->videos.end();
        if (hasVideo == true) return true;
        
        vector<string> vidExtensions;
        vidExtensions.push_back("mov");
        vidExtensions.push_back("gif");
        string filepath = this->findFile(name, vidExtensions, "");
        
        if (filepath == "") return false;
        
        this->videos.insert(pair<string, ofVideoPlayer>(name, ofVideoPlayer()));
        this->videos[name].setPixelFormat(OF_PIXELS_RGBA); // Allow alpha channel
        this->videos[name].loadMovie(filepath);
        
        if (!loop) this->videos[name].setLoopState(OF_LOOP_NONE);
        
//        ofLogNotice("ofxSimpleLayout") << "Registered video: '" << filepath << "'" << endl;
        
        return true;
    }
    
    //---------
    ofVideoPlayer Layout::getVideo(string name) {
        
        auto findVideo = this->videos.find(name);
        if (findVideo != this->videos.end()) {
            return findVideo->second;
        } else {
            return  this->blankVideo;
        }
    }
    
    //---------
    void Layout::toggleVideos(bool play) {
        
        for (int i = 0; i<this->currentView.size(); i++) {
            if (this->currentView[i].type == TYPE_VID){
                if (play){
                    this->getVideo(this->currentView[i].name).play();
                }else{
                    this->getVideo(this->currentView[i].name).stop();
                }
            };
        }

    }

    //----------
    string Layout::findFile(string name, vector<string> requiredExtensions, string dataPath) {
        
        if (ofDirectory::doesDirectoryExist(dataPath)) {
            ofDirectory files;
            files.listDir(dataPath);
            for (int i = 0; i<files.size(); i++) {
                const auto filename = files.getPath(i);
                auto foundName = ofFilePath::getBaseName(filename);
                
                //check if subfolder. if so, look inside.
                if (ofDirectory(filename).isDirectory()) {
                    if (dataPath.empty()==false) dataPath = dataPath + "/";
                    string n =  this->findFile(name, requiredExtensions, dataPath + foundName);
                    if (!n.empty()) return n;
                    continue;
                }
                
                //if not, check for name match
                if (foundName != name){
                    continue;
                }
                
                //check whether it has the right extension
                const auto extension = ofFilePath::getFileExt(filename);
                if(std::find(requiredExtensions.begin(), requiredExtensions.end(), extension) == requiredExtensions.end()) {
                    continue;
                }

                //insert and load the image
                if (this->images.find(foundName) != this->images.end()) {
                    continue;
                }

                return filename;
                
            }
        }
        
        return "";
        
    }

    
}