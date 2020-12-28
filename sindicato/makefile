TARGET = ./build/sindicato

SRC_DIR =./src
BUILD_DIR =./build
SHARED_SRC_DIR = ../shared/src
SHARED_BUILD_DIR = ../shared/build

LIBS = -lcommons -lreadline -pthread -lm 
CC = gcc 
CFLAGS = -I $(SRC_DIR) -I $(SHARED_SRC_DIR) -g -Wall 

.PHONY: default all clean

default: clean $(TARGET)
all: default 

HEADERS = $(shell find $(SRC_DIR) -iname '*.h' | tr '\n' ' ')
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(shell find $(SRC_DIR) -iname '*.c' | tr '\n' ' '))

SHARED_HEADERS = $(shell find $(SHARED_SRC_DIR) -iname '*.h' | tr '\n' ' ')
SHARED_OBJECTS = $(patsubst $(SHARED_SRC_DIR)/%.c, $(SHARED_BUILD_DIR)/%.o, $(shell find $(SHARED_SRC_DIR) -iname '*.c' | tr '\n' ' '))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) $(SHARED_HEADERS)
	@test ! -d $(BUILD_DIR) && mkdir $(BUILD_DIR) || echo -n ""
	$(CC) -c -o $@ $< $(CFLAGS) 

$(SHARED_BUILD_DIR)/%.o: $(SHARED_SRC_DIR)/%.c $(SHARED_HEADERS)
	@test ! -d $(SHARED_BUILD_DIR) && mkdir $(SHARED_BUILD_DIR) || echo -n ""
	@test ! -d $(SHARED_BUILD_DIR)/operaciones && mkdir $(SHARED_BUILD_DIR)/operaciones || echo -n ""
	@test ! -d $(SHARED_BUILD_DIR)/structs && mkdir $(SHARED_BUILD_DIR)/structs || echo -n ""
	$(CC) -c -o $@ $< $(CFLAGS)

.PRECIOUS: $(TARGET) $(OBJECTS) $(SHARED_HEADERS) 

$(TARGET): $(OBJECTS) $(SHARED_OBJECTS)
	@echo
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean: 
	rm -fr $(BUILD_DIR)/* *~ core $(INCDIR)/*~ $(TARGET) $(SHARED_BUILD_DIR)/*



