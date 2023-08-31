EIGEN := $(shell pkg-config eigen3 --cflags)
ROOT := $(shell root-config --cflags)
ROOT_LIBS := $(shell root-config --libs)

SRC := $(wildcard *.cpp)

C_FLAGS := -Wall -Werror -Wextra -O2 -std=c++20

%.exe: %.cpp
	@echo "COMPILING $< --> $@"
	g++ $< -o $@ $(EIGEN) $(ROOT) $(ROOT_LIBS) -I inc `wx-config --cxxflags --libs`

dump:
	@echo $(EIGEN)
	@echo $(ROOT)
	@echo $(ROOT_LIBS)
	@echo $(SRC)
