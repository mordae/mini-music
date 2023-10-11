#!/usr/bin/make -f

CFLAGS = -O2 -ggdb3
LDFLAGS = -lm -lcurses

all: music

music: music.c
	${CC} ${CPPFLAGS} ${CFLAGS} ${LDFLAGS} -o $@ $^

# EOF
