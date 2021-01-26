ofxLibFreenect2
===============


Description
-----------

This is an OpenFrameworks addon for Kinect V2 using the libfreenect2 library:

https://github.com/OpenKinect/libfreenect2

Static libraries are provided in this addon for Linux and Windows. If they don't work for you install libfreenect2 from above URL and link to the static (or dynamic) lib.

This addon requires OpenCL to be installed. How to do this is platform specific, see below for more details.

I've tried to model it as close to the original ofxKinect as possible, to make it an easy drop in replacement. 

For an addon using Libfreenect2 and OpenNI, check out https://github.com/pierrep/ofxNI2


Dependencies
------------
You'll need some dependencies installed to get this addon working, most importantly OpenCL. 

### Linux

More general info can be found here - https://github.com/OpenKinect/libfreenect2/blob/master/README.md#linux

#### Ubuntu
You can install clinfo to verify if you have correctly set up the OpenCL stack, i.e.

```
sudo apt install clinfo
```

Under Ubuntu if you have an Intel iGPU you'll need to install the following packages to get the OpenCL backend working:

```
sudo apt install beignet-dev
```
Otherwise you'll need these too:
```
sudo apt install libva-dev
sudo apt install libturbojpeg0-dev 

``` 

#### Arch

You'll need your GPU vendor OpenCL SDK. See here for more details: https://wiki.archlinux.org/index.php/GPGPU
On NVidia, the Cuda package should install OpenCL, you may have it already installed.

Otherwise you'll need these too:

```
pacman -Sy ocl-icd opencl-headers libjpeg-turbo
```



Build Instructions for Libfreenect2 (optional)
----------------------------------------------

### Linux

- Build libfreenect2 by following the instructions [here](https://github.com/OpenKinect/libfreenect2#linux) _except_ that instead of building the library as suggested

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/freenect2
```
You need to build the static library and install it system-wide:
```bash
cmake -DBUILD_SHARED_LIBS=OFF ..
```
- Then copy the static library to the ofxLibFreenect addon directory:
```bash
cd ofxLibFreenect2
cp /usr/local/lib/libfreenect2.a libs/libfreenect2/lib/linux64
```






