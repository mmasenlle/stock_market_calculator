SRC_ROOT = ..
include $(SRC_ROOT)/flags.mk

TARGET = ccltor_output

sources = $(wildcard *.cc)
objects = $(sources:.cc=.o)

all:	$(TARGET)

$(TARGET):	$(objects)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(objects) $(CCLTOR_LIB) $(LDFLAGS)

$(objects): %.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(INCLUDE) $(CXXFLAGS) $< -o $@

install:
	cp $(TARGET) $(INSTALL_DIR)/$(TARGET)
	cp -R web $(INSTALL_DIR)/

clean:
	rm -f *.o $(TARGET)

