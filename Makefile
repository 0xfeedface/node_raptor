RAPTOR_VERSION=2.0.6
RM=rm -rf
WAF=node-waf
DEPS_DIR=$(CURDIR)/deps
RAPTOR_NAME=raptor2-$(RAPTOR_VERSION)
RAPTOR_DIR=$(DEPS_DIR)/$(RAPTOR_NAME)
RAPTOR_TARBALL_NAME=$(RAPTOR_NAME).tar.gz
RAPTOR_TARBALL=$(DEPS_DIR)/$(RAPTOR_TARBALL_NAME)
RAPTOR_URL=http://download.librdf.org/source/$(RAPTOR_TARBALL_NAME)
RAPTOR_PREFIX?=$(RAPTOR_DIR)/dist
RAPTOR_STATIC_LIB:=$(RAPTOR_PREFIX)/lib/libraptor2.a
CFLAGS?=-g -Wall
CXXFLAGS?=-g -Wall
CFLAGS+=-fPIC
CXXFLAGS+=-fPIC
export RAPTOR_PREFIX CFLAGS CXXFLAGS	# export for waf

WGET=@-$(shell command -v curl > /dev/null 2>&1)
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
	-mkdir $(DEPS_DIR)
	cd $(DEPS_DIR) \
		&& $(WGET) $(RAPTOR_URL)

$(RAPTOR_DIR): $(RAPTOR_TARBALL)
	cd $(DEPS_DIR) \
		&& tar -xzf $(RAPTOR_TARBALL)

$(RAPTOR_STATIC_LIB): $(RAPTOR_DIR)
	cd $(RAPTOR_DIR) \
		&& ./configure \
		--prefix=$(RAPTOR_DIR)/dist \
		--enable-static \
		--disable-shared \
		--disable-gtk-doc-html
	make install -C $(RAPTOR_DIR)

ifeq ($(wildcard $(RAPTOR_STATIC_LIB)),)
raptor: $(RAPTOR_STATIC_LIB)
else
raptor:
endif

.PHONY: clean
clean: clean-bindings clean-raptor

clean-bindings:
	-cd src \
		&& $(WAF) distclean > /dev/null 2>&1
	-$(RM) build > /dev/null 2>&1

clean-raptor:
	-$(RM) $(RAPTOR_TARBALL) > /dev/null 2>&1
	-$(RM) $(RAPTOR_DIR) > /dev/null 2>&1
	-$(RM) $(DEPS_DIR) > /dev/null 2>&1
