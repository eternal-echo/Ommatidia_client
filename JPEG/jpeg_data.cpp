#include "jpeg_data.hpp"

JPEG_data::JPEG_data()
{
    data_ = nullptr;
    size_ = 0;
    width_ = 0;
    height_ = 0;
    channels_ = 0;
}

JPEG_data::JPEG_data(unsigned char *data, size_t size)
{
    data_ = data;
    size_ = size;
    width_ = 0;
    height_ = 0;
    channels_ = 0;
}

JPEG_data::~JPEG_data()
{
    
}

void JPEG_data::set_data(unsigned char *data, size_t size)
{
    data_ = data;
    size_ = size;
    width_ = 0;
    height_ = 0;
    channels_ = 0;
}

void JPEG_data::set_data(unsigned char *data, size_t size, int width, int height)
{
    data_ = data;
    size_ = size;
    width_ = width;
    height_ = height;
    channels_ = 0;
}

void JPEG_data::set_data(unsigned char *data, size_t size, int width, int height, int channels)
{
    data_ = data;
    size_ = size;
    width_ = width;
    height_ = height;
    channels_ = channels;
}

int JPEG_data::get_size()
{
    return size_;
}

int JPEG_data::get_width()
{
    return width_;
}

int JPEG_data::get_height()
{
    return height_;
}

int JPEG_data::get_channels()
{
    return channels_;
}

bool JPEG_data::is_valid()
{
    if(data_[0] == 0xff && data_[1] == 0xd8 && data_[size_ - 2] == 0xff && data_[size_ - 1] == 0xd9)
    {
        return true;
    }
    else
    {
        return false;
    }
}
