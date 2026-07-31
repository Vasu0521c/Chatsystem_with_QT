CC = g++

FLAGS = $(shell pkg-config --cflags Qt6Gui)
WAR   = -Wall -Wpedantic -Wextra -g
LIBS  = $(shell pkg-config --libs Qt6Gui)

SRC_DIR = src
BIN_DIR = bin

CLIENT_FILES = $(SRC_DIR)/client/(wildcard *.[ch])
SERVER_FILES = $(SRC_DIR)/server/(wildcard *.[ch])

LDLIBS = $(patsubst -I%, -isystem%, $(LIBS))

all : $(BIN_DIR)/server $(BIN_DIR)/client

$(BIN_DIR)/server : $(SERVER_FILES)
	@echo "Compiling Server"
	@$(CC) $^ -o $@ $(WAR) $(FLAGS) $(LDLIBS)

$(BIN_DIR)/client : $(CLIENT_FILES)
	@echo "Compiling Client"
	@$(CC) $^ -o $@ $(WAR) $(FLAGS) $(LDLIBS)
