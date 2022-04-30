#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <libv4l2.h>

template <typename T>
class Buffer {
public:
    T *start;
    size_t length;
};

class v4l2_camera
{
public:
    v4l2_camera(const char *dev_name = "/dev/video0");
    ~v4l2_camera();
    int init(const char *dev_name, int width = 640, int height = 480, int format = V4L2_PIX_FMT_MJPEG);
    int init(int width = 640, int height = 480, int format = V4L2_PIX_FMT_MJPEG);
    int deinit();
    int start_preview();
    int stop_preview();
    /**
     * @brief Get a frame from camera.
     * Dequeue a buffer from outcoming queue and queue it to incoming queue.
     * It's used to circularly get frames from camera. (preview mode)
     * It's faster than capture()
     * 
     * @param data frame data
     * @param size frame size
     * @return int 
     */
    int preview(unsigned char *&data, size_t &size);
    /**
     * @brief Get a photo in present time.
     * Dequeue all buffers from outcoming queue to clear the buffers.
     * Then queue a new buffer to incoming queue, and waiting in incoming queue.
     * Make sure that we can get a photo in present time.(capture mode)
     * It's slower than preview(), but it's more accurate.
     * 
     * @param data 
     * @param size 
     * @return int 
     */
    int capture(unsigned char *&data, size_t &size);
private:
    void xioctl(int fh, int request, void *arg);
    /**
     * @brief open_device
     *
     * @return int 0 on success, -1 on error
     */
    int open_device();
    /**
     * @brief Set the format object
     *
     * @param width
     * @param height
     * @param pixelformat
     * @return int 0 on success, -1 on error
     */
    int set_format(unsigned int width, unsigned int height, unsigned int pixelformat);
    int request_buffers(int n_buffers);
    int query_buffer(struct v4l2_buffer &buf, int index);
    int queue_buffer(struct v4l2_buffer &buf, int index);
    int dequeue_buffer(struct v4l2_buffer &buf);
    int start_capturing();
    int stop_capturing();

    struct v4l2_format fmt_;
    struct v4l2_buffer buf_;
    struct v4l2_requestbuffers req_;
    enum v4l2_buf_type type_ = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    enum v4l2_memory memory_ = V4L2_MEMORY_MMAP;
    fd_set fds_;
    struct timeval tv_;
    int r_, fd_ = -1;
    const char *dev_name_ = "/dev/video0";
    char out_name_[256];
    FILE *fout_;
    Buffer<void> *buffers_;
};