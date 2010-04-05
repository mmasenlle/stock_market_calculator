SRC_ROOT = ..
include $(SRC_ROOT)/flags.mk

INCLUDE = -I/usr/include/libxml2
TARGET = ccltor_lib.a

sources = $(wildcard *.cc)
objects = $(sources:.cc=.o)

all:	$(TARGET)

$(TARGET):	$(objects)
	$(AR) r $(TARGET) $(objects)

$(objects): %.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(INCLUDE) $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o *.a
