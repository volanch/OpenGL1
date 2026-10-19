BUILD_DIR := build
MODEL ?= bound-lo-sphere.smf

.PHONY: all build clean run

all: build
	@cp -f $(BUILD_DIR)/assignment3 . 2>/dev/null || cp -f $(BUILD_DIR)/assignment3.exe . 2>/dev/null || true
	@mkdir -p shaders && cp -f "Resource Files/Shaders"/* shaders/

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) --config Release

run: all
	./assignment3 $(MODEL)

clean:
	rm -rf $(BUILD_DIR) shaders assignment3 assignment3.exe
