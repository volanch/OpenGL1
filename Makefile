#for macOS and Linux
BUILD_DIR := build

.PHONY: all build run-robot_controller clean

all: build

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) --config Release

run-robot_controller: build
	cd $(BUILD_DIR) && ./robot_controller

clean:
	rm -rf $(BUILD_DIR)
