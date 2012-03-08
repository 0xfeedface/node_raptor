RAPTOR_VERSION=2.0.6
RM=rm -rf
WAF=node-waf
CWD=$(shell pwd -P)
DEPS_DIR=$(CWD)/deps
RAPTOR_NAME=raptor2-$(RAPTOR_VERSION)
RAPTOR_DIR=$(DEPS_DIR)/$(RAPTOR_NAME)
RAPTOR_TARBALL_NAME=$(RAPTOR_NAME).tar.gz
RAPTOR_TARBALL=$(DEPS_DIR)/$(RAPTOR_TARBALL_NAME)
RAPTOR_URL=http://download.librdf.org/source/$(RAPTOR_TARBALL_NAME)
RAPTOR_STATIC_LIB=$(RAPTOR_DIR)/dist/lib
RAPTOR_PREFIX=$(RAPTOR_DIR)
export RAPTOR_PREFIX	# export for waf

WGET=$(shell command -v curl)
ifeq ($(WGET),)
	WGET=wget
else
	WGET=curl -O
endif

all: bindings

bindings: raptor
	cd src \
		&& $(WAF) configure \
		&& $(WAF) build

$(RAPTOR_TARBALL):
	cd $(DEPS_DIR); $(WGET) $(RAPTOR_URL)

$(RAPTOR_DIR): $(RAPTOR_TARBALL)
	cd $(DEPS_DIR); tar -xzf $(RAPTOR_TARBALL)

$(RAPTOR_STATIC_LIB): $(RAPTOR_DIR)
	cd $(RAPTOR_DIR); \
	./configure --prefix=$(RAPTOR_DIR)/dist \
		--enable-static \
		--disable-shared \
		--disable-gtk-doc-html
	make install -C $(RAPTOR_DIR)

raptor: $(RAPTOR_STATIC_LIB)

.PHONY: clean
clean: clean-bindings clean-raptor

clean-bindings:
	@-cd src && $(WAF) distclean > /dev/null 2>&1
	@-$(RM) build > /dev/null 2>&1

clean-raptor:
	@-$(RM) $(RAPTOR_TARBALL) > /dev/null 2>&1
	@-$(RM) $(RAPTOR_DIR) > /dev/null 2>&1
