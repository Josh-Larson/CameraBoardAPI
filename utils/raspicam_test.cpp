#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include "raspicam.h"
using namespace std;

RaspiCam cameraBoard;
volatile int pictures_taken = 0;
const int max_pictures = 5;

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

int main(int argc, char *argv[]) {
	int width = 640;
	int height = 480;
	unsigned int length = 62 + width*height*3 + height*2; // Header + Image Data + Padding
	
	cout << "Initializing...\n";
	cameraBoard.setWidth(width);
	cameraBoard.setHeight(height);
	cameraBoard.setEncoding(CAMERA_BOARD_ENCODING_BMP);
	
	if (cameraBoard.initialize()) return -1;
	
	unsigned char * data = new unsigned char[length];
	if (cameraBoard.takePicture(data, length)) return -1;
	saveImage("output_blocking.bmp", data, length);
	cout << "Saved Blocking Image\n";
	while (pictures_taken < max_pictures) {
		if (cameraBoard.startCapture(onImageTaken, data, 0, length)) return -1;
		const int currentImages = pictures_taken;
		while (currentImages == pictures_taken)
			usleep(2000);
		cameraBoard.stopCapture();
		usleep(3000);
	}
	return 0;
}

