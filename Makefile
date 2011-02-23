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
PROGRAMS = cineinfo
LIBRARY = lib/libvrp.a
LIB_OBJ = lib/read_cine.o lib/print_helpers.o
CFLAGS += -I.

default: smalltest

smalltest: cineinfo test_data/appendix_example.cine
	./cineinfo test_data/appendix_example.cine

test: ${PROGRAMS} magic test_data/appendix_example.cine
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

cineinfo: cineinfo.o ${LIBRARY}

TAGS:
	etags **/*.c **/*.h

clean:
	rm -f *.o lib/*.[oa]

clobber: clean
	rm -f ${LIBRARY} ${PROGRAMS}
