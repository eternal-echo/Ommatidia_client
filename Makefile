
build: main.cpp v4l2_camera/v4l2_camera.cpp tcp_client/tcp_client.cpp ommatidia/ommatidia.cpp JPEG/jpeg_data.cpp
	arm-linux-gnueabihf-g++ main.cpp v4l2_camera/v4l2_camera.cpp tcp_client/tcp_client.cpp ommatidia/ommatidia.cpp JPEG/jpeg_data.cpp -l v4l2 -pthread -o build/main -g

run:
	./build/main

debug:
	gdb ./build/main

clean:
	rm -f build/main photo/*.jpg