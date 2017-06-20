
CURL = curl -L -s

BUILD_PATH=build
INSTALL_PATH=install

SPHINXBASE = sphinxbase-5prealpha.tar.gz
SPHINXPATH = https://downloads.sourceforge.net/project/cmusphinx/sphinxbase/5prealpha/$(SPHINXBASE)

PKS_NAME = pocketsphinx-5prealpha.tar.gz
PKS_PATH = https://sourceforge.net/projects/cmusphinx/files/pocketsphinx/5prealpha/$(PKS_NAME)

SWIG_NAME = swig-3.0.12.tar.gz
SWIG_PATH = https://sourceforge.net/projects/swig/files/swig/swig-3.0.12/$(SWIG_NAME)

PCRE_PATH = https://ftp.pcre.org/pub/pcre/pcre-8.40.tar.gz
PCRE_NAME = $(notdir $(PCRE_PATH))

PKGCONFIG_NAME = pkg-config-0.29.2.tar.gz
PKGCONFIG_PATH = https://pkgconfig.freedesktop.org/releases/$(PKGCONFIG_NAME)

SDL2_PATH = https://www.libsdl.org/release/SDL2-2.0.5.tar.gz
SDL2_NAME = $(notdir $(SDL2_PATH))

FETCH_PATHS = $(PKGCONFIG_PATH) $(SWIG_PATH) $(SDL2_PATH) $(PCRE_PATH) $(SPHINXPATH) $(PKS_PATH)
FETCH_FILES = $(notdir $(FETCH_PATHS))

all: fetch unpack build

fetch: $(FETCH_FILES)

$(SPHINXBASE):
	$(CURL) $(SPHINXPATH) > $@

$(PKS_NAME):
	$(CURL) $(PKS_PATH) > $@

$(SWIG_NAME):
	$(CURL) $(SWIG_PATH) > $@

$(PCRE_NAME):
	$(CURL) $(PCRE_PATH) > $@

$(PKGCONFIG_NAME):
	$(CURL) $(PKGCONFIG_PATH) > $@

$(SDL2_NAME):
	$(CURL) $(SDL2_PATH) > $@

EXTRACT_TARGETS := $(addprefix .extract_,$(FETCH_FILES))
#BUILD_TARGETS := $(addprefix .build_,$(FETCH_FILES))
CLEAN_TARGETS := $(addprefix .clean_,$(FETCH_FILES))

unpack: $(EXTRACT_TARGETS)

$(EXTRACT_TARGETS): .extract_%: %
	tar xf $<
	touch $@

#build: $(BUILD_TARGETS)

#$(BUILD_TARGETS): .build_%: %
#	build_$(basename $<)
#	touch $@

clean: $(CLEAN_TARGETS)
	$(RM) -f $(FETCH_FILES)
	$(RM) -rf $(BUILD_PATH)/*
	$(RM) -rf $(INSTALL_PATH)/*

$(CLEAN_TARGETS): .clean_%: .extract_%
	$(RM) -rf "$(shell tar tf $(patsubst .extract_%,%,$<) | head -1)"
	$(RM) -f $<
