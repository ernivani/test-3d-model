# Makefile pour la génération CMake
BUILD_DIR = build
EXECUTABLE = $(BUILD_DIR)/bin/sdl_obj_viewer

all: configure build

configure:
	mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && cmake ..

build:
	cd $(BUILD_DIR) && make

run: build
	./$(EXECUTABLE) path/to/model.obj

clean:
	rm -rf $(BUILD_DIR)/bin/*
	rm -rf $(BUILD_DIR)/CMakeFiles
	rm -f $(BUILD_DIR)/CMakeCache.txt

fclean: clean
	rm -rf $(BUILD_DIR)

re: fclean all

.PHONY: all configure build run clean fclean re 