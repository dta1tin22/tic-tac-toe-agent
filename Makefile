SOURCE_DIR ?= src
INCLUDE_DIR ?= include
BIN_DIR ?= bin
BUILD_DIR ?= obj

SOURCE_CPP := $(shell find $(SOURCE_DIR) -type f -name "*.cpp")
SOURCE_HPP := $(shell find $(INCLUDE_DIR) -type f -name "*.hpp")
DEPS := $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.d,$(SOURCE_CPP))
OBJECTS := $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCE_CPP))
TARGET ?= $(BIN_DIR)/tic_tac_toe_agent

CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -O2 -I$(INCLUDE_DIR) -MMD -MP

.PHONY: all build format clean help

all: format build

build: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

-include $(DEPS)

format:
	clang-format --style=file -i $(SOURCE_CPP) $(SOURCE_HPP)

format-check:
	clang-format --style=file --dry-run --Werror $(SOURCE_CPP) $(SOURCE_HPP)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all            - Format code and build the project (default)"
	@echo "  build          - Build the project"
	@echo "  format         - Format source code using clang-format"
	@echo "  format-check   - Check if source code is formatted correctly"
	@echo "  clean          - Remove build artifacts"
