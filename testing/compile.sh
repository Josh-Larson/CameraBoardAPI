export CAMERA_MODULE_INCLUDES="-I/opt/vc/include/interface/vcos/pthreads/ -I/home/pi/devel/userland/ -I/opt/vc/include"
g++ -c -o CameraBoard.o $CAMERA_MODULE_INCLUDES ../CameraBoard.cpp
g++ -o CameraTest $CAMERA_MODULE_INCLUDES -L/opt/vc/lib *.o -lmmal -lvcos -lbcm_host CameraTest.cpp
