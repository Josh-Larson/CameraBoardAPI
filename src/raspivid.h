#ifndef _RaspiVideo_H
#define _RaspiVideo_H

#include "VideoBoardTypes.h"
class VideoBoard;
class RaspiVid{
	//the implementation of the camera
	VideoBoard *_impl;

public:
	static const unsigned int METHOD_MMAP;
	static const unsigned int METHOD_READ;
	RaspiVid(const char * device, int width, int height);
	~RaspiVid();
	
	
	int getWidth();
	int getHeight();
	bool initialize(unsigned int method);
	void destroy();
	void startCapturing();
	VideoBuffer grabFrame();
	void releaseFrame(VideoBuffer * buffer);
	void setBrightness(int brightness);
	void setImage(const char * name, Mat image);
	Mat grabImage(const char * name);
	void releaseImage(const char * name);
};
#endif // _RaspiVideo_H
