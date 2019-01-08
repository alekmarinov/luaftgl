# modules/c/gui/gl/luaftgl/src/luaftgl.mak

TARGET=luaftgl
VERSION=1.1.0
OBJS=LuaFTGL.o
EXTRA_INCS=$(shell pkg-config ftgl --cflags) -I../include
EXTRA_LIBS=$(shell pkg-config ftgl --libs)
CC=${CXX}
