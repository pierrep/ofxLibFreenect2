# ofxLibFreenect2
Kinect V2 addon using the libfreenect2 library

https://github.com/OpenKinect/libfreenect2

Install libfreenect2 from above URL and link to the static lib.

Tested under Linux but should work on all platforms

This currently only uses the OpenCL backend, so make sure that is enabled.

I've tried to model it as close to the original ofxKinect as possible, to make it an easy drop in replacement. Point clouds are not currently included. There's the potential to hook this up with OpenNI and NITE but I haven't tried that yet.

