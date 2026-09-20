#for macOS and Linux
BUILD_DIR := build

.PHONY: all build run-red_triangle run-blue_square run-polygons_part1 run-polygons_part2 clean

all: build

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) --config Release

run-red_triangle: build
	cd $(BUILD_DIR) && ./red_triangle

run-blue_square: build
	cd $(BUILD_DIR) && ./blue_square

run-polygons_part1: build
	cd $(BUILD_DIR) && ./polygons_part1

run-polygons_part2: build
	cd $(BUILD_DIR) && ./polygons_part2

clean:
	rm -rf $(BUILD_DIR)
