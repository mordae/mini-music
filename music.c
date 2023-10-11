#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Last frequency selected. */
static int freq = 440;

/* Called when a key is pressed. */
void key_pressed(char c)
{
	if ('c' == c) {
		freq = 261;
	} else if ('d' == c) {
		freq = 294;
	} else if ('e' == c) {
		freq = 330;
	} else if ('f' == c) {
		freq = 349;
	} else if ('g' == c) {
		freq = 392;
	} else if ('a' == c) {
		freq = 440;
	} else if (('h' == c) || ('b' == c)) {
		freq = 494;
	} else if ('C' == c) {
		freq = 523;
	}
}

/* Generates the music samples, blocking until they have been sent out. */
void play_music(FILE *out)
{
	/* Play the first half-period. */
	for (int i = 0; i < 48000 / freq / 2; i++) {
		char x = 0;
		fwrite(&x, 1, 1, out);
	}

	/* Play the second half-period. */
	for (int i = 0; i < 48000 / freq / 2; i++) {
		char x = 255;
		fwrite(&x, 1, 1, out);
	}
}
