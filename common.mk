CXXFLAGS=-Wall -pedantic
FLAGS:=$(shell pkg-config --libs --static glew glfw3)
FLAGS+=-I../include
UNAME=$(shell uname -s)
ifeq ($(UNAME), Darwin)
	FLAGS+=-framework OpenGL
endif

bin?=app
src?=$(shell find *.cpp -type f)

$(bin): $(src)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(FLAGS)

debug: CXXFLAGS += -DDEBUG -g
debug: clean $(bin);

clean:
	@if [ -f $(bin) ];then rm $(bin);fi

all: $(bin)
