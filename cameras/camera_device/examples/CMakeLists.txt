# V4L2 摄像头示例程序配置

# 查找OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# 创建 V4L2 摄像头示例程序

# 创建示例程序
add_executable(v4l2_camera_example v4l2_camera_example.cpp)

# 创建虚拟摄像头测试程序（独立，无依赖）
add_executable(virtual_camera_test virtual_camera_simple_test.cpp)

# 设置包含目录
target_include_directories(v4l2_camera_example
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# 链接库
target_link_libraries(v4l2_camera_example
    PRIVATE
    v4l2_camera
    ${OpenCV_LIBS}
)

# 虚拟摄像头测试程序无需额外链接库

# 添加执行权限（对于Unix系统）
if(UNIX)
    add_custom_command(TARGET v4l2_camera_example POST_BUILD
        COMMAND chmod +x $<TARGET_FILE:v4l2_camera_example>
        COMMENT "Adding execute permission to v4l2_camera_example"
        VERBATIM
    )
    add_custom_command(TARGET virtual_camera_test POST_BUILD
        COMMAND chmod +x $<TARGET_FILE:virtual_camera_test>
        COMMENT "Adding execute permission to virtual_camera_test"
        VERBATIM
    )
endif()

# 安装示例程序
install(TARGETS v4l2_camera_example virtual_camera_test virtual_camera_simple_test
    RUNTIME DESTINATION bin/examples
)
