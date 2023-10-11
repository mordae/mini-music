#!/usr/bin/make -f

CPPFLAGS = -D_GNU_SOURCE
CFLAGS = -O2 -ggdb3
LDFLAGS = -lm -lcurses

all: music

music: main.c music.c
	${CC} ${CPPFLAGS} ${CFLAGS} ${LDFLAGS} -o $@ $^

# EOF
