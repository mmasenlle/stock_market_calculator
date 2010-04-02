
CCLTOR_VERSION = 0.1.0beta

SRC_ROOT ?= .
INSTALL_DIR ?= $(SRC_ROOT)/bin

INCLUDE = -I$(SRC_ROOT)/lib -I$(SRC_ROOT)/ccltor -I/usr/include/libxml2
LDFLAGS = $(SRC_ROOT)/lib/ccltor_lib.a $(SRC_ROOT)/ccltor/ccltor.a -lxml2

CPPFLAGS = -DCCLTOR_VERSION="\"$(CCLTOR_VERSION)\""

RELEASE_FLAGS = -march=native -O3 -pipe -DLOG_COMPILATION_LEVEL=4 -DNDEBUG
DEBUG_FLAGS = -g

CFLAGS = $(RELEASE_FLAGS)

ifeq "$(DEBUG)" "1"
CFLAGS = $(DEBUG_FLAGS)
endif
CXXFLAGS = $(CFLAGS)

