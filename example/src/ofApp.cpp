#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    //ofSetLogLevel(OF_LOG_VERBOSE);

    kinect.listDevices();
    kinect.init();                  // set up depth and RGB image streams
    //kinect.open();                // opens first available kinect
    kinect.open(0);                 // open a kinect by id, starting with 0
    //kinect.open("041081740747");	// open a kinect using it's unique serial number

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
	
    // draw colour and depth images
    float ratio = (float)kinect.getHeight()/(float)kinect.getWidth();
    int imgW = 450;

    kinect.draw(10, 10, imgW, ratio*imgW);
    
    ratio = (float)kinect.getDepthHeight()/(float)kinect.getDepthWidth();
    kinect.drawDepth(imgW+20, 10, imgW, ratio*imgW);
    
	ofDrawBitmapString("Colour image width: "+ofToString(kinect.getWidth())+" height: "+ofToString(kinect.getHeight()),10,ofGetHeight()-60);
	ofDrawBitmapString("Depth image width: "+ofToString(kinect.getDepthWidth())+" height: "+ofToString(kinect.getDepthHeight()),10,ofGetHeight()-40);
    string isOn = kinect.isConnected()?"yes":"no";
    ofDrawBitmapString("kinect connected: " +isOn,10,ofGetHeight()-20);
}

//--------------------------------------------------------------
void ofApp::exit() {

	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
    if(key == ' ') {
        if(kinect.isInitialized()) {
            if(kinect.isConnected()) {
                ofLogNotice() << "Closing Kinect...";
                kinect.close();
            } else {
                kinect.open();
            }
        }
    }
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

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}
