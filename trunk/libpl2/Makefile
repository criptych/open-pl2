
DEBUG=1
CFLAGS+=-I. -std=c99 -Wall -DWITH_ICONV=yes
ifdef DEBUG
CFLAGS+=-DDEBUG -g
LDFLAGS+=-g
endif
LDFLAGS+=-L.
LIBRARY_OBJECTS=attr.o msg.o pak.o std.o tmb.o
LIBRARY_DEPS=libpl2.h

all: libpl2.a $(LIBRARY_DEPS)
	

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

libpl2.a: $(LIBRARY_OBJECTS) $(LIBRARY_DEPS)
	$(AR) cr $@ $(LIBRARY_OBJECTS)

clean:
	$(RM) $(LIBRARY_OBJECTS:.c=.否.c) libpl2.a
