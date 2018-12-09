CC ?= gcc
RM ?= rm -rf
STRIP ?= strip

CFLAGS += -Wall -O2 -std=c99 -Iinclude

DEP_CORE=  $(wildcard include/*.h)

SRC_CORE=  pdkformat/pdkformat.c
SRC_CORE+= emucpu/emucpu.c
SRC_CORE+= opcode/opcode13.c opcode/opcode14.c opcode/opcode15.c opcode/opcode16.c
SRC_CORE+= disass/disass13.c disass/disass14.c disass/disass15.c disass/disass16.c
SRC_CORE+= cpuvariant/pmx150.c cpuvariant/pmx154.c
OBJ_CORE=  $(subst .c,.o,$(SRC_CORE))

all: depdk dispdk emupdk

depdk: $(DEP_CORE) $(OBJ_CORE) depdk.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o depdk depdk.c pdkformat/pdkformat.o $(LIBS)
	$(STRIP) depdk

dispdk: $(DEP_CORE) $(OBJ_CORE) dispdk.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o dispdk dispdk.c $(OBJ_CORE) $(LIBS)
	$(STRIP) depdk

emupdk: $(DEP_CORE) $(OBJ_CORE) emupdk.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o emupdk emupdk.c $(OBJ_CORE) $(LIBS)
	$(STRIP) depdk

clean:
	$(RM) $(OBJ_CORE)
	$(RM) depdk
	$(RM) dispdk
	$(RM) emupdk

dist-clean: clean
