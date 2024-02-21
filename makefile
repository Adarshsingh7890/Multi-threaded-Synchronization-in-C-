CC=gcc
CFLAGS=-Wall -std=gnu11 -fcommon
SYNCOBJ=pe_sync.o
EXTLIBS=-pthread -lrt

all: readers_writers_test one_slot_buffer_test child_care_test monkey_crossing_test

debug: CFLAGS += -DDEBUG -g
debug: all

clean:
	rm -f *.o *.bin

.PHONY: all debug clean

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

%_test: %_test.c %.o $(SYNCOBJ)
	$(CC) $(CFLAGS) $? -o $@.bin $(EXTLIBS)
