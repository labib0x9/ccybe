# -------------------------
# Project Configuration
# -------------------------

TARGET      := server
SRC_DIR     := src
BUILD_DIR   := build
THIRD_PARTY := third_party

# -------------------------
# llhttp Configuration
# -------------------------

LLHTTP_DIR := $(THIRD_PARTY)/llhttp
LLHTTP_BUILD := $(LLHTTP_DIR)/build
LLHTTP_INC := $(LLHTTP_DIR)/include
LLHTTP_LIB := $(LLHTTP_BUILD)/libllhttp.a

# -------------------------
# khash Configuration (header-only)
# -------------------------

KLIB_DIR  := $(THIRD_PARTY)/klib
KLIB_INC  := $(KLIB_DIR)

# -------------------------
# Compiler
# -------------------------

CC := gcc

# -------------------------
# Flags
# -------------------------

CFLAGS := \
	-Wall -Wextra -Werror \
	-std=c11 -O2 \
	-D_POSIX_C_SOURCE=200809L \
	-I$(SRC_DIR) \
	-I$(LLHTTP_INC) \
	-I$(KLIB_INC)
DEBUG_FLAGS := -g -O0 -DDEBUG
LDFLAGS := -fsanitize=address

# -------------------------
# Sources
# -------------------------

SRCS := \
	main.c \
	$(filter-out $(SRC_DIR)/http_kqueue.c $(SRC_DIR)/http_epoll.c, $(wildcard $(SRC_DIR)/*.c)) \
	$(wildcard $(SRC_DIR)/ds/*.c) \
	$(wildcard $(SRC_DIR)/url/*.c)
# 	$(wildcard $(SRC_DIR)/*.c) \

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    SRCS += src/http_kqueue.c
else
    SRCS += src/http_epoll.c
endif
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# -------------------------
# Default Target
# -------------------------

all: $(LLHTTP_LIB) $(TARGET)

# -------------------------
# Build llhttp
# -------------------------
# $(LLHTTP_LIB):
# 	@echo "Building llhttp..."
# 	cd $(LLHTTP_DIR) && rm -rf build && cmake -Bbuild -DCMAKE_BUILD_TYPE=Release && cmake --build build
# 	@echo "✔ llhttp built"

$(LLHTTP_LIB):
	@echo "Building llhttp..."
	cd $(LLHTTP_DIR) && rm -rf build && cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON && cmake --build build
	@echo "✔ llhttp built"

# -------------------------
# Link
# -------------------------

ifeq ($(UNAME), Darwin)
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LLHTTP_LIB) -o $@
	@echo "✔ Built $(TARGET)"
else
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -Wl,--start-group $(LLHTTP_LIB) -Wl,--end-group -o $@
	@echo "✔ Built $(TARGET)"
endif

# -------------------------
# Compile
# -------------------------

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# -------------------------
# Debug
# -------------------------

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all

# -------------------------
# Clean
# -------------------------

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# -------------------------
# Clean all (including llhttp)
# -------------------------
distclean: clean
	rm -rf $(LLHTTP_BUILD)

# -------------------------
# Run
# -------------------------

# run: $(TARGET)
# 	./$(TARGET)

run: all
	./$(TARGET)

# -------------------------
# Dependencies
# -------------------------

-include $(DEPS)
.PHONY: all clean distclean debug run
