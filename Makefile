CFLAGS += -Werror -Wall -Wextra

HEADERS = vrptools.h

default: test

test: header_check lib cineinfo magic appendix_example.cine
	file -M magic *.cine
	./cineinfo *.cine

header_check: header_check.c ${HEADERS}

get_docs:
	wget http://www.visionresearch.com/devzonedownloads/cine640.pdf

appendix_example.cine: appendix_example.txt hex2cine
	./hex2cine $<

LIBRARY = lib/libvrp.a

CFLAGS += -I.

lib/read_cine.o cineinfo.o: ${HEADERS}

lib: ${LIBRARY}
${LIBRARY}: lib/read_cine.o
	${AR} cruv $@ $<

cineinfo: cineinfo.o ${LIBRARY}

clean:
	rm -f *.o lib/*.[oa]
