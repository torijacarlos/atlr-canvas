CC=gcc
CFLAGS=-Wpedantic -Wextra -Wall -std=c23 $(shell pkg-config --cflags sdl3)
LIBS=c m
LINKED_LIBS=$(foreach L, $(LIBS), -l$(L)) $(shell pkg-config --libs sdl3)
NOISE_WARNINGS=-Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wno-missing-braces
PROJECT_NAME=canvas
RAW_GITHUB=https://raw.githubusercontent.com

.PHONY: all

all: debug release
	@echo "===== finished building"

install: clean deps setup

atlr:
	@if [ -s ./vendor/atlr/atlr.h ]; then        \
		echo "------ atlr: installed ";          \
	else                                         \
		echo "------ atlr: downloading ";        \
		mkdir -p ./vendor/atlr 2> /dev/null ||:; \
		curl -s $(RAW_GITHUB)/torijacarlos/atlr/refs/heads/main/atlr.h > ./vendor/atlr/atlr.h;   \
		curl -s $(RAW_GITHUB)/torijacarlos/atlr/refs/heads/main/LICENSE > ./vendor/atlr/LICENSE; \
	fi;

stb:
	@if [ -s ./vendor/stb/stb_truetype.h ]; then \
		echo "------ stb: installed ";           \
	else                                         \
		echo "------ stb: downloading ";         \
		mkdir -p ./vendor/stb 2> /dev/null ||:;  \
		curl -s $(RAW_GITHUB)/nothings/stb/refs/heads/master/LICENSE > ./vendor/stb/LICENSE;               \
		curl -s $(RAW_GITHUB)/nothings/stb/refs/heads/master/stb_truetype.h > ./vendor/stb/stb_truetype.h; \
		curl -s $(RAW_GITHUB)/nothings/stb/refs/heads/master/stb_image.h > ./vendor/stb/stb_image.h;       \
	fi; 


sdl3:
	@echo "------ sdl3: make sure you have installed sdl3"

deps: atlr stb sdl3

clean:
	@echo "------ cleaning"
	@rm -rf ./vendor 2> /dev/null ||:;

setup:
	@mkdir -p ./build/debug ||:;
	@mkdir -p ./build/release ||:;

debug:
	@echo "===== building debug"
	@rm ./build/debug/* 2> /dev/null ||:;
	@$(CC) $(CFLAGS) $(LINKED_LIBS) $(NOISE_WARNINGS) -g -DATLR_DEBUG main.c -o ./build/debug/$(PROJECT_NAME)

release:
	@echo "===== building release"
	@rm ./build/release/* 2> /dev/null  ||:;
	@rm ~/.local/.atelier/$(PROJECT_NAME) 2> /dev/null  ||:;
	@$(CC) $(CFLAGS) $(LINKED_LIBS) $(NOISE_WARNINGS) -O2 main.c -o ./build/release/$(PROJECT_NAME)
	@cp ./build/release/$(PROJECT_NAME) ~/.local/.atelier/;

