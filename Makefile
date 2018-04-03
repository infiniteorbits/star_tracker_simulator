CC=gcc
CXX=g++
RM=rm -f
OPENGL=-lglut -lGL -lGLU -lglut -lm
CPPFLAGS=-g $(shell root-config --cflags)
LDFLAGS=-g $(shell root-config --ldflags)
LDLIBS=$(shell root-config --libs) $(OPENGL)

SRCS=hip_parser.cpp Star.cpp
OBJS=$(subst .cc,.o,$(SRCS))
TARGET=hip_parser

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

# tool.o: tool.cc support.hh

# support.o: support.hh support.cc

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) $(TARGET)