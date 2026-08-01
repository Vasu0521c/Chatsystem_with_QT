CC = g++

FLAGS   := $(shell pkg-config --cflags Qt6Gui Qt6Widgets)
WAR     := -Wall -Wpedantic -Wextra -pthread -g
LDLIBS  := $(shell pkg-config --libs Qt6Gui Qt6Widgets)
CFLAGS  :=  $(patsubst -I%, -isystem%, $(FLAGS))

SRC_DIR := src
BIN_DIR := bin
SHR_DIR := shared

/==========================================================================/

SER_SRC := $(wildcard $(SRC_DIR)/server/*.c) \
	   $(wildcard $(SRC_DIR)/server/*.h) \
	   $(wildcard $(SRC_DIR)/server/*.cpp)

CLI_SRC := $(wildcard $(SRC_DIR)/client/*.c) \
	   $(wildcard $(SRC_DIR)/client/*.h) \
	   $(wildcard $(SRC_DIR)/client/*.cpp)

/==========================================================================/

S_FILES := $(addprefix $(SHR_DIR)/server/, $(addsuffix .o, $(notdir $(basename $(SER_SRC)))))

C_FILES := $(addprefix $(SHR_DIR)/client/, $(addsuffix .o, $(notdir $(basename $(CLI_SRC)))))

all : $(BIN_DIR)/server $(BIN_DIR)/client

/==========================================================================/

$(BIN_DIR)/server : $(S_FILES)
	@echo "Compiling Server"
	@$(CC) $^ -o $@ $(WAR) $(CFLAGS) $(LDLIBS)

$(BIN_DIR)/client : $(C_FILES)
	@echo "Compiling Client"
	@$(CC) $^ -o $@ $(WAR) $(CFLAGS) $(LDLIBS)

/==========================================================================/

$(SHR_DIR)/server/%.o : $(SRC_DIR)/server/%.c $(SRC_DIR)/server/%.h
	@ gcc -c $< -o $@

$(SHR_DIR)/client/%.o : $(SRC_DIR)/client/%.c $(SRC_DIR)/client/%.h
	@ gcc -c $< -o $@

/==========================================================================/

$(SHR_DIR)/client/client_window.o : $(SRC_DIR)/client/client_window.cpp
	@ $(CC) -c $< -o $@ $(CFLAGS)

$(SHR_DIR)/server/server_window.o : $(SRC_DIR)/server/server_window.cpp
	@ $(CC) -c $< -o $@ $(CFLAGS)

/==========================================================================/
