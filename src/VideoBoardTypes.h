#ifndef VideoBoardTypes_H
#define VideoBoardTypes_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>

#include "VideoBuffer.h"
using namespace std;
using namespace cv;

struct Buffer {
	unsigned char * start;
	unsigned int length;
};
#endif // VideoBoardTypes_H
