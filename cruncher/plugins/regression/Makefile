SRC_ROOT = ../../..
include $(SRC_ROOT)/flags.mk

INCLUDE += -I$(SRC_ROOT)/cruncher
CXXFLAGS += -fPIC
TARGET = libCCregression.so

sources = $(wildcard *.cc)
objects = $(sources:.cc=.o)

all:	$(TARGET)

$(TARGET):	$(objects)
	$(CXX) -shared -o $(TARGET) $(objects)

$(objects): %.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(INCLUDE) $(CXXFLAGS) $< -o $@

install:
	cp $(TARGET) $(INSTALL_DIR)/$(TARGET)

clean:
	rm -f *.o $(TARGET)
