UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
	CC := $(shell which clang)
    CXX := $(shell which clang++)
	RELEASE_CFLAGS := -framework CoreFoundation -framework IOKit -framework Security -lobjc -Wno-error=unused-command-line-argument -std=c++23 -O3 -Wall -Wextra -Werror # -Wpedantic
	DEBUG_CFLAGS := -framework CoreFoundation -framework IOKit -framework Security -lobjc -Wno-error=unused-command-line-argument -std=c++23 -g -O0 -Wall -Wextra -Werror # -Wpedantic
else
	CC := $(abspath ./musl/bin/gcc)
    CXX := $(abspath ./musl/bin/g++)
	RELEASE_CFLAGS := -static -std=c++23 -O3 -Wall -Wextra -Werror # -Wpedantic
	DEBUG_CFLAGS := -static -std=c++23 -g -O0 -Wall -Wextra -Werror # -Wpedantic
endif

SRCS := $(wildcard ./src/*.cpp)
RELEASE_OBJS := $(SRCS:./src/%.cpp=./obj/release/%.o)
DEBUG_OBJS := $(SRCS:./src/%.cpp=./obj/debug/%.o)

release: ./bin/PowerDMX
./bin/PowerDMX: $(RELEASE_OBJS) | ./lib/libusb ./musl
	@mkdir -p ./bin
	$(CXX) $(RELEASE_CFLAGS) -o $@ $^ ./lib/libusb/lib/libusb-1.0.a
	@strip $@

debug: ./bin/PowerDMX_Debug
./bin/PowerDMX_Debug: $(DEBUG_OBJS) | ./lib/libusb ./musl
	@mkdir -p ./bin
	$(CXX) $(DEBUG_CFLAGS) -o $@ $^ ./lib/libusb/lib/libusb-1.0.a

./obj/release/%.o: ./src/%.cpp | ./lib/libusb ./musl
	@mkdir -p ./obj/release
	$(CXX) $(RELEASE_CFLAGS) -I"./lib/libusb/include" -c $< -o $@

./obj/debug/%.o: ./src/%.cpp | ./lib/libusb ./musl
	@mkdir -p ./obj/debug
	$(CXX) $(DEBUG_CFLAGS) -I"./lib/libusb/include" -c $< -o $@

./lib/libusb: | ./musl
	@mkdir -p ./lib
	@rm -rf ./lib/libusb_git 1>/dev/null 2>&1
	@echo Cloning LibUsb...
	@git clone https://github.com/libusb/libusb.git ./lib/libusb_git 1>/dev/null 2>&1
	@echo Building LibUsb..
	@cd ./lib/libusb_git && ./bootstrap.sh 1>/dev/null 2>&1
ifeq ($(UNAME), Darwin)
	cd ./lib/libusb_git && CC="$(CC)" CXX="$(CXX)" CFLAGS="" ./configure --enable-static --disable-shared --prefix='$(abspath ./lib/libusb)'
	cd ./lib/libusb_git && CC="$(CC)" CXX="$(CXX)" CFLAGS="" make -j$(shell sysctl -n hw.logicalcpu)
else
	@cd ./lib/libusb_git && CC="$(CC)" CXX="$(CXX)" CFLAGS="" ./configure --host=x86_64-linux-musl --enable-static --disable-shared --disable-udev --prefix='$(abspath ./lib/libusb)' 1>/dev/null 2>&1
	@cd ./lib/libusb_git && CC="$(CC)" CXX="$(CXX)" CFLAGS="" make -j$(shell nproc) 1>/dev/null 2>&1
endif
	@cd ./lib/libusb_git && make install 1>/dev/null 2>&1
	@rm -rf ./lib/libusb_git 1>/dev/null 2>&1
	@rm -rf ./lib/libusb/lib/pkgconfig/ 1>/dev/null 2>&1
	@rm -f ./lib/libusb/lib//libusb-1.0.la 1>/dev/null 2>&1

./musl:
ifeq ($(UNAME), Darwin)
	@mkdir -p ./musl
else
	@echo 'Downloading Musl...'
	@curl https://musl.cc/x86_64-linux-musl-native.tgz -o ./musl.tgz --progress-bar
	@echo 'Extracting Musl...'
	@tar -xvf ./musl.tgz 1>/dev/null 2>&1
	@rm ./musl.tgz 1>/dev/null 2>&1
	@mv ./x86_64-linux-musl-native ./musl 1>/dev/null 2>&1
endif

clean:
	rm -rf ./bin ./obj

cleanall:
	rm -rf ./bin ./obj ./lib ./musl

.PHONY: release debug clean cleanall