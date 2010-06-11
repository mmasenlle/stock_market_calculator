
CCLTOR_VERSION = 0.2.0

SRC_ROOT ?= .
INSTALL_DIR ?= ~/calculinator

INCLUDE = -I$(SRC_ROOT)/ccltor -I$(SRC_ROOT)/ccltor/db -I$(SRC_ROOT)/ccltor/ic \
	  -I$(SRC_ROOT)/ccltor/utils -I$(SRC_ROOT)/ccltor/calculus -I/usr/include/libxml2

CCLTOR_LIB = $(SRC_ROOT)/ccltor/ccltor.a
CCLTOR_LD = -lccltor -Wl,-L$(SRC_ROOT)/ccltor

LDFLAGS = -lxml2 -lpq
CPPFLAGS =

RELEASE_FLAGS = -march=native -O3 -pipe -DLOG_COMPILATION_LEVEL=4 -DNDEBUG
DEBUG_FLAGS = -g

CFLAGS = $(DEBUG_FLAGS)

ifeq "$(DEBUG)" "0"
CFLAGS = $(RELEASE_FLAGS)
endif
CXXFLAGS = $(CFLAGS)

