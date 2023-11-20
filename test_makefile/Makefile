CC=gcc
CFLAGS=-ansi -g

cs240StreamingService: main.c streaming_service.c streaming_service.h  
	$(CC) $(CFLAGS) main.c streaming_service.c -o $@

.PHONY: clean

clean:
	rm -f cs240StreamingService
