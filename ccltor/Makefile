SRC_ROOT = ..
include $(SRC_ROOT)/flags.mk

CPPFLAGS += -DCCLTOR_VERSION="\"$(CCLTOR_VERSION)\"" \
	-DCCLTOR_REVISION="\"$(shell cat $(SRC_ROOT)/.git/refs/heads/master)\""
CXXFLAGS += -fPIC
TARGET = ccltor.a
TARGET_LD = libccltor.so

sources = $(wildcard */*.cc) $(wildcard *.cc) #$(wildcard ic/*.cc) $(wildcard db/*.cc)
objects = $(sources:.cc=.o)

all:	$(TARGET) $(TARGET_LD)
	
$(TARGET):	$(objects)
	$(AR) r $(TARGET) $(objects)

$(TARGET_LD):	$(objects)
	$(CXX) -shared -o $(TARGET_LD) $(objects)
	
$(objects): %.o: %.cc $(SRC_ROOT)/flags.mk
	$(CXX) -c $(CPPFLAGS) $(INCLUDE) $(CXXFLAGS) $< -o $@

install:
	cp $(TARGET_LD) $(INSTALL_DIR)/$(TARGET_LD)

clean:
	rm -f */*.o *.o *.a *.so
