# mm-0116-dance-math
What does half-speed feel like? Record your movements at different speeds to  learn about ratios.

## Installation
#### 1. Copy project from SMM server.
* Copy this folder to your Desktop: */Projects/Math Moves-SMM/Multimedia/0116 Dance Math/04_Programming/01_latest/danceMath*   

#### 2. Install [ffmpeg](https://www.ffmpeg.org/).
1. Copy into your $PATH directory: ``` $ cp ~/Desktop/danceMath/ffmpeg /usr/local/bin/ ```
2. Install by running: ```$ /usr/local/bin/ffmpeg```

#### 3. Set up app to launch automatically.
1. Drag *danceMath.app* icon into dock.
2. Right-click icon, then select *Options > Open at Login*.


## Editing & Compiling (Advanced)
#### 1. Download openFrameworks
* [http://openframeworks.cc/download/](http://openframeworks.cc/download/)
* This project was built using v0.8.4.  If you come from the future, check [here.](http://openframeworks.cc/download/older.html)

#### 2. Clone this repository into *myApps* folder
* Your folder structure will look like this:
```
OpenFrameworks/apps/myApps/mm-0116-dance-math/...
```

#### 3. Open Xcode project
1. Open *danceMath.xcodeproj* in Xcode.
2. Ensure the "Scheme" (right of play/stop buttons) is set to "danceMath Release".
3. Click the Play button to build and run project.
