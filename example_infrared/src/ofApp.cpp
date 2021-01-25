#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    kinect.init(true); // shows infrared instead of RGB video image
    kinect.open();	   // opens first available kinect

}

//--------------------------------------------------------------
void ofApp::update() {
	ofBackground(100, 100, 100);
	
	kinect.update();
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofSetColor(255, 255, 255);
	
    // draw from the live kinect
    float ratio = (float)kinect.depthHeight/(float)kinect.depthWidth;
    int imgW = 800;

    kinect.draw(10, 10, imgW, ratio*imgW);
	
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
