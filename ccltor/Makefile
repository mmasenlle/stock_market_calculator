SRC_ROOT = ..
include $(SRC_ROOT)/flags.mk

TARGET = ccltor.a

sources = $(wildcard *.cc)
objects = $(sources:.cc=.o)

all:	$(TARGET)

$(TARGET):	$(objects)
	$(AR) r $(TARGET) $(objects)
	
$(objects): %.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(INCLUDE) $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o *.a
