#include "sndwch.h"
#include <errno.h>  /* for definition of errno */
#include <stdarg.h> /* ISO C variable aruments */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <yaml.h>
#include <yaml.h>

static const size_t max_line = 4096; /* max line length */
static const size_t max_files = 200; /* max line length */

/*
** Print a message and return to caller.
** Caller specifies "errnoflag".
*/
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char buf[max_line];

	vsnprintf(buf, max_line - 1, fmt, ap);

	if (errnoflag)
		snprintf(buf + strlen(buf), max_line - strlen(buf) - 1, ": %s", strerror(error));

	strcat(buf, "\n");
	fflush(stdout); /* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL); /* flushes all stdio output streams */
}

/*
** Fatal error unrelated to a system call.
** Print a message and terminate.
*/
void err_quit(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
** Fatal error related to a system call.
** Print a message, dump core, and terminate.
*/
void err_dump(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	abort(); /* dump core and terminate */
	exit(1); /* shouldn′t get here */
}

int main()
{

	snd_err_t res = SWC_OK;

	/*------------- Reading parameters -----------------------*/
	{
		cut_file_t **cuts;
		int cuts_len;
                swc_read_conf_file("./assets/cuts.yaml", &cuts, &cuts_len);

		for (size_t i = 0; i < cuts_len; i++) {
			cut_file_t *cut_ptr = cuts[i];
			printf("found: %s - %.1f - %.1f - %.1f - %.1f\n", cut_ptr->path, cut_ptr->x,
			       cut_ptr->y, cut_ptr->zstart, cut_ptr->zend);
		}
	}

	/*------------- merging two files -----------------------*/
	{
		const char *p1 = "./assets/a.svg";
		const char *p1_ = "/opt/garage/sndwch/test/assets/a.svg";
		const char *p2 = "./assets/b.svg";
		const char *p2_ = "/opt/garage/sndwch/test/assets/b.svg";
		const char *paths[2];
		paths[0] = p1;
		paths[1] = p2;
		res = swc_merge(paths, 2, "./merged.svg");
		if (res != SWC_OK)
			err_quit("Error Merging.");
	}
	/*--------------------------------------------------------*/

	return 0;
}
