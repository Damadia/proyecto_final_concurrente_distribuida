CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread
LDFLAGS = -pthread

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include

# Sources for each executable
SRCS_MAIN = $(SRC_DIR)/main.c $(SRC_DIR)/monitors.c
SRCS_AUTH = $(SRC_DIR)/authServer.c $(SRC_DIR)/monitors.c
SRCS_CLIENT = $(SRC_DIR)/authClient.c

OBJS_MAIN = $(SRCS_MAIN:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJS_AUTH = $(SRCS_AUTH:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJS_CLIENT = $(SRCS_CLIENT:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TARGET_MAIN = $(BIN_DIR)/main
TARGET_AUTH = $(BIN_DIR)/authServer
TARGET_CLIENT = $(BIN_DIR)/authClient

all: directories $(TARGET_MAIN) $(TARGET_AUTH) $(TARGET_CLIENT)

directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_MAIN): $(OBJS_MAIN)
	$(CC) $(CFLAGS) -o $@ $(OBJS_MAIN) $(LDFLAGS)

$(TARGET_AUTH): $(OBJS_AUTH)
	$(CC) $(CFLAGS) -o $@ $(OBJS_AUTH) $(LDFLAGS)

$(TARGET_CLIENT): $(OBJS_CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJS_CLIENT) $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/*

run-main: $(TARGET_MAIN)
	./$(TARGET_MAIN)

run-auth: $(TARGET_AUTH)
	./$(TARGET_AUTH)

run-client: $(TARGET_CLIENT)
	./$(TARGET_CLIENT)

.PHONY: all clean run-main run-auth run-client