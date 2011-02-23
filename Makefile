CFLAGS += -Werror -Wall -Wextra

HEADERS = vrptools.h

default: smalltest

smalltest: cineinfo test_data/appendix_example.cine
	./cineinfo test_data/appendix_example.cine

test: header_check cineinfo magic test_data/appendix_example.cine
	file -M magic test_data/*.cine
	./cineinfo test_data/*.cine

header_check: header_check.c ${HEADERS}

get_docs:
	wget http://www.visionresearch.com/devzonedownloads/cine640.pdf

test_data/appendix_example.cine: test_data/appendix_example.txt hex2cine
	./hex2cine $<

LIBRARY = lib/libvrp.a
LIB_OBJ = lib/read_cine.o lib/print_helpers.o
CFLAGS += -I.

${LIB_OBJ} cineinfo.o: ${HEADERS}

library: ${LIBRARY}
${LIBRARY}: ${LIB_OBJ}
	${AR} cruv $@ ${LIB_OBJ}

cineinfo: cineinfo.o ${LIBRARY}

TAGS:
	etags **/*.c **/*.h

clean:
	rm -f *.o lib/*.[oa]
