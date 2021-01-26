#include "ofxLibfreenect2.h"

        const static int width = 1920;
        const static int height = 1080;
        const static int depthWidth = 512;
        const static int depthHeight = 424;
        
//--------------------------------------------------------------------------------
ofxLibfreenect2::ofxLibfreenect2(){
    bNewFrame       = false;
    bNewBuffer      = false;
    bKinectInited  = false;
    bIsConnected    = false;
    bUseTexture     = true;
    bUseRegistration = false;
    bClosing = false;
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
void ofxLibfreenect2::setRegistration(bool registration) {
    bUseRegistration = registration;
}

//--------------------------------------------------------------------
bool ofxLibfreenect2::setup(int w, int h)
{
    return true;
}

//--------------------------------------------------------------------
bool ofxLibfreenect2::init(bool infrared, bool video, bool texture) {
	if(isConnected()) {
		ofLogWarning("ofxLibfreenect2") << "init(): do not call init while ofxLibfreenect2 is running!";
		return false;
	}

    bKinectInited = false;
    bUseRGB = video;
    bUseInfrared = infrared;
    bUseTexture = texture;

#ifdef TARGET_LINUX
    //This is a work around for iHD Intel drivers - see https://github.com/OpenKinect/libfreenect2/issues/1137
    if(strcmp("Intel",(const char*) glGetString(GL_VENDOR)) == 0) {
        ofLogVerbose() << "Intel GPU detected";
        char driver_name[] = "LIBVA_DRIVER_NAME=i965";
        if (putenv(driver_name) != 0) {
            ofLogError() << "Failed to set correct environment variable for Intel Linux";
        } else {
            ofLogVerbose() << "set LIBVA_DRIVER_NAME=i965";
        }
    }
#endif

    if(ofGetLogLevel() == OF_LOG_VERBOSE) {
        libfreenect2::Logger* logger = libfreenect2::createConsoleLogger(libfreenect2::Logger::Level::Info);
        libfreenect2::setGlobalLogger(logger);
    }
    else if(ofGetLogLevel() == OF_LOG_NOTICE) {
        libfreenect2::Logger* logger = libfreenect2::createConsoleLogger(libfreenect2::Logger::Level::Warning);
        libfreenect2::setGlobalLogger(logger);
    }
    else if(ofGetLogLevel() == OF_LOG_WARNING) {
        libfreenect2::Logger* logger = libfreenect2::createConsoleLogger(libfreenect2::Logger::Level::Warning);
        libfreenect2::setGlobalLogger(logger);
    }
    else if(ofGetLogLevel() == OF_LOG_ERROR) {
        libfreenect2::Logger* logger = libfreenect2::createConsoleLogger(libfreenect2::Logger::Level::Error);
        libfreenect2::setGlobalLogger(logger);
    }
    else if(ofGetLogLevel() == OF_LOG_SILENT) {
        libfreenect2::Logger* logger = libfreenect2::createConsoleLogger(libfreenect2::Logger::Level::None);
        libfreenect2::setGlobalLogger(logger);
    }

    if(freenect2.enumerateDevices() == 0)
    {
		ofLogError("ofxLibfreenect2") << "no Kinect2 device connected!";
		return false;
	}

    if(!pipeline) {
        pipeline = new libfreenect2::OpenCLPacketPipeline();
	}

    bKinectInited = true;

    return bKinectInited;
}

//--------------------------------------------------------------------------------
bool ofxLibfreenect2::isInitialized() const
{
    return bKinectInited;
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
bool ofxLibfreenect2::open(int deviceIndex){
    if(deviceIndex == -1) {
        deviceSerial = freenect2.getDefaultDeviceSerialNumber();
    } else {
        deviceSerial = freenect2.getDeviceSerialNumber(deviceIndex);
    }

    bool bSuccess = open(deviceSerial);

    return bSuccess;
}

//--------------------------------------------------------------------------------
bool ofxLibfreenect2::open(string serial){
    close();

    bNewFrame  = false;
    bNewBuffer = false;

    deviceSerial = serial;

	if(pipeline)
	{
        dev = freenect2.openDevice(deviceSerial, pipeline);
	}
	else
	{
        dev = freenect2.openDevice(deviceSerial);
	}

	if(dev == 0)
	{
		ofLogError("ofxLibfreenect2") << "Failure opening device!" << std::endl;
		return false;
	} else {
        ofLogNotice("ofxLibfreenect2") << "Opening device with serial: " << dev->getSerialNumber() << " device firmware: " << dev->getFirmwareVersion();
		lastFrameNo = -1;
        listener = new libfreenect2::SyncMultiFrameListener(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
        dev->setColorFrameListener(listener);
        dev->setIrAndDepthFrameListener(listener);
        dev->start();
        registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
        undistorted = new libfreenect2::Frame(512, 424, 4);
        registered = new libfreenect2::Frame(512, 424, 4);
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
        ofLogNotice("ofxLibfreenect2") << "No devices found";
		return;
	}
    else {
        ofLogNotice() << "Devices Found: ";
        for(int i = 0;i < numDevices; i++) {
            ofLogNotice() << "Kinect2 index: " << i << " serial: " << freenect2.getDeviceSerialNumber(i);
        }
    }
}

//--------------------------------------------------------------------------------
void ofxLibfreenect2::threadedFunction(){

    while(isThreadRunning()){
        listener->waitForNewFrame(frames);
		libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
		libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
		libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

        if(bUseRegistration) {
            registration->apply(rgb,depth,undistorted,registered);
        }

        ofPixelFormat pixelFormat;
        if (rgb->format == libfreenect2::Frame::BGRX) {
            pixelFormat = OF_PIXELS_BGRA;
        } else {
            pixelFormat = OF_PIXELS_RGBA;
        }

        if(bUseRGB) {
            videoPixelsBack.setFromPixels(rgb->data, rgb->width, rgb->height, pixelFormat);
            videoPixelsFront.swap(videoPixelsBack);
        }
        if(bUseInfrared) {
            irPixelsBack.setFromPixels(reinterpret_cast<float*>(ir->data), ir->width, ir->height, 1);
            irPixelsFront.swap(irPixelsBack);
        }
        if(bUseRegistration) {
            registeredPixelsBack.setFromPixels(registered->data, registered->width, registered->height, pixelFormat);
            registeredPixelsFront.swap(registeredPixelsBack);
        }
        depthPixelsBack.setFromPixels(reinterpret_cast<float*>(depth->data), depth->width, depth->height, 1);
        depthPixelsFront.swap(depthPixelsBack);

        lock();
        bNewBuffer = true;
        unlock();

        listener->release(frames);

        ofSleepMillis(2);
    }

    dev->stop();
    dev->close();

    if(listener != nullptr) {
        delete listener;
        listener = nullptr;
    }
    if (undistorted != nullptr) {
        delete undistorted;
        undistorted = nullptr;
    }
    if (registered != nullptr) {
        delete registered;
        registered = nullptr;
    }
    if (registration != nullptr) {
        delete registration;
        registration = nullptr;
    }
}

//--------------------------------------------------------------------------------
void ofxLibfreenect2::update(){
    if(!bKinectInited || !isConnected() || bClosing) return;

    if( ofGetFrameNum() != (unsigned int) lastFrameNo ){
        bNewFrame = false;
        lastFrameNo = ofGetFrameNum();
    }
    if( bNewBuffer ){
        lock();
            rawDepthPixels = depthPixelsFront;
            if(bUseRGB) videoPixels = videoPixelsFront;
            if(bUseInfrared) rawIrPixels = irPixelsFront;
            if(bUseRegistration) registeredPixels = registeredPixelsFront;
            bNewBuffer = false;
        unlock();

        // TOOD: Need to use lookup table like Kinect 1
        if( rawDepthPixels.size() > 0 ){
            if( depthPixels.getWidth() != rawDepthPixels.getWidth() ){
                depthPixels.allocate(rawDepthPixels.getWidth(), rawDepthPixels.getHeight(), 1);
            }
            unsigned char * pixels  = depthPixels.getData();
            for(size_t i = 0; i < depthPixels.size(); i++){
                pixels[i] = ofMap(rawDepthPixels[i], minDistance, maxDistance, 255, 0, true);
                if( pixels[i] == 255 ){
                    pixels[i] = 0;
                }
            }
        }

        // TOOD: Need to use lookup table like Kinect 1
        if (rawIrPixels.size() > 0) {
            if (irPixels.getWidth() != rawIrPixels.getWidth()) {
                irPixels.allocate(rawIrPixels.getWidth(), rawIrPixels.getHeight(), 1);
            }
            float* pixelsFloat = rawIrPixels.getData();
            unsigned char * pixels = irPixels.getData();
            for (size_t i = 0; i < irPixels.size(); i++) {
                pixels[i] = ofMap(pixelsFloat[i], 0, 4500, 0, 255, true);
            }
        }

        if(bUseTexture) {
            depthTex.loadData( depthPixels );
            if(bUseRGB) {
                videoTex.loadData( videoPixels );
            }
            if(bUseInfrared) {
                irTex.loadData( irPixels );
            }
        }

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
        if(bUseInfrared) {
            if(irTex.isAllocated()) {
                irTex.draw(_x, _y, _w, _h);
            }
        } else {
            if(videoTex.isAllocated()) {
                videoTex.draw(_x, _y, _w, _h);
            }
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

//----------------------------------------------------------
float ofxLibfreenect2::getDepthHeight() const{
	return (float) depthHeight;
}

//---------------------------------------------------------------------------
float ofxLibfreenect2::getDepthWidth() const{
	return (float) depthWidth;
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

//--------------------------------------------------------------------------------
ofPixels & ofxLibfreenect2::getIrPixels(){
    return irPixels;
}

//--------------------------------------------------------------------------------
const ofPixels & ofxLibfreenect2::getIrPixels() const{
    return irPixels;
}

//------------------------------------
float ofxLibfreenect2::getDistanceAt(int x, int y)  const{
    return getWorldCoordinateAt(x, y).z;
}

//------------------------------------
float ofxLibfreenect2::getDistanceAt(const ofPoint & p)  const{
    return getDistanceAt(p.x, p.y);
}

//------------------------------------
ofVec3f ofxLibfreenect2::getWorldCoordinateAt(int x, int y)  const{
    float wx, wy, wz;

    if(!bKinectInited) {
        ofLogError("ofxLibfreenect2::getWorldCoordinateAt") << "Kinect is not initialised";
        return ofVec3f(0, 0, 0);
    }

    if(bUseRegistration) {
        if (registration && undistorted)
        {
            if (x < undistorted->width && y < undistorted->height) {
                registration->getPointXYZ(undistorted, y, x, wx, wy, wz);
            }
            else {
                ofLogWarning("ofxLibfreenect2::getWorldCoordinateAt") << "Invalid coordinates...";
            }
        }
    } else {
        ofLogError("ofxLibfreenect2::getWorldCoordinateAt") << "Registration not enabled";
        return ofVec3f(0, 0, 0);
    }

    return ofVec3f(wx, wy, wz);
}


//------------------------------------
ofColor ofxLibfreenect2::getColorAt(int x, int y)  const{    
    return registeredPixels.getColor(x, y);
}

//------------------------------------
ofColor ofxLibfreenect2::getColorAt(const ofPoint & p)  const{
    return getColorAt(p.x, p.y);
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
    bClosing = true;
    if(bIsConnected) {
        ofLogVerbose() << "Stopping Thread...";
		stopThread();
        ofLogVerbose() << "Waiting for thread to finish...";
        waitForThread(false);
    }

    bNewFrame       = false;
    bNewBuffer      = false;
    bIsConnected    = false;
    lastFrameNo     = -1;
    bClosing = false;
}


