BUILD_DIR_PREFIX := build
BUILD_TYPE := $(if $(BUILD_TYPE),$(BUILD_TYPE),Debug)
TEST_LOG_FILE := test.log
BUILD_DIR := "$(BUILD_DIR_PREFIX)/$(BUILD_TYPE)"

HALF_THREADS := $$(expr `nproc --all` / 2)
NUMBER_OF_THREADS := $$(($(HALF_THREADS)>0 ? $(HALF_THREADS) : 1))

PROFILE_EXECUTABLE := "$(BUILD_DIR)/BSG_CLP"
PROFILE_EXECUTABLE_ARGS := problems/clp/benchs/BR/BR15.txt -i 0 --min_fr=0.98 --timelimit=30 --alpha=4 --beta=1 --gamma=0.2 -p0.04 -fBR --seed=42
FREQUENCY := 1000
PERF_RECORD_OUTPUT := "perf.record.out"
PERF_SCRIPT_OUTPUT := "perf.script.out"
PERF_REPORT_OUTPUT := "perf.report.out"

.PHONY: build test profile clean

build:
	cmake . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build $(BUILD_DIR) -- -j $(NUMBER_OF_THREADS)

test: build
	ctest --test-dir $(BUILD_DIR) -j $(NUMBER_OF_THREADS) --output-log $(TEST_LOG_FILE) --output-on-failure

profile: build
	perf record -F $(FREQUENCY) -g -o $(PERF_RECORD_OUTPUT) -- $(PROFILE_EXECUTABLE) $(PROFILE_EXECUTABLE_ARGS)
	perf script -i $(PERF_RECORD_OUTPUT) > $(PERF_SCRIPT_OUTPUT)
	perf report -i $(PERF_RECORD_OUTPUT) --stdio --call-graph none > $(PERF_REPORT_OUTPUT)
	@echo "go to <https://thlorenz.com/flamegraph/web/> and upload the file '$(PERF_SCRIPT_OUTPUT)'"

clean:
	rm -rf $(BUILD_DIR) .cache/ *.log
