##
# MegaZeux Build System (GNU Make)
#
# NOTE: This build system was recently re-designed to not use recursive
#       Makefiles. The rationale for this is documented here:
#                  http://aegis.sourceforge.net/auug97.pdf
##

.PHONY: clean package_clean help_check

include platform.inc
include version.inc

all: mzx utils

include arch/${PLATFORM}/Makefile.in

CC  ?= gcc
CXX ?= g++
AR  ?= ar

ifeq (${DEBUG},1)
#
# Force debug builds to disable the optimizer
#
OPTIMIZE_CFLAGS = -O0
else
OPTIMIZE_CFLAGS ?= -O2
endif

SDL_CFLAGS  ?= `sdl-config --cflags`
SDL_LDFLAGS ?= `sdl-config --libs`

VORBIS_CFLAGS  ?= -I${PREFIX}/include
ifneq (${TREMOR},1)
VORBIS_LDFLAGS ?= -L${PREFIX}/lib -lvorbisfile -lvorbis -logg
else
VORBIS_LDFLAGS ?= -L${PREFIX}/lib -lvorbisidec
endif

MIKMOD_LDFLAGS ?= -L${PREFIX} -lmikmod

ifeq (${LIBPNG},1)
LIBPNG_CFLAGS ?= `libpng12-config --cflags`
LIBPNG_LDFLAGS ?= `libpng12-config --libs`
endif

ifeq (${DEBUG},1)
CFLAGS    = ${OPTIMIZE_CFLAGS} -g -Wall -std=gnu99 -DDEBUG ${ARCH_CFLAGS}
CXXFLAGS  = ${OPTIMIZE_CFLAGS} -g -Wall -DDEBUG ${ARCH_CXXFLAGS}
o         = dbg.o
else
CFLAGS   += ${OPTIMIZE_CFLAGS} -Wall -std=gnu99 -DNDEBUG ${ARCH_CFLAGS}
CXXFLAGS += ${OPTIMIZE_CFLAGS} -Wall -DNDEBUG ${ARCH_CXXFLAGS}
o         = o
endif

#
# The SUPPRESS_BUILD hack is required to allow the placebo "dist"
# Makefile to provide an 'all:' target, which allows it to print
# a message. We don't want to pull in other targets, confusing Make.
#
ifneq (${SUPPRESS_BUILD},1)

ifneq (${DEBUG},1)
mzx = ${TARGET}${BINEXT}
else
mzx = ${TARGET}.dbg${BINEXT}
endif

mzx: ${mzx}

ifeq (${BUILD_MODPLUG},1)
BUILD_GDM2S3M=1
endif

include src/utils/Makefile.in
include src/Makefile.in
include src/network/Makefile.in

package_clean: utils_package_clean
	@mv ${mzx} ${mzx}.backup
	-@${MAKE} DEBUG=1 ${TARGET}.dbg${BINEXT}_clean # hack
	@${MAKE}         ${mzx}_clean
	@rm -f src/config.h
	@echo "PLATFORM=none" > platform.inc
	@mv ${mzx}.backup ${mzx}

clean: ${mzx}_clean utils_clean

distclean: clean
	@echo "  DISTCLEAN"
	@rm -f src/config.h
	@echo "PLATFORM=none" > platform.inc

mzx_help.fil: ${txt2hlp} docs/WIPHelp.txt
	@src/utils/txt2hlp docs/WIPHelp.txt $@

help_check: ${hlp2txt} mzx_help.fil
	@src/utils/hlp2txt mzx_help.fil help.txt
	@diff -q docs/WIPHelp.txt help.txt
	@rm -f help.txt

endif
