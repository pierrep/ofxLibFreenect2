#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    kinect.init(false,false);      // set up depth only, no RGB and IR streams, you should see a performance boost
    kinect.open();                 // opens first available kinect

}

//--------------------------------------------------------------
void ofApp::update() {

    ofSetWindowTitle("FPS: "+ofToString(ofGetFrameRate()));
	ofBackground(100, 100, 100);
	
	kinect.update();

}

//--------------------------------------------------------------
void ofApp::draw() {

	ofSetColor(255, 255, 255);
	
    // draw depth image
    float w = kinect.depthWidth;
    float h = kinect.depthHeight;
    float ratio = h/w;
    int imgW = 800;

    kinect.drawDepth(10, 10, imgW, ratio*imgW);

    ofDrawBitmapString("Depth image width: "+ofToString(w)+" height: "+ofToString(h),10,ofGetHeight()-30);
		
}

//--------------------------------------------------------------
void ofApp::exit() {

	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}
