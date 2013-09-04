export CAMERA_MODULE_INCLUDES="-I/opt/vc/include"
g++ -c -o CameraBoard.o $CAMERA_MODULE_INCLUDES ../CameraBoard.cpp
g++ -o CameraTest $CAMERA_MODULE_INCLUDES -L/opt/vc/lib *.o -lmmal CameraTest.cpp
