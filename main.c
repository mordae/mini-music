#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

/* Pipe to send our music into. */
static FILE *audio;
static pid_t cpid;

/* From "music.c". */
void key_pressed(char c);
void play_music(FILE *audio);

/* Resets the terminal when we CTRL-C the program. */
static void handler(int signo)
{
	nocbreak();
	echo();
	endwin();
	exit(signo);
}

/* Start audio player (aplay) as our child. */
static void spawn_audio_player()
{
	int pipefd[2];

	/* Create a pipe to send music through. */
	if (pipe(pipefd) < 0) {
		perror("pipe");
		exit(1);
	}

	/* Shrink the pipe to 4096 bytes. */
	fcntl(pipefd[0], F_SETPIPE_SZ, sizeof(size_t));

	if ((cpid = fork()) < 0) {
		perror("fork");
		exit(1);
	}

	if (0 == cpid) {
		/* We are the child. Make the pipe our input. */
		dup2(pipefd[0], 0);

		/* Do not use the other end. */
		close(pipefd[1]);

		/* Become audio player. */
		execlp("aplay", "aplay", "-f", "S16_LE", "-c", "1", "-r", "48000", "-q", "-B", "1",
		       NULL);

		/* If it did not work out, fail. */
		perror("execlp");
		exit(1);
	} else {
		/* We are the parent. Make sure we can write to the child. */
		if (NULL == (audio = fdopen(pipefd[1], "a"))) {
			perror("fdopen");
			exit(1);
		}

		/* Disable buffering to keep player responsive. */
		setvbuf(audio, NULL, _IONBF, 0);

		/* Close the other end. */
		close(pipefd[0]);
	}
}

int main()
{
	spawn_audio_player();

	signal(SIGINT, handler);
	signal(SIGTERM, handler);

	initscr();
	noecho();
	cbreak();
	timeout(0);

	while (1) {
		int c;

		while ((c = getch()) >= 0) {
			if (c <= 4)
				handler(0);

			key_pressed(c);
		}

		play_music(audio);
	}

	return 0;
}
