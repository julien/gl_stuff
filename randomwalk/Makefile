CXXFLAGS = -Wall -pedantic
FLAGS = $(shell pkg-config --libs --static glew glfw3)
UNAME = $(shell uname -s)
ifeq ($(UNAME), Darwin)
	FLAGS += -framework OpenGL
endif

path=$(abspath $(lastword $(MAKEFILE_LIST)))
bin=$(notdir $(patsubst %/,%,$(dir $(path))))
src = $(shell find *.cpp -type f)

$(bin): $(src)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(FLAGS)

debug: CXXFLAGS += -DDEBUG -g
debug: clean $(bin);

all: $(bin)
