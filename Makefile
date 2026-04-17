RELEASE_CFLAGS := -static -s -O3 -Wall -Wextra -Werror # -Wpedantic
DEBUG_CFLAGS := -static -g -O0 -Wall -Wextra -Werror # -Wpedantic
SRCS := $(wildcard ./src/*.cpp)
RELEASE_OBJS := $(SRCS:./src/%.cpp=./obj/release/%.o)
DEBUG_OBJS := $(SRCS:./src/%.cpp=./obj/debug/%.o)

release: ./bin/PowerDMX
./bin/PowerDMX: $(RELEASE_OBJS) | ./lib/libusb ./musl
	@mkdir -p ./bin
	./musl/bin/g++ $(RELEASE_CFLAGS) -o $@ $^ ./lib/libusb/lib/libusb-1.0.a

debug: ./bin/PowerDMX_Debug
./bin/PowerDMX_Debug: $(DEBUG_OBJS) | ./lib/libusb ./musl
	@mkdir -p ./bin
	./musl/bin/g++ $(DEBUG_CFLAGS) -o $@ $^ ./lib/libusb/lib/libusb-1.0.a

./obj/release/%.o: ./src/%.cpp | ./lib/libusb ./musl
	@mkdir -p ./obj/release
	./musl/bin/g++ $(RELEASE_CFLAGS) -I"./lib/libusb/include" -c $< -o $@

./obj/debug/%.o: ./src/%.cpp | ./lib/libusb ./musl
	@mkdir -p ./obj/debug
	./musl/bin/g++ $(DEBUG_CFLAGS) -I"./lib/libusb/include" -c $< -o $@

./lib/libusb: | ./musl
	@mkdir -p ./lib
	@rm -rf ./lib/libusb_git 1>/dev/null 2>&1
	@echo Cloning LibUsb...
	@git clone https://github.com/libusb/libusb.git ./lib/libusb_git 1>/dev/null 2>&1
	@echo Building LibUsb...
	@cd ./lib/libusb_git && CC="$(abspath ./musl/bin/gcc)" CXX="$(abspath ./musl/bin/g++)" CFLAGS="$(RELEASE_CFLAGS)" ./bootstrap.sh
	@cd ./lib/libusb_git && CC="$(abspath ./musl/bin/gcc)" CXX="$(abspath ./musl/bin/g++)" CFLAGS="$(RELEASE_CFLAGS)" ./configure --host=x86_64-linux-musl --enable-static --disable-shared --disable-udev --prefix='$(abspath ./lib/libusb)'
	@cd ./lib/libusb_git && CC="$(abspath ./musl/bin/gcc)" CXX="$(abspath ./musl/bin/g++)" CFLAGS="$(RELEASE_CFLAGS)" make -j$(nproc)
	@cd ./lib/libusb_git && CC="$(abspath ./musl/bin/gcc)" CXX="$(abspath ./musl/bin/g++)" CFLAGS="$(RELEASE_CFLAGS)" make install
	@rm -rf ./lib/libusb_git 1>/dev/null 2>&1
	@rm -rf ./lib/libusb/lib/pkgconfig/ 1>/dev/null 2>&1
	@rm -f ./lib/libusb/lib/libasound.la 1>/dev/null 2>&1

./musl:
	@echo 'Downloading Musl...'
	@curl https://musl.cc/x86_64-linux-musl-native.tgz -o ./musl.tgz --progress-bar
	@echo 'Extracting Musl...'
	@tar -xvf ./musl.tgz 1>/dev/null 2>&1
	@rm ./musl.tgz 1>/dev/null 2>&1
	@mv ./x86_64-linux-musl-native ./musl 1>/dev/null 2>&1

clean:
	rm -rf ./bin ./obj

cleanall:
	rm -rf ./bin ./obj ./lib ./musl

.PHONY: release debug clean cleanall