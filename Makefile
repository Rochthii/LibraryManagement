# Makefile for LibraryManagement (Windows / MSYS / MinGW)
# Usage:
#   make            -> builds bin/main.exe
#   make run        -> builds if needed then runs the program
#   make clean      -> remove build artifacts
#
# Customize by overriding variables on the make command line, e.g.
#   make CXX=g++ SFML_DIR=/e/Code/libs/SFML

ROOT := $(CURDIR)
SFML_DIR ?= $(ROOT)/lib/SFML
CXX ?= g++

INCLUDE_DIRS := -Iinclude -I"$(SFML_DIR)/include"
LIB_DIRS := -L"$(SFML_DIR)/lib"
SFML_LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
EXTRA_LIBS := -lgdi32 -luser32 -lole32 -loleaut32 -lcomdlg32 -luuid

CXXFLAGS := -std=c++17 -Wall -Wextra -g $(INCLUDE_DIRS)
LDFLAGS := $(LIB_DIRS) $(SFML_LIBS) $(EXTRA_LIBS)

SRCS := \
	main.cpp \
	GiaoDien.cpp \
	data/KiemTraDuLieu.cpp \
	data/QuanLySach.cpp \
	data/ThaoTacFile.cpp \
	data/NhapLieu.cpp \
	utils/ThongBao.cpp \
	utils/NgayThang.cpp \
	utils/XuLyChuoi.cpp

# Create object paths under build/objs preserving subdirectories
OBJS := $(patsubst %.cpp, build/objs/%.o, $(SRCS))
TARGET := bin/main.exe

.PHONY: all build run clean rebuild

all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	@echo Linking $@
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "-> $@"

# Compile each source to its corresponding object file
build/objs/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo Compiling $<
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Convenience target identical to all
build: all

# Run target: ensures binary exists, copies SFML dlls if present, then runs
run: $(TARGET)
	@echo "Preparing runtime..."
	@if [ -d "$(SFML_DIR)/bin" ]; then \
		mkdir -p bin && cp -f "$(SFML_DIR)/bin"/*.dll bin/ 2>/dev/null || true; \
		echo "Copied SFML DLLs from $(SFML_DIR)/bin to bin/"; \
	fi
	@echo "Running $(TARGET)"
	@"$(TARGET)"

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf build/objs || true
	@rm -f $(TARGET)

rebuild: clean all
	@echo "Rebuild done"

# Print helpful info
info:
	@echo "CXX      = $(CXX)"
	@echo "SFML_DIR = $(SFML_DIR)"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"

# --- Test Build Rules ---
# Sources for the test build
TEST_SRCS := \
	test.cpp \
	data/KiemTraDuLieu.cpp \
	data/QuanLySach.cpp \
	data/ThaoTacFile.cpp \
	data/NhapLieu.cpp \
	utils/ThongBao.cpp \
	utils/NgayThang.cpp \
	utils/XuLyChuoi.cpp

# Object files for the test build
TEST_OBJS := $(patsubst %.cpp, build/objs/%.test.o, $(TEST_SRCS))
TEST_TARGET := bin/test.exe

.PHONY: test run-test

# Target to build the test executable
test: $(TEST_TARGET)

# Link step for the test executable
$(TEST_TARGET): $(TEST_OBJS)
	@mkdir -p $(dir $@)
	@echo "Linking Test Target: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "-> Successfully created test executable $@"

# Rule to compile source files for the test build into separate .test.o objects
build/objs/%.test.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling for test: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Target to run the tests
run-test: test
	@echo "--- Running Tests ---"
	@"$(TEST_TARGET)"
	@echo "--- Tests Finished ---"
