#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Last period selected. */
static int period = 48000 / 440;
static float strength = 0.0;
static float decay = 1.0;

enum mode {
	MODE_SINE = 0,
	MODE_SQUARE,
	MODE_SAW,
	MODE_STRING,
	MODE_MAX,
};

static enum mode mode = MODE_SINE;
static int delay[48000 / 100];

enum tone {
	KEY_NONE = 0,
	KEY_C4,
	KEY_D4,
	KEY_E4,
	KEY_F4,
	KEY_G4,
	KEY_A4,
	KEY_H4,
	KEY_C5,
	KEY_D5,
	KEY_E5,
	KEY_F5,
	KEY_MAX,
};

enum tone key_tone_map[256] = {
	['<'] = KEY_C4, ['z'] = KEY_D4, ['x'] = KEY_E4, ['c'] = KEY_F4,
	['v'] = KEY_G4, ['b'] = KEY_A4, ['n'] = KEY_H4, ['m'] = KEY_C5,
	[','] = KEY_D5, ['.'] = KEY_E5, ['/'] = KEY_F5,
};

float tone_freq_map[KEY_MAX] = {
	[KEY_C4] = 261.6256, [KEY_D4] = 293.6648, [KEY_E4] = 329.6276, [KEY_F4] = 349.2282,
	[KEY_G4] = 391.9954, [KEY_A4] = 440.0000, [KEY_H4] = 493.8833, [KEY_C5] = 523.2511,
	[KEY_D5] = 587.3295, [KEY_E5] = 659.2551, [KEY_F5] = 698.4565,
};

/* Called when a key is pressed. */
void key_pressed(char c)
{
	if ((c >= '1') && (c < '1' + MODE_MAX)) {
		mode = c - '1';
		return;
	}

	enum tone tone = key_tone_map[(int)c];

	if (!tone)
		return;

	float freq = tone_freq_map[tone];
	period = 48000.0 / freq;
	strength = 0.5;
	decay = 440.0 / freq;

	if (MODE_STRING == mode) {
		for (int i = 0; i < 48000 / 100; i++)
			delay[i] = (short)(rand() - RAND_MAX / 2);
	}
}

static void play_sine(FILE *out)
{
	for (int i = 0; i < period; i++) {
		short x = 32000 * sinf(i * 2.0 * M_PI / period) * strength;
		fwrite(&x, 2, 1, out);
	}
}

static void play_square(FILE *out)
{
	/* Play the first half-period. */
	for (int i = 0; i < period / 2; i++) {
		short x = 32000 * strength * 0.707;
		fwrite(&x, 2, 1, out);
	}

	/* Play the second half-period. */
	for (int i = 0; i < period / 2; i++) {
		short x = -32000 * strength * 0.707;
		fwrite(&x, 2, 1, out);
	}
}

static void play_saw(FILE *out)
{
	for (int i = 0; i < period; i++) {
		short x = i * 32000 / period * strength;
		fwrite(&x, 2, 1, out);
	}
}

static void play_string(FILE *out)
{
	short x = delay[0];
	delay[0] = 4095 * (delay[0] * 4 + 4 * delay[period - 1]) / 4096 / 8;
	fwrite(&x, 2, 1, out);

	for (int i = 1; i < period; i++) {
		x = delay[i];
		delay[i] = 4095 * (delay[i] * 4 + 4 * delay[i - 1]) / 4096 / 8;
		fwrite(&x, 2, 1, out);
	}
}

/* Generates the music samples, blocking until they have been sent out. */
void play_music(FILE *out)
{
	switch (mode) {
	case MODE_SINE:
		play_sine(out);
		break;

	case MODE_SQUARE:
		play_square(out);
		break;

	case MODE_SAW:
		play_saw(out);
		break;

	case MODE_STRING:
		play_string(out);

	case MODE_MAX:
		/* No idea. */
	}

	strength -= 0.002 * decay;
	if (strength < 0.0)
		strength = 0;
}
