SRC_ROOT = ..
include $(SRC_ROOT)/flags.mk

LDFLAGS += -pthread

TARGET = ccltor_cruncher

sources = $(wildcard *.cc)
objects = $(sources:.cc=.o)

all:	$(TARGET)

$(TARGET):	$(objects)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(objects) $(CCLTOR_LD) $(LDFLAGS)

$(objects): %.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(INCLUDE) $(CXXFLAGS) $< -o $@

install:
	cp $(TARGET) $(INSTALL_DIR)/$(TARGET)

clean:
	rm -f *.o $(TARGET)

