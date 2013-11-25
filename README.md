RaspiCam and RaspiVid project

### How to build:
Create a new directory for build, then run cmake and make
```
mkdir build
cd build
cmake ..
make
```

### How to initialize /dev/video0 to have the camera stream
Installing UV4L (also accessible here: http://www.linux-projects.org/modules/sections/index.php?op=viewarticle&artid=14)
```
wget http://www.linux-projects.org/listing/uv4l_repo/lrkey.asc && sudo apt-key add ./lrkey.asc
deb http://www.linux-projects.org/listing/uv4l_repo/raspbian/ wheezy main
sudo apt-get update
sudo apt-get install uv4l uv4l-raspicam
```
To initialize /dev/video0 with realtime scheduling, a 640x480 image size, YUV420 encoding, a blurring effect, auto whitebalance off, and the framerate capped at 30fps.
```
sudo uv4l --sched-rr --driver raspicam --auto-video_nr --width 640 --height 480 --encoding yuv420 --nopreview --imgfx blur --awb off --framerate 30
```
If you want to destroy /dev/video0 and re-create it, run this command and then re-initialize it by running the command above.
```
sudo pkill uv4l
```

### How to run the RaspiCam test program:
This test program shows a little bit about how to use the slower but more functional RaspiCam API.
```
cd build/utils
LD_PRELOAD=/usr/lib/uv4l/uv4lext/armv6l/libuv4lext.so ./camera_test
```

### How to run the RaspiVid test program:
This test program shows a little bit about how to use the much faster RaspiVid API that would work better for fast video processing applications.
```
cd build/utils
LD_PRELOAD=/usr/lib/uv4l/uv4lext/armv6l/libuv4lext.so ./video_test
```

### How to build and run the example program:
The example program uses the RaspiVid API to grab 100 frames and do some OpenCV processing.
```
# Copy library files from when the project was built
cd examples
cp ../build/src/lib* ./

# Compile example program
g++ -L/usr/lib/uv4l/uv4lext/armv6l -I ../src/ -L ./ -lraspicam -luv4lext -Wl,-rpath,'/usr/lib/uv4l/uv4lext/armv6l' `pkg-config --cflags opencv` `pkg-config --libs opencv` -o FindContours FindContours.cpp

# Run example program
LD_PRELOAD=/usr/lib/uv4l/uv4lext/armv6l/libuv4lext.so ./FindContours;
```

### Possible Problems:
* If you are using Memory Mapping (RaspiVid::METHOD_MMAP) then when you run the program you must be sure to specify LD_PRELOAD before you execute.
* If the framerate appears spurratic or very slow, make sure you initialized the UV4L driver with --sched-rr
* If the camera won't start up at all, make sure you are supplying at least 5V at 1.0A to the Raspberry Pi.``

