#include <iostream>
#include "raspivid.h"
#include <sys/time.h>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

unsigned long getmsofday() {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return (long long)tv.tv_sec*1000 + tv.tv_usec/1000;
}

int main() {
	const int width = 320;
	const int height = 240;
	namedWindow("Image", CV_WINDOW_AUTOSIZE);
	RaspiVid v("/dev/video0", width, height);
	if (!v.initialize(RaspiVid::METHOD_MMAP)) {
		cout << "Unable to initialize!\n";
		return -1;
	}
	v.setBrightness(10);
	
	v.startCapturing();
	long start = getmsofday();
	for (int i = 0; i < 100; i++) {
		waitKey(1);
		VideoBuffer buffer = v.grabFrame();
		Mat image(height, width, CV_8UC1, buffer.data(), false);
		imshow("Image", image);
		long now = getmsofday();
		cout << "Frame #" << (i+1) << "\tTime: " << (now - start) << "ms         \r";
		cout.flush();
		start = now;
	}
	v.destroy();
}

