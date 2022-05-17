#include "v4l2_camera.hpp"

v4l2_camera::v4l2_camera(const char *dev_name)
{
    dev_name_ = dev_name;
}

v4l2_camera::~v4l2_camera()
{
    deinit();
} 

int v4l2_camera::init(const char *dev_name, int width, int height, int format)
{
    int ret;
    if(dev_name)
        dev_name_ = dev_name;
    ret = init(width, height, format);
    return ret;
}

int v4l2_camera::init(int width, int height, int format)
{
    int ret;

    ret = open_device();
    if(ret < 0) {
        printf("Failed to open device\n");
        return -1;
    }

    ret = set_format(width, height, format);
    if(ret < 0) {
        printf("Failed to set format\n");
        return -1;
    }

    ret = request_buffers(2);
    if(ret < 0) {
        printf("Failed to request buffers\n");
        return -1;
    }

    buffers_ = new Buffer<void>[req_.count];
    if(!buffers_) {
        printf("Failed to allocate buffers\n");
        return -1;
    }

    for(int i = 0; i < req_.count; i++) {
        ret = query_buffer(buf_, i);
        if(ret < 0) {
            printf("Failed to query buffer %d\n", i);
            return -1;
        }
        buffers_[i].length = buf_.length;
        buffers_[i].start = mmap(NULL, buf_.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, buf_.m.offset);
        if(buffers_[i].start == MAP_FAILED) {
            printf("Failed to mmap buffer %d\n", i);
            return -1;
        }
    }
    return 0;
}

int v4l2_camera::deinit()
{
    int ret;
    ret = stop_capturing();
    if(ret < 0) {
        printf("Failed to stop capturing\n");
        return -1;
    }
    for(int i = 0; i < req_.count; i++) {
        ret = munmap(buffers_[i].start, buffers_[i].length);
        if(ret < 0) {
            printf("Failed to munmap buffer %d\n", i);
            return -1;
        }
    }
    delete[] buffers_;
    v4l2_close(fd_);
    return 0;
}

int v4l2_camera::start_preview()
{
    int ret;
    ret = start_capturing();
    if(ret < 0) {
        printf("Failed to start capturing\n");
        return -1;
    }
    return 0;
}

int v4l2_camera::preview(unsigned char *&data, size_t &size)
{
    do {
        FD_ZERO(&fds_);
        FD_SET(fd_, &fds_); // add fd to fd_set
        // Timeout.
        tv_.tv_sec = 2;
        tv_.tv_usec = 0;
        // select() will block until either data is available or timeout occurs
        r_ = select(fd_ + 1, &fds_, NULL, NULL, &tv_);
    } while ((r_ == -1 && (errno = EINTR)));
    if (r_ == -1)
        return -1;
    if (dequeue_buffer(buf_) < 0)
        return -1;
    size = buf_.bytesused;
    data = (unsigned char *)buffers_[buf_.index].start;
    if (queue_buffer(buf_, buf_.index) < 0)
        return -1;
    return 0;
}

int v4l2_camera::stop_preview()
{
    int ret;
    ret = stop_capturing();
    if(ret < 0) {
        printf("Failed to stop capturing\n");
        return -1;
    }
    return 0;
}

int v4l2_camera::capture(unsigned char *&data, size_t &size)
{
    start_capturing();
    do {
        FD_ZERO(&fds_);
        FD_SET(fd_, &fds_); // add fd to fd_set
        // Timeout.
        tv_.tv_sec = 2;
        tv_.tv_usec = 0;
        // select() will block until either data is available or timeout occurs
        r_ = select(fd_ + 1, &fds_, NULL, NULL, &tv_);
    } while ((r_ == -1 && (errno = EINTR)));
    if (r_ == -1)
        return -1;
    if (dequeue_buffer(buf_) < 0)
        return -1;
    size = buf_.bytesused;
    data = (unsigned char *)buffers_[buf_.index].start;
    stop_capturing();
    return 0;
}



void v4l2_camera::xioctl(int fh, int request, void *arg)
{
    int r;

    do {
        r = v4l2_ioctl(fh, request, arg);
    } while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

    if (r == -1) {
        fprintf(stderr, "error %d, %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int v4l2_camera::open_device()
{
    fd_ = v4l2_open(dev_name_, O_RDWR | O_NONBLOCK, 0);
    if (fd_ < 0) {
        perror("Cannot open device");
        return -1;
    }
    return 0;
}

int v4l2_camera::set_format(unsigned int width, unsigned int height, unsigned int pixelformat)
{
    fmt_.type = type_;
    fmt_.fmt.pix.width = width;
    fmt_.fmt.pix.height = height;
    fmt_.fmt.pix.pixelformat = pixelformat;
    fmt_.fmt.pix.field = V4L2_FIELD_ANY;

    xioctl(fd_, VIDIOC_S_FMT, &fmt_);

    if(fmt_.fmt.pix.pixelformat != pixelformat) {
        printf("Libv4l didn't accept pixel format. Can be unsupported by device\n");
        return -1;
    }

    if(fmt_.fmt.pix.width != width || fmt_.fmt.pix.height != height) {
        printf("Warning: driver is sending image at %dx%d\n", fmt_.fmt.pix.width, fmt_.fmt.pix.height);
    }

    return 0;
}

int v4l2_camera::request_buffers(int n_buffers)
{
    memset(&req_, 0, sizeof(req_));
    req_.count = n_buffers;
    req_.type = type_;
    req_.memory = memory_;

    xioctl(fd_, VIDIOC_REQBUFS, &req_);

    if(req_.count < n_buffers) {
        printf("Insufficient buffer memory on device\n");
        return -1;
    }

    return 0;
}

int v4l2_camera::query_buffer(struct v4l2_buffer &buf, int index)
{
    memset(&buf, 0, sizeof(buf));
    buf.type = type_;
    buf.memory = memory_;
    buf.index = index;

    xioctl(fd_, VIDIOC_QUERYBUF, &buf);

    return 0;
}

int v4l2_camera::queue_buffer(struct v4l2_buffer &buf, int index)
{
    memset(&buf, 0, sizeof(buf));
    buf.type = type_;
    buf.memory = memory_;
    buf.index = index;

    xioctl(fd_, VIDIOC_QBUF, &buf);

    return 0;
}

int v4l2_camera::dequeue_buffer(struct v4l2_buffer &buf)
{
    memset(&buf, 0, sizeof(buf));
    buf.type = type_;
    buf.memory = memory_;

    xioctl(fd_, VIDIOC_DQBUF, &buf);

    return 0;
}


int v4l2_camera::start_capturing()
{
    unsigned int i;
    enum v4l2_buf_type type;

    for(i = 0; i < req_.count; i++) {
        if(queue_buffer(buf_, i) < 0)
            return -1;
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd_, VIDIOC_STREAMON, &type);

    return 0;
}

int v4l2_camera::stop_capturing()
{
    type_ = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd_, VIDIOC_STREAMOFF, &type_);

    return 0;
}