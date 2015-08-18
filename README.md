# mm-0116-dance-math
What does half-speed feel like? Record your movements at different speeds to kinesthetically learn about ratios.

## Installation
1. Copy contents from SMM project server: */Projects/Math Moves-SMM/Multimedia/0116 Dance Math/04_Programming/01_latest/danceMath/*
2. Ensure [ffmpeg](https://www.ffmpeg.org/) framework is intalled on machine. See instructions in next section.
3. Drag *danceMath.app* icon into dock.
4. Right-click *danceMath* application in dock, then select Options > Open at Login.

## Installing ffmpeg
This application utilizes the [ffmpeg](https://www.ffmpeg.org/) video framework. Below is the simplest way to install. 
1. Go to [http://ffmpegmac.net/](http://ffmpegmac.net/) and download the 64 bit binary.
2. Unzip, then copy the *ffmpeg* file into your $PATH directory. (typically *usr/local/bin/*)
3. In Terminal, navigate to the same directory: ``` $ cd /usr/local/bin ```
4. Install the ffmpeg library: ```$ ./ffmpeg```


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
