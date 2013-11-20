#include <iostream>
#include "raspivid.h"
#include <opencv2/highgui/highgui.hpp>
using namespace std;

unsigned long getmsofday() {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return (long long)tv.tv_sec*1000 + tv.tv_usec/1000;
}

int main() {
	const int width = 640;
	const int height = 480;
	RaspiVid v("/dev/video0", width, height);
	if (!v.initialize(RaspiVid::METHOD_READ)) {
		cout << "Unable to initialize!\n";
		return -1;
	}
	v.setBrightness(10);
	
	v.startCapturing();
	for (int i = 0; i < 305; i++) {
		long start = getmsofday();
		VideoBuffer buffer = v.grabFrame();
		cout << "Frame #" << (i+1) << "\tTime: " << (getmsofday() - start) << "\n";
		//Mat image(height, width, CV_8UC1, buffer.data(), false);
		//imshow("Image", image);
	}
	v.destroy();
}

