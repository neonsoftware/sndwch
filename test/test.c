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

        /* different path */
        cut_file_t aaa = {"aaa.svg", 0, 1};
        cut_file_t aab = {"aab.svg", 0, 1};
        cut_file_t baa = {"baa.svg", 0, 1};

        swc_layer_t la = { {&aaa}, 1, -2.0, -0.5 };
        swc_layer_t lb = { {&aab, &baa}, 2, 0.5, 1.0 };
        swc_layer_t lc = { {&aab, &baa}, 2, 2.0, 3.0 };

        /*------------- sort and comparison ------------*/
        if(argc == 2 && (strcmp(argv[1], "cutorder") == 0)){

            /* to aaa, same path, different x */
            cut_file_t aaax = {"aaa.svg", 1, 1};
            /* to aaa, different y */
            cut_file_t aaay = {"aaa.svg", 0, 2};
            /* same as aaa */
            cut_file_t aaasame = {"aaa.svg", 0, 1};


            /* cutcmp , error */
            assert( swc_cutcmp(NULL, &aaa) == -2 );
            assert( swc_cutcmp(&aaa, NULL) == -2 );
            assert( swc_cutcmp(NULL, NULL) == -2 );

            /* cutcmp, valid */
            /* by path */
            assert( swc_cutcmp(&aaa, &aab) == -1 );
            assert( swc_cutcmp(&aab, &aaa) == 1 );
            assert( swc_cutcmp(&aab, &baa) == -1 );
            assert( swc_cutcmp(&aaa, &baa) == -1 );

            /* by x */
            assert( swc_cutcmp(&aaa, &aaax) == -1 );
            assert( swc_cutcmp(&aaax, &aaa) == 1 );
            assert( swc_cutcmp(&aaay, &aaax) == -1 );
            assert( swc_cutcmp(&aaax, &aaay) == 1 );

            /* by y */
            assert( swc_cutcmp(&aaa, &aaay) == -1 );
            assert( swc_cutcmp(&aaay, &aaa) == 1 );

            /* same */
            assert( swc_cutcmp(&aaa, &aaasame) == 0 );

            /* cutsort */
            cut_file_t *c[5] = {&aaax, &baa, &aaa, &aab, &aaay};
            swc_cutsort(c, 5);
            assert( strcmp(c[0]->path, aaa.path) == 0 );
            assert( strcmp(c[1]->path, aaay.path) == 0 );
            assert( strcmp(c[2]->path, aaax.path) == 0 );
            assert( strcmp(c[3]->path, aab.path) == 0 );
            assert( strcmp(c[4]->path, baa.path) == 0 );
            return 0;
        }

        if(argc == 2 && (strcmp(argv[1], "layerorder") == 0)){

            /* layercmp, error */
            assert( swc_layercmp(NULL, &la) == -2 );
            assert( swc_layercmp(&la, NULL) == -2 );
            assert( swc_layercmp(NULL, NULL) == -2 );

            /* layercmp, valid */
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


        /*---------------------- Neighbors -----------------------*/
        if(argc == 2 && (strcmp(argv[1], "neighbors") == 0)){

            swc_layer_t lbneighbor = { {&aab, &baa}, 2, 1.0, 1.5 };

            /* layercmp, error */
            assert( swc_layer_equivalent_neighbors(&lb, NULL) == -2 );
            assert( swc_layer_equivalent_neighbors(NULL, &lb) == -2 );
            assert( swc_layer_equivalent_neighbors(NULL, NULL) == -2 );

            /* layer, valid */
            assert( swc_layer_equivalent_neighbors(&lb, &lc) == -1 );
            assert( swc_layer_equivalent_neighbors(&lb, &lbneighbor) == 0 );

            return 0;
        }
        /*--------------------------------------------------------*/

        /*---------------------- Layers -----------------------*/
        if(argc == 2 && (strcmp(argv[1], "layer") == 0)){

            swc_layer_t **layers = NULL;
            size_t layers_len = 0;

            cut_file_t c1 = {"c1.svg", 0, 0, -10, -9};
            cut_file_t c2 = {"c2.svg", 0, 0, -1, 0};
            cut_file_t c3 = {"c3.svg", 0, 0, 2.0,3.0};
            cut_file_t c4 = {"c4.svg", 0, 0, -10, -9};
            cut_file_t c5 = {"c5.svg", 0, 0, -1, 0};

            cut_file_t *cs[5] = {&c1, &c2, &c3, &c4, &c5};

            swc_layer(cs, 5, &layers, &layers_len);

            assert( layers_len == 3 );

            assert( (layers[0])->zstart == -10 );
            assert( (layers[0])->cuts_len == 2);
            assert( (layers[1])->zstart == -1 );
            assert( (layers[1])->cuts_len == 2);
            assert( (layers[2])->zstart == 2.0 );
            assert( (layers[2])->cuts_len == 1);

            return 0;
        }
        /*--------------------------------------------------------*/

        /*---------------------- Minimise -----------------------*/
        if(argc == 2 && (strcmp(argv[1], "minimise") == 0)){

            swc_layer_t **layers = NULL;
            size_t layers_len = 0;
            swc_layer_t **layers_mini = NULL;
            size_t layers_mini_len = 0;

            cut_file_t c1 = {"a.svg", 0, 0, -10, -9.5};
            cut_file_t c2 = {"a.svg", 0, 0, -9.5, -9.0};
            cut_file_t c3 = {"b.svg", 0, 0, -9.0, -8.5};
            cut_file_t c4 = {"c.svg", 0, 0, -8.5, -8.0};
            cut_file_t c5 = {"c.svg", 0, 0, -8.0, -7.5};

            cut_file_t *cs[5] = {&c1, &c2, &c3, &c4, &c5};

            swc_layer(cs, 5, &layers, &layers_len);

            assert( layers_len == 5 );
            assert( (layers[0])->zstart == -10 );
            assert( (layers[0])->cuts_len == 1);
            assert( (layers[1])->zstart == -9.5 );
            assert( (layers[1])->cuts_len == 1);
            assert( (layers[2])->zstart == -9.0 );
            assert( (layers[2])->cuts_len == 1);

            swc_minimize_layers(layers, layers_len, &layers_mini, &layers_mini_len);

            assert( layers_mini_len == 3 );

            assert( (layers_mini[0])->zstart == -10 );
            assert( (layers_mini[0])->zend == -9 );
            assert( (layers_mini[0])->cuts_len == 1);
            assert( (layers_mini[1])->zstart == -9.0 );
            assert( (layers_mini[1])->zend == -8.5 );
            assert( (layers_mini[1])->cuts_len == 1);
            assert( (layers_mini[2])->zstart == -8.5 );
            assert( (layers_mini[2])->zend == -7.5 );
            assert( (layers_mini[2])->cuts_len == 1);

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
