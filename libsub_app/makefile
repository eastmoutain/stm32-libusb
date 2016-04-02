
CC = gcc
LDIR = /usr/loacal/lib
LFLAG = -lusb-1.0
INCLUDES = /usr/local/include/libusb-1.0

CFLAGS = -I$(INCLUDES) -std=c99

src = $(wildcard *.c)

obj = $(patsubst %.c, %.o, $(src))

.PHONY: all clean debug

all: main

main: $(obj)
	$(CC)   $(obj) -o main $(LFLAG)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@-rm -f main $(obj)

debug:
	@echo LDIR $(LDIR)
	@echo LFLAG $(LFLAG)
	@echo CFLAGS $(CFLAGS)
	@echo src $(src)
	@echo ojb $(obj)
