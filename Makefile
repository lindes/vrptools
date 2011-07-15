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
PROGRAMS = cine-info cine-extract
LIBRARY = lib/libvrp.a
LIB_OBJ = lib/read_cine.o lib/print_helpers.o
CFLAGS += -I.

EXAMPLE_CINE = test_data/appendix_example.cine
OUTPUT_DIR = cine-extract.d

default: smalltest

smalltest: cine-extract ${EXAMPLE_CINE} ${OUTPUT_DIR}
	./cine-extract ${EXAMPLE_CINE}

test: ${PROGRAMS} magic ${EXAMPLE_CINE}
	file -M magic test_data/*.cine
	./cine-info test_data/*.cine

${OUTPUT_DIR}:
	mkdir -p ${OUTPUT_DIR}

programs program: ${PROGRAMS}

get_docs:
	wget http://www.visionresearch.com/devzonedownloads/cine640.pdf

test_data/appendix_example.cine: test_data/appendix_example.txt hex2cine
	./hex2cine $<

${LIB_OBJ} cine-info.o cine-extract.o: ${HEADERS}

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
	rm -rf ${OUTPUT_DIR}

distclean: clobber
	rm -f cine640.pdf
