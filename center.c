#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <locale.h>

#define ERR(str) { perror(str); err = EXIT_FAILURE; break; }

int main()
{
	if (setlocale(LC_ALL, "") == NULL) {
		perror("setlocale");
		exit(EXIT_FAILURE);
	}

	char *ptr = NULL;
	size_t siz = 0;
	ssize_t slen;
	int err = EXIT_SUCCESS;

	while ((slen = getline(&ptr, &siz, stdin)) > 0) {
		ptr[slen - 1] = '\0';
	
		size_t len = mbstowcs(NULL, ptr, 0);
		if (len == (size_t) -1) ERR("msbtowcs")

		wchar_t *wcs = calloc(len + 1, sizeof *wcs);
		if (!wcs) ERR("calloc")

		if (mbstowcs(wcs, ptr, len + 1) == (size_t) -1) ERR("msbtowcs")

		struct winsize ws;
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) < 0) ERR("ioctl")

		int trm_width = ws.ws_col;
		int str_width = wcswidth(wcs, len);

		free(wcs);

		for (int i = (trm_width - str_width) / 2; i > 0; i--)
			putchar(' ');
		puts(ptr);
	}

	if (ptr)
		free(ptr);

	return err;
}
