
include flags.mk

install_dirs = feeder

build_dirs = lib ccltor $(install_dirs)

.PHONY: all clean install

all:
	for i in $(build_dirs); do \
		$(MAKE) -C $$i || exit 1; \
	done

install:
	for i in $(install_dirs); do \
		$(MAKE) -C $$i install || exit 1; \
	done

clean:
	for i in $(build_dirs); do \
		$(MAKE) -C $$i clean; \
	done

