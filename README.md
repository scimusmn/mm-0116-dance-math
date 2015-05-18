# mm-0116-dance-math
Track your dance moves at different speeds to learn about ratios.


## Required Hardware
####
1. 1920 x 1080 display
2. 720p USB webcam

##To Run
1. Download into desired directory
2. Run app file located at mm-0116-dance-math/bin/danceMath.app

##To Edit or Build Source

#### 1. Download openFrameworks
* [Download link](http://openframeworks.cc/download/)
* This was built using v0.8.4, which was latest release as of May 12, 2015

#### 2. Clone Dance Math repo into the myApps folder
* Should look like: <openFrameworks>/apps/myApps/mm-0116-dance-math/

#### 3. Get ofxSimpleLayout addon
* Download [ofxSimpleLayout](https://github.com/scimusmn/ofxSimpleLayout.git) into <OpenFrameworks>/addons/ directory.

#### 3. Setup Xcode project
1. Return to the danceMath.xcodeproj, and open in Xcode.
2. Ensure the "Scheme" (next to play/stop buttons) is set to danceMath Release (not "openframeworks").
3. Click the Play button to build and run project.
4. If you encounter errors, you may need to re-link the addons. [Follow instructions here.](http://openframeworks.cc/setup/xcode/)

