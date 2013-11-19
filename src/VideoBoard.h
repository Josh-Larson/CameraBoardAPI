#ifndef VIDEOBOARD_H
#define VIDEOBOARD_H

#include "VideoBoardTypes.h"

class VideoBoard {
	private:
	const char * device;
	bool initialized;
	int fd;
	int width;
	int height;
	unsigned int method;
	unsigned int numBuffers;
	unsigned int bufferSize;
	vector <ImageContainer> images;
	Buffer * buffers;
	struct v4l2_buffer buf;
	
	int xioctl(int fd, int request, void * arg);
	int readFrame(Buffer * buffer);
	int readFrameMmap(Buffer * buffer);
	bool openDevice();
	bool initializeDevice();
	void initMmap();
	
	public:
	static const unsigned int METHOD_MMAP;
	static const unsigned int METHOD_READ;
	VideoBoard(const char * device, int width, int height);
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

#endif // VIDEOBOARD_H

