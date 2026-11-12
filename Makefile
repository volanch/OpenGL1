BUILD_DIR := build

.PHONY: all build run-part1 run-part2 run-part3a run-part3b clean

all: build

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) --config Release

run-part1: build
	cd $(BUILD_DIR) && ./part1_bezier

run-part2: build
	cd $(BUILD_DIR) && ./part2_picking

run-part3a: build
	cd $(BUILD_DIR) && ./part3a_texture_patch

run-part3b: build
	cd $(BUILD_DIR) && ./part3b_procedural

clean:
	rm -rf $(BUILD_DIR)
