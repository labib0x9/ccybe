# -------------------------
#  Project Configuration -> AI Generated
# -------------------------

TARGET := server

SRC_DIR := src
BUILD_DIR := build

# -------------------------
#  llhttp Configuration
# -------------------------

# LLHTTP_DIR := third_party/llhttp
# LLHTTP_INC := $(LLHTTP_DIR)
# LLHTTP_LIB := $(LLHTTP_DIR)/libllhttp.a

LLHTTP_DIR := third_party/llhttp/build
LLHTTP_INC := $(LLHTTP_DIR)
LLHTTP_LIB := $(LLHTTP_DIR)/libllhttp.a

# -------------------------
#  Source Files
# -------------------------

SRCS := main.c route_handler.c $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)

# -------------------------
#  Compiler & Flags
# -------------------------

CC := gcc

CFLAGS := \
	-Wall -Wextra -Werror -std=c11 -O2 \
	-I$(SRC_DIR) \
	-I$(LLHTTP_INC)

LDFLAGS :=

DEBUG_FLAGS := -g -O0

# -------------------------
#  Default Rule
# -------------------------

all: $(TARGET)

# -------------------------
#  Link
# -------------------------

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LLHTTP_LIB) -o $@
	@echo "✔ Build complete → ./$(TARGET)"

# -------------------------
#  Compile
# -------------------------

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# -------------------------
#  Build Directory
# -------------------------

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# -------------------------
#  Debug Build
# -------------------------

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all

# -------------------------
#  Clean
# -------------------------

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# -------------------------
#  Dependency Includes
# -------------------------

-include $(OBJS:.o=.d)

.PHONY: all clean debug
