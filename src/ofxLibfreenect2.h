
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

        bool init(bool texture=true);
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

        /// (not implemented) get the distance in millimeters to a given point as a float array
        ofFloatPixels & getDistancePixels();
        const ofFloatPixels & getDistancePixels() const;

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

        ofPixels videoPixelsFront;
        ofPixels videoPixelsBack;
        ofPixels videoPixels;
        ofFloatPixels depthPixelsFront;
        ofFloatPixels depthPixelsBack;
        ofPixels depthPixels;
        ofFloatPixels rawDepthPixels;
        ofPixelFormat pixelFormat;

        bool bNewBuffer;
        bool bNewFrame;
        bool bGrabberInited;
        bool bIsConnected;
        bool bUseTexture;



        ofTexture depthTex;
        ofTexture videoTex;

        int lastFrameNo;

        int deviceId;

        libfreenect2::Freenect2 freenect2;
		libfreenect2::Freenect2Device *dev = 0;
		libfreenect2::PacketPipeline *pipeline = 0;
		libfreenect2::Registration* registration;
		libfreenect2::SyncMultiFrameListener* listener = 0;
		libfreenect2::FrameMap frames;

		string serial;

};
