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
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);  

	
    ofxLibfreenect2 kinect;

};
