#include "sndwch.h"
#include <errno.h>  /* for definition of errno */
#include <stdarg.h> /* ISO C variable aruments */
#include <stdbool.h>
#include <assert.h>
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

int main(int argc, const char* argv[])
{

	snd_err_t res = SWC_OK;

        /* config file read */
        cut_file_t **cuts;
        cut_file_t **cuts_sliced;
        size_t cuts_len;
        size_t cuts_sliced_len;

        /*------------- sort and comparison ------------*/
        if(argc == 2 && (strcmp(argv[1], "cutorder") == 0)){
            cut_file_t aaa = {"aaa.svg", 0, 1};
            cut_file_t aab = {"aab.svg", 0, 1};
            cut_file_t baa = {"baa.svg", 0, 1};

            /* cutcmp */
            assert( swc_cutcmp(&aaa, &aaa) == 0 );
            assert( swc_cutcmp(&aaa, &aab) == -1 );
            assert( swc_cutcmp(&aab, &aaa) == 1 );
            assert( swc_cutcmp(&aab, &baa) == -1 );
            assert( swc_cutcmp(&aaa, &baa) == -1 );

            /* cutsort */
            cut_file_t *c[3] = {&aaa, &baa, &aab};
            swc_cutsort(c, 3);
            assert( strcmp(c[0]->path, aaa.path) == 0 );
            assert( strcmp(c[1]->path, aab.path) == 0 );
            assert( strcmp(c[2]->path, baa.path) == 0 );
            return 0;
        }

        if(argc == 2 && (strcmp(argv[1], "layerorder") == 0)){

            swc_layer_t la = { {}, 0, -2.0, -0.5 };
            swc_layer_t lb = { {}, 0, 0.5, 1.0 };
            swc_layer_t lc = { {}, 0, 2.0, 3.0 };

            assert( swc_layercmp(&la, &la) == 0 );
            assert( swc_layercmp(&la, &lb) == -1 );
            assert( swc_layercmp(&lb, &la) == 1 );
            assert( swc_layercmp(&lb, &lc) == -1 );
            assert( swc_layercmp(&la, &lc) == -1 );

            swc_layer_t *s[3] = {&lc, &la, &lb};

            swc_layersort(s, 3);
            assert( s[0]->zstart == -2.0 );
            assert( s[1]->zstart == 0.5 );
            assert( s[2]->zstart == 2.0 );
            return 0;
        }
        /*--------------------------------------------------------*/


	/*------------- Reading parameters -----------------------*/
	{
                /* prepare file */
                FILE *fp = fopen("./log_config_parse.txt", "w+");
                if( fp == NULL )
                    return 1;

                swc_read_conf_file("./assets/cuts.yaml", &cuts, &cuts_len);

		for (size_t i = 0; i < cuts_len; i++) {
			cut_file_t *cut_ptr = cuts[i];
                        fprintf(fp, "%s - %.1f - %.1f - %.1f - %.1f\n", cut_ptr->path, cut_ptr->x,
			       cut_ptr->y, cut_ptr->zstart, cut_ptr->zend);
		}

                fclose(fp);
        }

	/*------------- merging two files -----------------------*/
	{
                const char *p1 = "./assets/a.svg";
                const char *p2 = "./assets/b.svg";
		const char *paths[2];
		paths[0] = p1;
		paths[1] = p2;
		res = swc_merge(paths, 2, "./merged.svg");
		if (res != SWC_OK)
			err_quit("Error Merging.");
	}
	/*--------------------------------------------------------*/


        /*------------- reading and merging two files ------------*/
        {

                const char *paths[2];
                paths[0] = cuts[0]->path;
                paths[1] = cuts[1]->path;

                res = swc_merge((const char **)paths, 2, "./parsed_and_merged.svg");
                if (res != SWC_OK)
                        err_quit("Error Merging.");
        }
        /*--------------------------------------------------------*/

        /*------------- reading and merging two files ------------*/
        {
                res = swc_translate_and_merge(cuts, cuts_len, "./translated.svg");
                if (res != SWC_OK)
                        err_quit("Error Merging.");
        }
        /*--------------------------------------------------------*/

        /*------------- slicing files ------------*/
        {
                FILE *fp = fopen("./log_slice.txt", "w+");
                if( fp == NULL )
                        return 1;

                res = swc_slice(cuts, cuts_len, &cuts_sliced, &cuts_sliced_len);
                if (res != SWC_OK)
                        err_quit("Error Merging.");

                for (size_t i = 0; i < cuts_sliced_len; i++) {
                        cut_file_t *cut_ptr = cuts_sliced[i];
                        fprintf(fp, "%s - %.1f - %.1f - %.1f - %.1f\n", cut_ptr->path, cut_ptr->x,
                               cut_ptr->y, cut_ptr->zstart, cut_ptr->zend);
                }
        }
        /*--------------------------------------------------------*/

	return 0;
}
