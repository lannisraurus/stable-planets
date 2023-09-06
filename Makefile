EIGEN := $(shell pkg-config eigen3 --cflags)
ROOT := $(shell root-config --cflags)
ROOT_LIBS := $(shell root-config --libs)

SRC := $(wildcard src/*.cpp)

C_FLAGS := -Wall -Werror -Wextra -O2 -std=c++20

all:
	g++ $(SRC) -o app $(EIGEN) $(ROOT) $(ROOT_LIBS) -I inc `wx-config --cxxflags --libs`
