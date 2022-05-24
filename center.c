#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE
#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>

#define ERR(str) { perror(str); err = EXIT_FAILURE; goto end; }

int main()
{
	if (setlocale(LC_ALL, "") == NULL) {
		perror("setlocale");
		exit(EXIT_FAILURE);
	}

	char *buf = NULL;
	size_t siz = 0;
	ssize_t len;
	int err = EXIT_SUCCESS;

	int tty_fd = open("/dev/tty", O_RDWR);
	if (tty_fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	while ((len = getline(&buf, &siz, stdin)) > 0) {
		struct winsize ws;
		if (ioctl(tty_fd, TIOCGWINSZ, &ws) < 0) ERR("ioctl")

		int term_width = ws.ws_col;

		char *ptr = buf;
		char *last = ptr;
		int str_width = 0;
		mbstate_t mbs = {0};

		while (len > 0) {
			wchar_t wc;

			size_t adv = mbrtowc(&wc, ptr, len, &mbs);
			if (adv == (size_t) -1 || adv == (size_t) -2) ERR("mbrtowc")

			int width = wcwidth(wc);
			if (width > 0)
				str_width += width;

			if (*ptr == '\n' || str_width >= term_width - 12) {
				for (int i = (term_width - str_width) / 2; i > 0; i--)
					putchar(' ');
				fwrite(last, 1, ptr - last + 1, stdout);

				if (*ptr != '\n')
					putchar('\n');

				last = ptr + adv;
				str_width = 0;
			}

			ptr += adv;
			len -= adv;
		}
	}

	end:
	close(tty_fd);

	if (buf)
		free(buf);

	return err;
}
