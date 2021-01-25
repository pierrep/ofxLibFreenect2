#pragma once

#include "ofMain.h"
#include "ofxLibfreenect2.h"

class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
    void updateAverageFps();

    ofxLibfreenect2 kinect;
	
	// used for viewing the point cloud
	ofEasyCam easyCam;
    ofMesh mesh;
    float distanceThreshold;

    //Visuals
    bool bUseColour;
    int pointSize;

    //Timing
    int framesThisSecond;
    float curTime;
    float avgFps;

};
