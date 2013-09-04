#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include "CameraBoard.h"
using namespace std;

CameraBoard cameraBoard;
unsigned char * combined_data;
volatile int pictures_taken = 0;
const int max_pictures = 5;

void onImageTaken(unsigned char *data, unsigned int length) {
	stringstream ss;
	ss << "output_image" << pictures_taken << ".bmp";
	string pTaken;
	ss >> pTaken;
	FILE * file = fopen(pTaken.c_str(), "w");
	fwrite(combined_data, 1, length, file);
	fclose(file);
	cout << "Saved Image\n";
	pictures_taken++;
}

int main(int argc, char *argv[]) {
	int width = 1280;
	int height = 720;
	unsigned int length = width*height*4;
	combined_data = new unsigned char[length];
	
	cout << "Initializing...\n";
	cameraBoard.setWidth(width);
	cameraBoard.setHeight(height);
	cameraBoard.setEncoding(CAMERA_BOARD_ENCODING_JPEG);
	
	if (cameraBoard.initialize()) return -1;
	cameraBoard.setImageCallback(onImageTaken);
	unsigned char * data = new unsigned char[length];
	int ISO = 128000;
	while (pictures_taken < max_pictures) {
		if (cameraBoard.startCapture(data, 0, length)) return -1;
		const int currentImages = pictures_taken;
		while (currentImages == pictures_taken)
			usleep(2000);
		cameraBoard.stopCapture();
		usleep(1000);
	}
	return 0;
}

