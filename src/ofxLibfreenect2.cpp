#include "ofxLibfreenect2.h"

//--------------------------------------------------------------------------------
ofxLibfreenect2::ofxLibfreenect2(){
    bNewFrame       = false;
    bNewBuffer      = false;
    bGrabberInited  = false;
    bIsConnected    = false;
    bUseTexture     = true;
    lastFrameNo     = -1;

    //set default distance range to 50cm - 600cm
    minDistance.set("minDistance", 500, 0, 12000);
    maxDistance.set("maxDistance", 6000, 0, 12000);
}

//--------------------------------------------------------------------------------
ofxLibfreenect2::~ofxLibfreenect2(){
    close();
}

//--------------------------------------------------------------------
bool ofxLibfreenect2::setup(int w, int h)
{
    return true;
}

//--------------------------------------------------------------------
bool ofxLibfreenect2::init(bool texture) {
	if(isConnected()) {
		ofLogWarning("ofxLibfreenect2") << "init(): do not call init while ofxLibfreenect2 is running!";
		return false;
	}

	//clear();
    bGrabberInited = false;

    if(freenect2.enumerateDevices() == 0)
    {
		ofLogError("ofxLibfreenect2") << "no Kinect2 device connected!";
		return false;
	}

	bUseTexture = texture;

	videoPixels.allocate(width, height, GL_RGBA);
	videoPixelsFront.allocate(width, height, GL_RGBA);
	videoPixelsBack.allocate(width, height, GL_RGBA);

	depthPixels.allocate(depthWidth, depthHeight, 1);
	depthPixelsFront.allocate(depthWidth, depthHeight, 1);
	depthPixelsBack.allocate(depthWidth, depthHeight, 1);

	videoPixels.set(0);
	videoPixelsFront.set(0);
	videoPixelsBack.set(0);

	depthPixels.set(0);
	depthPixelsFront.set(0);
	depthPixelsBack.set(0);

	rawDepthPixels.set(0);

	if(bUseTexture) {
		depthTex.allocate(depthWidth, depthHeight, GL_LUMINANCE);
		videoTex.allocate(width, height, GL_RGB);
	}

    if(!pipeline) {
        pipeline = new libfreenect2::OpenCLPacketPipeline();
	}

    libfreenect2::Logger* logger = libfreenect2::createConsoleLogger(libfreenect2::Logger::Level::Warning);
    libfreenect2::setGlobalLogger(logger);

	bGrabberInited = true;

	return bGrabberInited;
}

//--------------------------------------------------------------------------------
bool ofxLibfreenect2::isInitialized() const
{
    return bGrabberInited;
}

//---------------------------------------------------------------------------
bool ofxLibfreenect2::isConnected() const{
	return isThreadRunning();
}

//--------------------------------------------------------------------------------
bool ofxLibfreenect2::isConnected(int id)
{
    return isConnected();
}

//--------------------------------------------------------------------------------
bool ofxLibfreenect2::open(){
    close();

    bNewFrame  = false;
    bNewBuffer = false;

    serial = freenect2.getDefaultDeviceSerialNumber();


	if(pipeline)
	{
		dev = freenect2.openDevice(serial, pipeline);
	}
	else
	{
		dev = freenect2.openDevice(serial);
	}

	if(dev == 0)
	{
		ofLogError("ofxLibfreenect2") << "Failure opening device!" << std::endl;
		return false;
	} else {
        ofLogNotice("ofxLibfreenect2") << "Opening device with serial: " << dev->getSerialNumber() << " device firmware: " << dev->getFirmwareVersion();
		lastFrameNo = -1;
		startThread();
	}

    bIsConnected = true;

    return true;
}

void ofxLibfreenect2::listDevices() {
    if(!isInitialized())
		init();

    int numDevices = freenect2.enumerateDevices();

	if(numDevices == 0) {
		ofLogNotice("ofxLibfreenect2") << "no devices found";
		return;
	}
	else if(numDevices == 1) {
		ofLogNotice("ofxLibfreenect2") << 1 << " device found";
	}
	else {
		ofLogNotice("ofxLibfreenect2") << numDevices <<" devices found";
	}

}

//--------------------------------------------------------------------------------
void ofxLibfreenect2::threadedFunction(){
    //listener = new listener(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
    libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
    libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4);

    dev->setColorFrameListener(&listener);
    dev->setIrAndDepthFrameListener(&listener);
    dev->start();

    registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());

    while(isThreadRunning()){
		listener.waitForNewFrame(frames);
		libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
		libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
		libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

		registration->apply(rgb,depth,&undistorted,&registered);

        lock();


        videoPixelsBack.setFromPixels(rgb->data, rgb->width, rgb->height, OF_PIXELS_RGBA);
        depthPixelsBack.setFromPixels((float *)depth->data, ir->width, ir->height, 1);

        videoPixelsFront.swap(videoPixelsBack);
        depthPixelsFront.swap(depthPixelsBack);

  //      lock();
        bNewBuffer = true;
        unlock();

        //ofSleepMillis(2);

		listener.release(frames);
    }

    dev->stop();
    dev->close();

    delete registration;
}

