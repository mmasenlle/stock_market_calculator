SRC_ROOT = ..
include $(SRC_ROOT)/flags.mk

CPPFLAGS += -DCCLTOR_VERSION="\"$(CCLTOR_VERSION)\"" \
	-DCCLTOR_REVISION="\"$(shell cat $(SRC_ROOT)/.git/refs/heads/master)\""
CXXFLAGS += -fPIC
TARGET = ccltor.a

sources = $(wildcard *.cc) $(wildcard ic/*.cc) $(wildcard db/*.cc)
objects = $(sources:.cc=.o)

all:	$(TARGET)

$(TARGET):	$(objects)
	$(AR) r $(TARGET) $(objects)
	
$(objects): %.o: %.cc $(SRC_ROOT)/flags.mk
	$(CXX) -c $(CPPFLAGS) $(INCLUDE) $(CXXFLAGS) $< -o $@

clean:
	rm -f db/*.o ic/*.o *.o *.a
