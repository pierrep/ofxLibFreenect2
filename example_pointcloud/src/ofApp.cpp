#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetVerticalSync(false);

    kinect.setRegistration(true); // enable depth->video image calibration
    
    kinect.init();
    kinect.open();

    mesh.setMode(OF_PRIMITIVE_POINTS);
    distanceThreshold = 1.5f;
    bUseColour = true;
    pointSize = 2;
}

//--------------------------------------------------------------
void ofApp::update() {
    ofSetWindowTitle("Fps: "+ofToString(ofGetFrameRate()));
	ofBackground(100, 100, 100);
	
	kinect.update();	

    updateAverageFps();
}

//--------------------------------------------------------------
void ofApp::draw() {
	
    easyCam.begin();
    int w = kinect.getDepthPixels().getWidth();
    int h = kinect.getDepthPixels().getHeight();
    mesh.clear();
    int step = pointSize;
    for(int y = 0; y < h; y += step) {
        for(int x = 0; x < w; x += step) {
            if(kinect.getDistanceAt(x, y) < 0) {
               ofLogNotice() << "distance of point < 0";
            }
            if(kinect.getDistanceAt(x, y) < distanceThreshold) //only draw points a certain distance away.
            {
                if(bUseColour) {
                    mesh.addColor(kinect.getColorAt(x,y));
                } else {
                    mesh.addColor(ofColor(255));
                }
                mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
            }
        }
    }

    ofPushMatrix();
    ofScale(1000, -1000, -1000); // the projected points are 'upside down' and 'backwards'
    glPointSize(pointSize);
    mesh.draw();
    ofPopMatrix();
    easyCam.end();

    ofDrawBitmapString("Spacebar toggles colour, +/- changes the distance threshold, < > changes the point size, mouse controls the camera",10,ofGetHeight()-90);
    ofDrawBitmapString("Distance threshold: "+ofToString(distanceThreshold)+ " Point size: "+ofToString(pointSize),10,ofGetHeight()-60);
    ofDrawBitmapString("Average FPS: "+ofToString(avgFps),10,ofGetHeight()-30);

}

//--------------------------------------------------------------
void ofApp::exit() {
	kinect.close();
}

//--------------------------------------------------------------
void ofApp::updateAverageFps() {
    framesThisSecond++;
    if(ofGetElapsedTimeMillis() - curTime > 1000.0f) {
        float alpha = 0.85f;
        avgFps = alpha * avgFps + (1.0 - alpha) * framesThisSecond;
        framesThisSecond = 0;
        curTime = ofGetElapsedTimeMillis();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
    switch (key) {
        case ' ':
            bUseColour = !bUseColour;
            break;
        case '>':
        case '.':
            pointSize++;
            if (pointSize > 50) pointSize = 50;
            break;
        case '<':
        case ',':
            pointSize--;
            if (pointSize < 1) pointSize = 1;
            break;
        case '+':
        case '=':
            distanceThreshold += 0.01f;
            if (distanceThreshold > 4.5f) distanceThreshold = 4.5f;
            break;

        case '-':
            distanceThreshold -= 0.01f;
            if (distanceThreshold < 0) distanceThreshold = 0;
            break;
    }
}

