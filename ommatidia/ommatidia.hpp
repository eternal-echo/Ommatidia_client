#ifndef __OMMATIDIA_HPP__
#define __OMMATIDIA_HPP__

#include "../v4l2_camera/v4l2_camera.hpp"
#include "../tcp_client/tcp_client.hpp"
#include "../JPEG/jpeg_data.hpp"
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <future>
#include <functional>
#include <algorithm>


class Ommatidia
{
public:
    Ommatidia();
    ~Ommatidia();
    int init(const char *host = "192.163.43.169", int port = 8000);
    int deinit();
    int run();
    int ctrl();
private:
    int capture();
    int preview();
    int stop_preview();
    int thread_preview();
    std::thread preview_thread_;
    bool is_preview_ = false;
    std::mutex camera_mutex_;
    std::condition_variable camera_cond_;
    int (Ommatidia::*state_)() = nullptr;
    std::vector<v4l2_camera*> cameras_ = {};
    tcp_client client_ = {};
};

#endif