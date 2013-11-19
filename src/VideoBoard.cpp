#include "VideoBoard.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))
const unsigned int VideoBoard::METHOD_MMAP = (1 << 0);
const unsigned int VideoBoard::METHOD_READ = (1 << 1);

int VideoBoard::xioctl(int fd, int request, void * arg) {
	int r;

	do r = ioctl (fd, request, arg);
	while (-1 == r && EINTR == errno);

	return r;
}

int VideoBoard::readFrame(Buffer * buffer) {
	while (read(fd, buffer->start, buffer->length) == -1) {
		switch (errno) {
			case EAGAIN:
				continue;
			case EINTR:
				continue;
			case EIO:
				/* Could ignore EIO, see spec. */
				/* fall through */
			default:
				cout << "Reading Error!\n";
				return errno;
		}
	}
	return 0;
}

bool VideoBoard::readFrameMmap(Buffer * buffer) {
	CLEAR(buf);
	
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	
	if (xioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
		switch (errno) {
			case EAGAIN:
				return false;
				
			case EIO:
			default:
				cout << "Error: VIDIOC_DQBUF in readFrameMmap(Buffer*)\n";
				return true;
		}
	}
	
	assert(buf.index < numBuffers);
	
	buffer->start = buffers[buf.index].start;
	buffer->length = buf.bytesused;
	return true;
}

bool VideoBoard::openDevice() {
	struct stat st; 
	
	if (stat(device, &st) == -1) {
		cout << "Cannot identify " << device << " with error: " << strerror(errno) << "\n";
		return false;
	}
	
	if (!S_ISCHR(st.st_mode)) {
		cout << "Error: " << device << " is not a device!\n";
		return false;
	}
	
	fd = open (device, O_RDWR | O_NONBLOCK, 0);
	
	if (fd == -1) {
		cout << "Error: Cannot open " << device << " with error: " << strerror(errno) << "\n";
		return false;
	}
	return true;
}

bool VideoBoard::initializeDevice() {
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	
	if (xioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
		if (EINVAL == errno) {
			cout << "Error: " << device << " is not a V4L2 device!\n";
		} else {
			cout << "Error: VIDIOC_QUERYCAP\n";
		}
	}
	
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		cout << "Error: " << device << " does not support video capture\n";
	}
	
	if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
		cout << "Error: " << device << " does not support read i/o\n";
	}
	
	/* Select video input, video standard and tune here. */
	
	CLEAR (cropcap);
	
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	if (xioctl(fd, VIDIOC_CROPCAP, &cropcap) == 0) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; /* reset to default */
	
		if (-1 == xioctl (fd, VIDIOC_S_CROP, &crop)) {
			switch (errno) {
			case EINVAL:
				/* Cropping not supported. */
				break;
			default:
				/* Errors ignored. */
				break;
			}
		}
	} else {	
		/* Errors ignored. */
	}
	
	CLEAR (fmt);
	
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = width; 
	fmt.fmt.pix.height = height;
	
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
	
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	
	if (xioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
		cout << "Error: VIDIOC_S_FMT\n";
	
	bufferSize = fmt.fmt.pix.sizeimage;
}

void VideoBoard::initMmap() {
	struct v4l2_requestbuffers req;
	
	CLEAR(req);
	
	req.count = numBuffers;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	
	if (xioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
		if (EINVAL == errno) {
			cout << "Error: " << device << " does not support memory mapping\n";
			return;
		} else {
			cout << "Error: VIDIOC_REQBUFS\n";
		}
	}
	
	if (req.count < 2) {
		cout << "Error: Not enough memory on " << device << " !\n";
		return;
	}
	
	buffers = new Buffer[req.count];
	
	if (!buffers) {
		cout << "Error: Out of memory!\n";
	}
	
	for (unsigned int i = 0; i < req.count; i++) {
		struct v4l2_buffer buf;
		
		CLEAR(buf);
		
		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = i;
		
		if (xioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
			cout << "Error: VIDIOC_QUERYBUF\n";
			return;
		}
		
		buffers[i].length = buf.length;
		buffers[i].start = (unsigned char *) mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
		
		if (MAP_FAILED == buffers[i].start) {
			cout << "Error: mmap\n";
			return;
		}
	}
}

VideoBoard::VideoBoard(const char * device, int width, int height) {
	this->device = device;
	this->width = width;
	this->height = height;
	fd = -1;
}

int VideoBoard::getWidth() {
	return width;
}

int VideoBoard::getHeight() {
	return height;
}

