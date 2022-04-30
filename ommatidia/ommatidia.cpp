#include "ommatidia.hpp"

Ommatidia::Ommatidia()
{

}

Ommatidia::~Ommatidia()
{
    deinit();
}

int Ommatidia::init(const char *host, int port)
{
    int ret;

    ret = client_.init(host, port);
    if(ret < 0) {
        std::cout<< "Failed to initialize client" << std::endl;
        return -1;
    }

    cameras_.push_back(new v4l2_camera("/dev/video0"));
    cameras_.push_back(new v4l2_camera("/dev/video2"));
    for(auto camera : cameras_) {
        ret = camera->init();
        if(ret < 0) {
            std::cout<< "Failed to initialize camera" << std::endl;
            return -1;
        }
    }
    unsigned char cameras_num = cameras_.size();
    client_.send(&cameras_num, 1);

    preview_thread_ = std::thread(&Ommatidia::thread_preview, this);
    preview_thread_.detach();

    state_ = &Ommatidia::ctrl;
    return 0;
}

int Ommatidia::deinit()
{
    for(auto &camera : cameras_) {
        camera->deinit();
    }
    cameras_.clear();
    client_.deinit();
}

int Ommatidia::run()
{
    int ret;
    while(1) {
        if(state_!=nullptr) {
            ret = (this->*state_)();
        }
        else
        {
            state_ = &Ommatidia::ctrl;
        }
    }
    return 0;
}

int Ommatidia::ctrl()
{
    std::cout << "ctrl" << std::endl;
    int ret;
    unsigned char cmd;
    client_.recv(&cmd, 1);
    switch (cmd)
    {
    case 'p':
        state_ = &Ommatidia::preview;
        std::cout << "preview cmd" << std::endl;
        break;
    case 'c':
        state_ = &Ommatidia::capture;
        break;
    case 's':
        state_ = &Ommatidia::stop_preview;
        break;
    default:
        state_ = &Ommatidia::ctrl;
        break;
    }
}

int Ommatidia::thread_preview()
{
    int ret;
    while(1) {
        std::unique_lock<std::mutex> lk(camera_mutex_);
        camera_cond_.wait(lk, [this]{return is_preview_;});
        lk.unlock();

        std::cout << "preview thread" << std::endl;
        unsigned char *data = nullptr;
        size_t length = 0;
        cameras_[0]->start_preview();
        while(is_preview_)
        {
            ret = cameras_[0]->preview(data, length);
            client_.send(data, length);
            if(ret < 0) {
                    std::cout<< "Failed to get frame" << std::endl;
                    continue;
            }
        }
        cameras_[0]->stop_preview();
    }
    return 0;
}

int Ommatidia::preview()
{
    std::unique_lock<std::mutex> lk(camera_mutex_);
    is_preview_ = true;
    camera_cond_.notify_one();
    lk.unlock();

    state_ = &Ommatidia::ctrl;
    std::cout << "Preview started" << std::endl;
    return 0;
}

int Ommatidia::stop_preview()
{
    std::unique_lock<std::mutex> lk(camera_mutex_);
    is_preview_ = false;
    camera_cond_.notify_one();
    lk.unlock();

    state_ = &Ommatidia::ctrl;
    std::cout << "Preview stopped" << std::endl;
    return 0;
}

int Ommatidia::capture()
{
    std::cout << "Capture started" << std::endl;
    std::unique_lock<std::mutex> lk(camera_mutex_);
    if(is_preview_) {
        is_preview_ = false;
        camera_cond_.notify_one(); // TDO:need?
    }

    int ret;
    std::vector<Buffer<unsigned char>> buffers(cameras_.size());
    std::vector<std::future<int>> futures;
    do {
        // clear the receive buffer in tcp_client
        client_.clear_recv_buffer();
        // async capture in all cameras
        for(int i = 0; i < cameras_.size(); i++) {
            std::future<int> future = std::async(&v4l2_camera::capture, cameras_[i], std::ref(buffers[i].start), std::ref(buffers[i].length));
            futures.push_back(std::move(future));
            //cameras_[i].preview(buffers[i].start, buffers[i].length);
        }
        for(auto &future : futures) {
            future.get();
        }
        futures.clear();
        for(char i = 0; i < cameras_.size(); i++) {
            JPEG_data jpeg_data(buffers[i].start, buffers[i].length);
            if(!jpeg_data.is_valid()) {
                std::cout<< "Invalid jpeg data, try again" << std::endl;
                ret = -1;
                break;
            }
            std::cout << "send jpeg data, length: " <<  buffers[i].length << std::endl;
            ret = client_.send(buffers[i].start, buffers[i].length);
            if(ret < 0) {
                std::cout<< "Failed to send jpeg data, try again" << std::endl;
                break;
            }
            // waiting for ack from server, make sure the server has received the jpeg data
            char acks[2];
            ret = client_.recv(acks, 2);
            if(ret < 0) {
                std::cout<< "Failed to recv ack, try again" << std::endl;
                break;
            }
            if(acks[0] != 'y' || acks[1] != i) {
                std::cout<< "Invalid ack, try again" << std::endl;
                ret = -1;
                break;
            }
        }
    } while(ret < 0); // retry if failed
        
    state_ = &Ommatidia::ctrl;
}