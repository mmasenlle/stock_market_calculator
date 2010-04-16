SRC_ROOT = ..
include $(SRC_ROOT)/flags.mk

install_scripts = ccltor.sh expansion2xml.pl

nothing:

install:
	for i in $(install_scripts); do \
		chmod +x $$i || exit 1; \
		cp -v $$i $(INSTALL_DIR)/$$i || exit 1; \
	done

clean: