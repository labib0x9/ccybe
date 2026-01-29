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

LLHTTP_DIR := $(THIRD_PARTY)/llhttp/build
LLHTTP_INC := $(LLHTTP_DIR)
LLHTTP_LIB := $(LLHTTP_DIR)/libllhttp.a

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
	-I$(SRC_DIR) \
	-I$(LLHTTP_INC) \
	-I$(KLIB_INC)

DEBUG_FLAGS := -g -O0 -DDEBUG

LDFLAGS :=

# -------------------------
# Sources
# -------------------------

SRCS := \
	main.c \
	$(wildcard $(SRC_DIR)/*.c) \
	$(wildcard $(SRC_DIR)/ds/*.c) \
	$(wildcard $(SRC_DIR)/url/*.c)

# 	route_handler.c \

OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# -------------------------
# Default Target
# -------------------------

all: $(TARGET)

# -------------------------
# Link
# -------------------------

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LLHTTP_LIB) -o $@
	@echo "✔ Built $(TARGET)"

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
# Run
# -------------------------

run: $(TARGET)
	./$(TARGET)

# -------------------------
# Dependencies
# -------------------------

-include $(DEPS)

.PHONY: all clean debug
