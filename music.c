#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>

static void handler(int signo)
{
	nocbreak();
	echo();
	endwin();
	exit(signo);
}

static void key_pressed(char c)
{
	printf("%c\r\n", c);
}

static void play_music()
{
}

int main()
{
	signal(SIGINT, handler);
	signal(SIGTERM, handler);

	initscr();
	noecho();
	cbreak();
	timeout(-1);

	while (1) {
		int c;

		while ((c = getch()) >= 0) {
			key_pressed(c);
		}

		play_music();
		usleep(100 * 1000);
	}

	handler(0);
	return 0;
}
