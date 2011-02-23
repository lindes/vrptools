CFLAGS += -Werror -Wall -Wextra

HEADERS = vrptools.h

default: smalltest

smalltest: cineinfo appendix_example.cine
	./cineinfo appendix_example.cine

test: header_check cineinfo magic appendix_example.cine
	file -M magic *.cine
	./cineinfo *.cine

header_check: header_check.c ${HEADERS}

get_docs:
	wget http://www.visionresearch.com/devzonedownloads/cine640.pdf

appendix_example.cine: appendix_example.txt hex2cine
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
