
include flags.mk

install_dirs = ccltor feeder control graphic/chart scripts \
		cruncher cruncher/statistics cruncher/test

build_dirs = $(install_dirs)

.PHONY: all clean install src

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

src:	clean
	tar cvzf  ../ccltor_src`date +%F`.tar.gz --exclude .git `ls` 
