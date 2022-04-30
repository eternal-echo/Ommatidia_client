#ifndef __JPEG_DATA_HPP__
#define __JPEG_DATA_HPP__

#include <unistd.h>

class JPEG_data
{
public:
    JPEG_data();
    JPEG_data(unsigned char *data, size_t size);
    ~JPEG_data();

    void set_data(unsigned char *data, size_t size);
    void set_data(unsigned char *data, size_t size, int width, int height);
    void set_data(unsigned char *data, size_t size, int width, int height, int channels);

    int get_size();
    int get_width();
    int get_height();
    int get_channels();

    bool is_valid();

private:
    unsigned char *data_;
    size_t size_;
    int width_;
    int height_;
    int channels_;
};


#endif