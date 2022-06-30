CFLAGS := -g -Wall -Wextra -pedantic -std=c11

PROJECT := ball-bounce 
BUILD_DIR := ./bin

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/$(PROJECT)

$(BUILD_DIR)/$(PROJECT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)
