.PHONY: all
CC := g++
EXE := test

VCPKG := /home/zaxtyson/vcpkg/packages
FMT_HEADER := $(VCPKG)/fmt_x64-linux/include
SPDLOG_HEADER := $(VCPKG)/spdlog_x64-linux/include
FMT_LIB := $(VCPKG)/fmt_x64-linux/lib
SPDLOG_LIB = $(VCPKG)/spdlog_x64-linux/lib

SRC := example.cpp

all:
	@echo "starting building..."
	$(CC) -o $(EXE) $(SRC) \
	-I$(FMT_HEADER) -I$(SPDLOG_HEADER) \
	-L$(FMT_LIB) -L$(SPDLOG_LIB) \
	-lspdlog -lfmt -lpthread