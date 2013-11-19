#ifndef VIDEOBUFFER_H
#define VIDEOBUFFER_H

class VideoBoard;
class VideoBuffer {
	private:
	VideoBoard * _board;
	unsigned char * _data;
	unsigned int _length;
	bool _initialized;
	
	public:
	VideoBuffer(VideoBoard * board, unsigned char * data, unsigned int length);
	~VideoBuffer();
	void release();
	unsigned char * data();
	unsigned int length();
	
};

#endif
