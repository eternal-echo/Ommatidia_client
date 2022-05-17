#include "./ommatidia/ommatidia.hpp"
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


std::string now()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm); 
    return buf;
}


int main(int argc, char **argv)
{
    std::vector<v4l2_camera*> cameras;
    Ommatidia ommatidia;
    int ret;
    // add cameras to ommatidia 
    ret = ommatidia.init("192.168.43.169", 8000);
    if(ret < 0) {
        std::cout<< "Failed to initialize ommatidia" << std::endl;
        return -1;
    }
    std::cout<< "Ommatidia initialized, start running" << std::endl;
    // start ommatidia
    ret = ommatidia.run();
    if(ret < 0) {
        std::cout<< "Failed to run ommatidia" << std::endl;
        return -1;
    }
    return 0;
}