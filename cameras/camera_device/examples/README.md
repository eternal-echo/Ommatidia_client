# V4L2摄像头示例程序

本目录包含多摄像头同步采集系统的示例程序，演示如何使用项目提供的摄像头设备抽象层。

## 示例程序列表

### 1. v4l2_camera_example.cpp
**真实摄像头捕获示例**

功能：
- 支持真实的V4L2摄像头设备
- 支持MJPEG和YUYV格式
- 使用OpenCV进行图像处理和保存
- 可配置分辨率和保存间隔
- 在图像上叠加时间戳和序列号

使用方法：
```bash
# 编译（需要OpenCV）
cd build && make v4l2_camera_example

# 运行
./v4l2_camera_example [选项] [设备路径]

# 示例
./v4l2_camera_example -w 1280 -h 720 -f MJPEG /dev/video0
```

参数说明：
- `-w WIDTH`: 设置宽度 (默认: 640)
- `-h HEIGHT`: 设置高度 (默认: 480)  
- `-f FORMAT`: 设置格式 (MJPEG 或 YUYV, 默认: MJPEG)
- `-o DIR`: 指定输出目录 (默认: output)
- `-i INTERVAL`: 保存图片的间隔(ms) (默认: 100)

### 2. virtual_camera_simple_test.cpp
**虚拟摄像头简化测试示例** (推荐)

功能：
- 完全独立，不依赖项目库
- 直接使用V4L2 API
- 专门测试虚拟V4L2摄像头驱动模块
- 设备能力验证
- 帧数据完整性检查
- 颜色循环功能测试
- 性能基准测试

使用方法：
```bash
# 首先加载虚拟摄像头内核模块
cd ../../drivers
sudo insmod virtual_v4l2.ko

# 编译测试程序（无需依赖）
g++ -std=c++17 -o virtual_camera_simple_test virtual_camera_simple_test.cpp

# 运行测试
./virtual_camera_simple_test [选项]

# 示例
./virtual_camera_simple_test -d /dev/video0 -o test_output -v
```

### 3. virtual_camera_test.cpp
**虚拟摄像头完整测试示例**

功能：
- 专门测试虚拟V4L2摄像头驱动模块
- 设备能力验证
- 帧数据完整性检查
- 颜色循环功能测试
- 性能基准测试
- 无需OpenCV依赖

使用方法：
```bash
# 首先加载虚拟摄像头内核模块
cd ../drivers
sudo insmod virtual_v4l2.ko

# 编译测试程序
cd ../build && make virtual_camera_test

# 运行测试
./virtual_camera_test [选项]

# 示例
./virtual_camera_test -d /dev/video0 -o test_output -v
```

参数说明：
- `-d DEVICE`: 指定设备路径 (默认: /dev/video0)
- `-o DIR`: 指定输出目录 (默认: virtual_camera_output)
- `-v`: 详细输出模式

测试内容：
1. **设备能力测试**: 验证分辨率、格式等参数
2. **帧捕获测试**: 验证数据读取和颜色循环
3. **性能基准测试**: 测量读取延迟和理论帧率
4. **数据完整性测试**: 保存PPM格式图像进行验证

## 构建说明

### 前置条件

1. **系统依赖**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake
   sudo apt-get install libopencv-dev  # 仅v4l2_camera_example需要
   ```

2. **虚拟摄像头模块** (仅virtual_camera_test需要):
   ```bash
   cd ../drivers
   make
   sudo insmod virtual_v4l2.ko
   ```

### 编译步骤

```bash
# 从项目根目录
cd cameras/build

# 配置CMake
cmake ..

# 编译所有示例
make

# 或编译特定示例
make v4l2_camera_example
make virtual_camera_test
```

### 运行测试

```bash
# 运行虚拟摄像头测试
./examples/virtual_camera_test -v

# 运行真实摄像头示例
./examples/v4l2_camera_example /dev/video1
```

## 输出说明

### v4l2_camera_example输出
- 保存带时间戳的JPEG图像文件
- 文件命名格式: `frame_YYYYMMDD_HHMMSS_UUUUUU_seqNNNNNN.jpg`
- 图像上叠加时间戳和序列号信息

### virtual_camera_test输出
- 控制台显示详细测试结果
- 保存PPM格式的测试图像
- 性能统计信息
- 颜色循环验证结果

## 故障排除

### 常见问题

1. **设备权限问题**:
   ```bash
   sudo chmod 666 /dev/video*
   # 或将用户添加到video组
   sudo usermod -a -G video $USER
   ```

2. **虚拟摄像头模块未加载**:
   ```bash
   lsmod | grep virtual_v4l2
   # 如果没有输出，需要加载模块
   cd ../drivers && sudo insmod virtual_v4l2.ko
   ```

3. **OpenCV未找到**:
   ```bash
   sudo apt-get install libopencv-dev
   # 或使用conda
   conda install opencv
   ```

4. **编译错误**:
   ```bash
   # 清理重新编译
   cd build && make clean && cmake .. && make
   ```

### 调试模式

启用详细输出查看更多调试信息：
```bash
./virtual_camera_test -v
```

## 集成到项目

这些示例程序展示了如何：

1. **使用摄像头设备抽象层**: 通过`icamera_device`接口统一访问不同类型的摄像头
2. **处理帧缓冲**: 使用`buffer`类管理图像数据和元数据
3. **实现设备初始化**: 正确的设备打开、配置和关闭流程
4. **进行错误处理**: 健壮的错误检查和资源清理
5. **性能优化**: 高效的数据传输和处理

可以基于这些示例开发自己的摄像头应用程序。
