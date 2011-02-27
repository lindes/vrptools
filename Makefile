# Makefile for vrptools.
#
# Copyright 2011 by David Lindes.  All rights reserved.
#
# part of vrptools -- https://github.com/lindes/vrptools
#
# Available under terms in the LICENSE file that should accompany this
# file.  Please consider that file to be included herein by reference.

CFLAGS += -Werror -Wall -Wextra

HEADERS = vrptools.h
PROGRAMS = cineinfo cine-extract
LIBRARY = lib/libvrp.a
LIB_OBJ = lib/read_cine.o lib/print_helpers.o
CFLAGS += -I.

EXAMPLE_CINE = test_data/appendix_example.cine

default: smalltest

smalltest: cine-extract ${EXAMPLE_CINE}
	./cine-extract ${EXAMPLE_CINE}

test: ${PROGRAMS} magic ${EXAMPLE_CINE}
	file -M magic test_data/*.cine
	./cineinfo test_data/*.cine

get_docs:
	wget http://www.visionresearch.com/devzonedownloads/cine640.pdf

test_data/appendix_example.cine: test_data/appendix_example.txt hex2cine
	./hex2cine $<

${LIB_OBJ} cineinfo.o: ${HEADERS}

library: ${LIBRARY}
${LIBRARY}: ${LIB_OBJ}
	${AR} cruv $@ ${LIB_OBJ}

${PROGRAMS}: ${LIBRARY}

TAGS:
	etags **/*.c **/*.h

clean:
	rm -f *.o lib/*.[oa]

clobber: clean
	rm -f ${LIBRARY} ${PROGRAMS} ${EXAMPLE_CINE} TAGS

distclean: clobber
	rm -f cine640.pdf