bool VideoBoard::initialize(unsigned int method) {
	this->method = method;
	if (!openDevice())
		return false;
	if (!initializeDevice())
		return false;
	if (method == METHOD_MMAP)
		initMmap();
	return true;
}

void VideoBoard::destroy() {
	if (close(fd) == -1)
		cout << "Error: Failed to close device\n";
	for (unsigned int i = 0; i < images.size(); i++) {
		pthread_mutex_destroy(images[i].mtx);
	}
	fd = -1;
}

void VideoBoard::startCapturing() {
	if (method == METHOD_MMAP) {
		enum v4l2_buf_type type;
		for (unsigned int i = 0; i < numBuffers; i++) {
			struct v4l2_buffer buf;
		
			CLEAR(buf);
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;
		
			if (xioctl(fd, VIDIOC_QBUF, &buf) == -1) {
				cout << "Error: VIDIOC_QBUF\n";
				return;
			}
		}
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (xioctl(fd, VIDIOC_STREAMON, &type) == -1) {
			cout << "Error: VIDIOC_STREAMON\n";
			return;
		}
	}
}

VideoBuffer VideoBoard::grabFrame() {
	Buffer b;
	b.start = NULL;
	b.length = bufferSize;
	if (method == METHOD_READ) {
		b.start = new unsigned char[bufferSize];
		readFrame(&b);
	} else if (method == METHOD_MMAP) {
		readFrameMmap(&b);
	}
	#if 0
	while (true) {
		if (method == METHOD_READ) {
			#if 0
			fd_set fds;
		
			FD_ZERO(&fds);
			FD_SET(fd, &fds);
			struct timeval tv = (timeval){1, 0}; // seconds, microseconds
			switch (select(fd + 1, &fds, NULL, NULL, &tv)) {
				case -1:
					if (EINTR == errno)
						continue;
					cout << "Error: select failed\n";
					break;
				case 0:
					cout << "Error: select timeout\n";
					break;
			}
			#endif
			if (readFrame(&b) == EAGAIN)
				break;
		} else if (method == METHOD_MMAP) {
			if (readFrameMmap(&b))
				break;
		}
	}
	#endif
	return VideoBuffer(this, b.start, b.length);
}

void VideoBoard::setBrightness(int brightness) {
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;
	
	memset(&queryctrl, 0, sizeof(queryctrl));
	queryctrl.id = V4L2_CID_BRIGHTNESS;
	
	if (xioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) == -1) {
		cout << "V4L2_CID_BRIGHTNESS is unable to be queried.\n";
		return;
	}
	if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
		cout << "V4L2_CID_BRIGHTNESS is not supported.\n";
		return;
	}
	if (brightness < queryctrl.minimum)
		brightness = queryctrl.minimum;
	else if (brightness > queryctrl.maximum)
		brightness = queryctrl.maximum;
	
	memset(&control, 0, sizeof(control));
	control.id = V4L2_CID_BRIGHTNESS;
	control.value = brightness;
	
	if (xioctl(fd, VIDIOC_S_CTRL, &control) == -1) {
		cout << "Failed to set V4L2_CID_BRIGHTNESS value.\n";
		return;
	}
}

void VideoBoard::setImage(const char * name, Mat image) {
	for (unsigned int i = 0; i < images.size(); i++) {
		if (images[i].name == name) {
			pthread_mutex_lock(images[i].mtx);
			images[i].image = image;
			pthread_mutex_unlock(images[i].mtx);
			return;
		}
	}
	ImageContainer cont;
	cont.name = name;
	cont.image = image;
	pthread_mutex_init(cont.mtx, NULL);
	images.push_back(cont);
}

Mat VideoBoard::grabImage(const char * name) {
	for (unsigned int i = 0; i < images.size(); i++) {
		if (images[i].name == name) {
			pthread_mutex_lock(images[i].mtx);
			return images[i].image;
		}
	}
	return Mat();
}

void VideoBoard::releaseImage(const char * name) {
	for (unsigned int i = 0; i < images.size(); i++) {
		if (images[i].name == name) {
			pthread_mutex_unlock(images[i].mtx);
		}
	}
}

void VideoBoard::releaseFrame(VideoBuffer * buffer) {
	if (method == METHOD_READ) {
		delete buffer->data();
		return;
	} else if (method == METHOD_MMAP) {
		if (xioctl(fd, VIDIOC_QBUF, &buf) == -1) {
			cout << "Error: VIDIOC_QBUF in releaseFrame()\n";
		}
	}
}

