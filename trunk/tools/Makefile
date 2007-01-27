DEBUG=1
CFLAGS+=-I. -I../libpl2 -std=c99
ifdef DEBUG
CFLAGS+=-DDEBUG -g
LDFLAGS+=-g
endif
LDFLAGS+=-L../libpl2 -lpl2

TOOLS=unpl2 chktmb
TOOLS_DEPS=../libpl2/libpl2.a

all: $(TOOLS) $(TOOLS_DEPS)

%: %.c $(TOOLS_DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	$(RM) $(TOOLS)
