#for macOS and Linux
BUILD_DIR := build

.PHONY: all build run-digital_postcard clean

all: build

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) --config Release

run-digital_postcard: build
	cd $(BUILD_DIR) && ./digital_postcard

clean:
	rm -rf $(BUILD_DIR)
