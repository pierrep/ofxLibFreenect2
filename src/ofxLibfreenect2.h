
#pragma once

#include "ofMain.h"
#include "ofxBase3DVideo.h"

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>

class ofxLibfreenect2 : public ofThread, public ofxBase3DVideo {

    public:
        ofxLibfreenect2();
        ~ofxLibfreenect2();

        bool init(bool infrared=false, bool video=true, bool texture=true);
        bool setup(int w, int h);
        bool open();
        void update();
        void close();
        void listDevices();

        bool isInitialized() const;
        bool isConnected() const;
        bool isConnected(int id);
        bool isFrameNew() const;

        ofPixels & getPixels();
        const ofPixels & getPixels() const;

        ofPixels& getDepthPixels();
        const ofPixels& getDepthPixels() const;

        /// get the calulated distance for a depth point
        float getDistanceAt(int x, int y) const;
        float getDistanceAt(const ofPoint & p) const;

        /// calculates the coordinate in the world for the depth point (perspective calculation)
        /// center of image is (0.0)
        ofVec3f getWorldCoordinateAt(int cx, int cy) const;
        //ofVec3f getWorldCoordinateAt(float cx, float cy, float wz) const;

        /// get the distance in millimeters to a given point as a float array
        ofFloatPixels & getDistancePixels();
        const ofFloatPixels & getDistancePixels() const;

        /// get the RGB value for a depth point
        ///
        /// see setRegistration() for calibrated depth->RGB points
        ofColor getColorAt(int x, int y) const;
        ofColor getColorAt(const ofPoint & p) const;

        /// calibrates the depth image to align with the rgb image, disabled by default
        ///
        /// call this before open(), has no effect while the connection is running
        void setRegistration(bool bUseRegistration);

        /// get the video (ir or rgb) texture
        ofTexture& getTexture();
        const ofTexture& getTexture() const;

        ofTexture& getDepthTexture();
        const ofTexture& getDepthTexture() const;

        bool setPixelFormat(ofPixelFormat pixelFormat);
        ofPixelFormat getPixelFormat() const;

        void setUseTexture(bool bUse);
        bool isUsingTexture() const;

        void draw(float x, float y, float w, float h) const;
        void draw(float x, float y) const;

        void drawDepth(float x, float y, float w, float h) const;
        void drawDepth(float x, float y) const;

        float getHeight() const;
        float getWidth() const;

        ofParameter <float> minDistance;
        ofParameter <float> maxDistance;

        const static int width = 1920;
        const static int height = 1080;
        const static int depthWidth = 512;
        const static int depthHeight = 424;

    protected:
        void threadedFunction();

        ofPixels videoPixels;
        ofPixels videoPixelsFront;
        ofPixels videoPixelsBack;
        ofPixels depthPixels;
        ofFloatPixels rawDepthPixels;
        ofFloatPixels depthPixelsFront;
        ofFloatPixels depthPixelsBack;
        ofPixels irPixels;
        ofFloatPixels rawIrPixels;
        ofFloatPixels irPixelsFront;
        ofFloatPixels irPixelsBack;
        ofPixels registeredPixels;
        ofPixels registeredPixelsBack;
        ofPixels registeredPixelsFront;
        ofPixelFormat pixelFormat;

        bool bNewBuffer;
        bool bNewFrame;
        bool bGrabberInited;
        bool bIsConnected;
        bool bUseInfrared;
        bool bUseRGB;
        bool bUseTexture;
        bool bUseRegistration;

        ofTexture depthTex;
        ofTexture videoTex;
        ofTexture irTex;

        int lastFrameNo;

        int deviceId;

        libfreenect2::Freenect2 freenect2;
		libfreenect2::Freenect2Device *dev = 0;
		libfreenect2::PacketPipeline *pipeline = 0;
        libfreenect2::Registration* registration = nullptr;
        libfreenect2::Frame* undistorted = nullptr;
        libfreenect2::Frame* registered = nullptr;
		libfreenect2::SyncMultiFrameListener* listener = 0;
		libfreenect2::FrameMap frames;

		string serial;

};
