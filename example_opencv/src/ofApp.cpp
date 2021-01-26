#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    kinect.init();	
	kinect.open();		// opens first available kinect
	
    colorImg.allocate(kinect.getDepthWidth(), kinect.getDepthHeight());
    grayImage.allocate(kinect.getDepthWidth(), kinect.getDepthHeight());
    grayThreshNear.allocate(kinect.getDepthWidth(), kinect.getDepthHeight());
    grayThreshFar.allocate(kinect.getDepthWidth(), kinect.getDepthHeight());
	
	nearThreshold = 230;
	farThreshold = 70;
	bThreshWithOpenCV = true;	

}

//--------------------------------------------------------------
void ofApp::update() {
    ofSetWindowTitle("Fps: "+ofToString(ofGetFrameRate()));
	ofBackground(100, 100, 100);
	
	kinect.update();
	
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {        

        // load grayscale depth image from the kinect source
        grayImage.setFromPixels(kinect.getDepthPixels());
		
        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        if(bThreshWithOpenCV) {
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        } else {
			
            // or we do it ourselves - show people how they can work with the pixels
            ofPixels & pix = grayImage.getPixels();
            int numPixels = pix.size();
            for(int i = 0; i < numPixels; i++) {
                if(pix[i] < nearThreshold && pix[i] > farThreshold) {
                    pix[i] = 255;
                } else {
                    pix[i] = 0;
                }
            }
        }
		
        // update the cv images
        grayImage.flagImageChanged();
		
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayImage, 100, (kinect.getDepthWidth()*kinect.getDepthHeight())/2, 20, false);
	}
	
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofSetColor(255, 255, 255);	

    // draw from the live kinect
    float ratio = (float)kinect.getHeight()/(float)kinect.getWidth();
    int imgW = 450;

    kinect.drawDepth(10, 10, imgW, ratio*imgW);
    kinect.draw(imgW+20, 10, imgW, ratio*imgW);

    grayImage.draw(10, ratio*imgW+20, imgW, ratio*imgW);
    contourFinder.draw(10, ratio*imgW+20, imgW, ratio*imgW);
	
	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;        
    
    reportStream << "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
	<< "set near threshold " << nearThreshold << " (press: + -)" << endl
    << "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs << endl;

    ofDrawBitmapString(reportStream.str(), 20, 700);
}

//--------------------------------------------------------------
void ofApp::exit() {

	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {
		case ' ':
			bThreshWithOpenCV = !bThreshWithOpenCV;
			break;			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;			
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
