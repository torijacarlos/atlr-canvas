CC=gcc
CFLAGS=-Wpedantic -Wextra -Wall -std=c23 $(shell pkg-config --cflags sdl3)
LIBS=c m
LINKED_LIBS=$(foreach L, $(LIBS), -l$(L)) $(shell pkg-config --libs sdl3)
NOISE_WARNINGS=-Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wno-missing-braces

.PHONY: all

all: deps setup

atlr:
	@echo "------ downloading atlr"
	@mkdir -p ./vendor/atlr ||:;
	@rm ./vendor/atlr/* ||:;
	@curl -s https://raw.githubusercontent.com/torijacarlos/atlr/refs/heads/main/atlr.h > ./vendor/atlr/atlr.h
	@curl -s https://raw.githubusercontent.com/torijacarlos/atlr/refs/heads/main/LICENSE > ./vendor/atlr/LICENSE

stb:
	@echo "------ downloading stb"
	@mkdir -p ./vendor/stb ||:;
	@rm ./vendor/stb/* ||:;
	@curl -s https://raw.githubusercontent.com/nothings/stb/refs/heads/master/LICENSE > ./vendor/stb/LICENSE
	@curl -s https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_truetype.h > ./vendor/stb/stb_truetype.h
	@curl -s https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h > ./vendor/stb/stb_image.h

sdl3:
	@echo "------ make sure you have installed sdl3"

deps: atlr stb sdl3

setup:
	@mkdir -p ./build/debug ||:;
	@mkdir -p ./build/release ||:;

debug:
	@echo "===== building debug"
	@rm ./build/debug/*
	@$(CC) $(CFLAGS) $(LINKED_LIBS) $(NOISE_WARNINGS) -g main.c -o ./build/debug/canvas

release:
	@echo "===== building release"
	@rm ./build/release/*
	@$(CC) $(CFLAGS) $(LINKED_LIBS) $(NOISE_WARNINGS) -O2 main.c -o ./build/release/canvas
	@cp ./build/release/canvas ~/.local/.atelier/ ||:;

build: debug release
	@echo "===== finished building"

