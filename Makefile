BUILD_DIR := build

.PHONY: all build clean run-part1 run-part2

all: build
	@cp -f $(BUILD_DIR)/part1 . 2>/dev/null || cp -f $(BUILD_DIR)/part1.exe . 2>/dev/null || true
	@cp -f $(BUILD_DIR)/part2 . 2>/dev/null || cp -f $(BUILD_DIR)/part2.exe . 2>/dev/null || true

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) --config Release

run-part1: all
	./part1

run-part2: all
	./part2

clean:
	rm -rf $(BUILD_DIR) part1 part1.exe part2 part2.exe
