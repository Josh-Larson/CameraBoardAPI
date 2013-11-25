#include <iostream>
#include "raspivid.h"
#include <sys/time.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
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
	RaspiVid v("/dev/video0", width, height);
	if (!v.initialize(RaspiVid::METHOD_MMAP)) {
		cout << "Unable to initialize!\n";
		return -1;
	}
	v.setBrightness(10);
	
	v.startCapturing();
	long start = getmsofday();
	for (int i = 0; i < 100; i++) {
		// Receive key-press updates, it is required if you want to output images
		waitKey(1);
		
		// Grab a frame from the vision API
		VideoBuffer buffer = v.grabFrame();
		
		// Put the frame into an OpenCV image matrix
		Mat image(height, width, CV_8UC1, buffer.data(), false);
		
		// Show the greyscale image with OpenCV on the screen
		//imshow("Image", image);
		
		// Threshold the image into a new matrix with a minimum value of 1 and maximum of 255
		Mat thresh;
		inRange(image, Scalar(1), Scalar(255), thresh);
		
		// Show the thresholded image with OpenCV on the screen
		imshow("Threshold", thresh);
		
		// Find all the contours in the thresholded image
		// The original thresholded image will be destroyed while finding contours
		vector <vector<Point> > contours;
		findContours(thresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		
		// Output information
		long now = getmsofday();
		cout << "Frame #" << (i+1) << "   Frame Time: " << (now - start) << "ms   Estimated FPS: " << (1000 / (now-start)) << " FPS      \r";
		cout.flush();
		start = now;
	}
	v.destroy();
}