//--------------------------------------------------------------------------------
void ofxLibfreenect2::update(){
    if(!bGrabberInited) return;
    if(!isConnected()) return;

    if( ofGetFrameNum() != (unsigned int) lastFrameNo ){
        bNewFrame = false;
        lastFrameNo = ofGetFrameNum();
    }
    if( bNewBuffer ){
        lock();
            //videoPixelsFront.swapRgb();
            videoPixels = videoPixelsFront;
            videoPixels.swapRgb();
            rawDepthPixels = depthPixelsFront;
            bNewBuffer = false;
        unlock();

        if( rawDepthPixels.size() > 0 ){
            if( depthPixels.getWidth() != rawDepthPixels.getWidth() ){
                depthPixels.allocate(rawDepthPixels.getWidth(), rawDepthPixels.getHeight(), 1);
            }

            //float * pixelsF         = rawDepthPixels.getData();
            unsigned char * pixels  = depthPixels.getData();

            for(int i = 0; i < depthPixels.size(); i++){
                pixels[i] = ofMap(rawDepthPixels[i], minDistance, maxDistance, 255, 0, true);
                if( pixels[i] == 255 ){
                    pixels[i] = 0;
                }
            }

        }

        depthTex.loadData( depthPixels, GL_LUMINANCE );
        videoTex.loadData( videoPixels, GL_RGBA );

        bNewFrame = true;
    }
}

//--------------------------------------------------------------------------------
bool ofxLibfreenect2::isFrameNew() const {
    return bNewFrame;
}

//------------------------------------
bool ofxLibfreenect2::isUsingTexture() const{
	return bUseTexture;
}

//----------------------------------------------------------
void ofxLibfreenect2::draw(float _x, float _y, float _w, float _h) const{
	if(bUseTexture) {
        if(videoTex.isAllocated()) {
            videoTex.draw(_x, _y, _w, _h);
        }
	}
}

//----------------------------------------------------------
void ofxLibfreenect2::draw(float _x, float _y) const{
	draw(_x, _y, (float)width, (float)height);
}

//----------------------------------------------------------
void ofxLibfreenect2::drawDepth(float _x, float _y, float _w, float _h) const{
	if(bUseTexture) {
        if(depthTex.isAllocated()) {
            depthTex.draw(_x, _y, _w, _h);
        }
	}
}

//---------------------------------------------------------------------------
void ofxLibfreenect2::drawDepth(float _x, float _y) const{
	drawDepth(_x, _y, (float)width, (float)height);
}

//--------------------------------------------------------------------
bool ofxLibfreenect2::setPixelFormat(ofPixelFormat pixelFormat){
	if(pixelFormat==OF_PIXELS_RGB){
		return true;
	}else{
		return false;
	}
}

//--------------------------------------------------------------------
ofPixelFormat ofxLibfreenect2::getPixelFormat() const{
    return OF_PIXELS_RGB;
}

//----------------------------------------------------------
float ofxLibfreenect2::getHeight() const{
	return (float) height;
}

//---------------------------------------------------------------------------
float ofxLibfreenect2::getWidth() const{
	return (float) width;
}

//--------------------------------------------------------------------------------
ofPixels & ofxLibfreenect2::getDepthPixels(){
    return depthPixels;
}

//--------------------------------------------------------------------------------
const ofPixels & ofxLibfreenect2::getDepthPixels() const{
	return depthPixels;
}

//--------------------------------------------------------------------------------
ofPixels & ofxLibfreenect2::getPixels(){
	return videoPixels;
}

//--------------------------------------------------------------------------------
const ofPixels & ofxLibfreenect2::getPixels() const{
	return videoPixels;
}

//------------------------------------
ofFloatPixels & ofxLibfreenect2::getDistancePixels(){
	return rawDepthPixels;
}

//------------------------------------
const ofFloatPixels & ofxLibfreenect2::getDistancePixels() const{
	return rawDepthPixels;
}

//------------------------------------
ofTexture& ofxLibfreenect2::getTexture(){
	if(!videoTex.isAllocated()){
		ofLogWarning("ofxLibfreenect2") << "getTexture(): device " << deviceId << " video texture not allocated";
	}
	return videoTex;
}

//---------------------------------------------------------------------------
ofTexture& ofxLibfreenect2::getDepthTexture(){
	if(!depthTex.isAllocated()){
		ofLogWarning("ofxLibfreenect2") << "getDepthTexture(): device " << deviceId << " depth texture not allocated";
	}
	return depthTex;
}

//------------------------------------
const ofTexture& ofxLibfreenect2::getTexture() const{
	if(!videoTex.isAllocated()){
		ofLogWarning("ofxLibfreenect2") << "getTexture(): device " << deviceId << " video texture not allocated";
	}
	return videoTex;
}

//---------------------------------------------------------------------------
const ofTexture& ofxLibfreenect2::getDepthTexture() const{
	if(!depthTex.isAllocated()){
		ofLogWarning("ofxLibfreenect2") << "getDepthTexture(): device " << deviceId << " depth texture not allocated";
	}
	return depthTex;
}

//--------------------------------------------------------------------------------
void ofxLibfreenect2::close()
{
    if(bIsConnected) {
		stopThread();
		ofSleepMillis(10);
		waitForThread(false);
    }

    bNewFrame       = false;
    bNewBuffer      = false;
    bIsConnected    = false;
    lastFrameNo     = -1;

}


