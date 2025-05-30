BUILD_DIR_PREFIX := build
BUILD_TYPE := $(if $(BUILD_TYPE),$(BUILD_TYPE),Debug)
TEST_LOG_FILE := test.log
BUILD_DIR := "$(BUILD_DIR_PREFIX)/$(BUILD_TYPE)"

HALF_THREADS := $$(expr `nproc --all` / 2)
NUMBER_OF_THREADS := $$(($(HALF_THREADS)>0 ? $(HALF_THREADS) : 1))

.PHONY: build test profile clean

build:
	cmake . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build $(BUILD_DIR) -- -j $(NUMBER_OF_THREADS)

clean:
	rm -rf $(BUILD_DIR) .cache/ *.log
