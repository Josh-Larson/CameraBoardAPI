#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include "raspicam.h"
#include <cmath>
#include <ctime>
#include <sys/time.h>
using namespace std;

RaspiCam cameraBoard;
volatile int pictures_taken = 0;
const int max_pictures = 5;

int atoi(string s) {
	stringstream ss;
	ss << s;
	int i = 0;
	ss >> i;
	return i;
}

unsigned long getmsofday() {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return (long long)tv.tv_sec*1000 + tv.tv_usec/1000;
}

void saveImage(string filename, unsigned char *data, unsigned int length) {
	FILE * file = fopen(filename.c_str(), "w");
	fwrite(data, 1, length, file);
	fclose(file);
}

void onImageTaken(unsigned char *data, unsigned int image_offset, unsigned int length) {
	stringstream ss;
	ss << "output_image" << pictures_taken << ".bmp";
	string pTaken;
	ss >> pTaken;
	saveImage(pTaken, data + image_offset, length);
	cout << "Saved Image\n";
	pictures_taken++;
}

void testCamera(RaspiCam * board, unsigned char * data, unsigned int length, int width, int height) {
	int iterations = 10;
	double totalTime = 0;
	double minTime = -1;
	double maxTime = -1;
	double meanTime = 0;
	for (int i = 0; i < iterations; i++) {
		unsigned long startTime = getmsofday();
		board->takePicture(data, length);
		double time = getmsofday() - startTime;
		cout << "Picture taken in " << time << "ms\n";
		totalTime += time;
		meanTime += time;
		if (time < minTime || minTime == -1) minTime = time;
		if (time > maxTime || maxTime == -1) maxTime = time;
	}
	meanTime /= iterations;
	cout << "Statistics: (" << width << "x" << height << ")\n";
	cout << "\tMin:          " << minTime << "\n";
	cout << "\tMax:          " << maxTime << "\n";
	cout << "\tAverage Time: " << meanTime << "\n";
	cout << "\tTotal Time:   " << totalTime << "\n";
}

int main(int argc, char *argv[]) {
	int width = 640;
	int height = 480;
	if (argc >= 2) width = atoi(argv[1]);
	if (argc >= 3) height = atoi(argv[2]);
	unsigned int length = 54 + width*height*4; // Header + Image Data
	
	cout << "Initializing...\n";
	
	cameraBoard.setWidth(width);
	cameraBoard.setHeight(height);
	cameraBoard.setQuality(10);
	cameraBoard.setEncoding(CAMERA_BOARD_ENCODING_JPEG);
	
	if (cameraBoard.initialize()) return -1;
	
	cout << "Taking Video Stream:\n";
	unsigned char * data = new unsigned char[length];
	testCamera(&cameraBoard, data, length, width, height);
	cameraBoard.close();
	cout << "Stopped capture.\n";
	return 0;
}

