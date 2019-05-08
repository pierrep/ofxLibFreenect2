ofxLibFreenect2
===============


Description
-----------

This is an OpenFrameworks addon for Kinect V2 using the libfreenect2 library:

https://github.com/OpenKinect/libfreenect2

Install libfreenect2 from above URL and link to the static (or dynamic) lib.

Tested under Linux but should work on all platforms

This currently only uses the OpenCL backend, so make sure that is enabled.

I've tried to model it as close to the original ofxKinect as possible, to make it an easy drop in replacement. Point clouds are not currently included. For an addon using Libfreenect2 and OpenNI, check out https://github.com/pierrep/ofxNI2


Build Instructions
------------------

### Linux


- Build libfreenect2 by following the instructions [here](https://github.com/OpenKinect/libfreenect2#linux) except that instead of building the library as suggested:
```bash
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/freenect2
```
You need to build the static library and install it system-wide:
```bash
cmake -DBUILD_SHARED_LIBS=OFF
```
- Then copy the static library to the ofxLibFreenect addon directory:
```bash
cd ofxLibFreenect2
cp /usr/local/lib/libfreenect2.a libs/libfreenect2/lib/linux64
```

### Dependencies

Under Ubuntu 18.04 if you have an Intel iGPU you'll need to install:

```
sudo apt install libva-dev
sudo apt install libturbojpeg0-dev 

``` 




