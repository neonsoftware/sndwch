#include "sndwch.h"
#include <errno.h>  /* for definition of errno */
#include <stdarg.h> /* ISO C variable aruments */
#include <stdio.h>
#include <stdlib.h>
#include <yaml.h>
#include <yaml.h>
#include <stdbool.h>

static const size_t max_line = 4096; /* max line length */

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

void read_params(){

    FILE *fh = fopen("./assets/cuts.yaml", "r");
    yaml_parser_t parser;
      yaml_token_t  token;   /* new variable */

     /* Initialize parser */
     if(!yaml_parser_initialize(&parser))
       err_quit("Failed to initialize parser!");
     if(fh == NULL)
       err_quit("Failed to open file!");

     /* Set input file */
     yaml_parser_set_input_file(&parser, fh);

     do {
       yaml_parser_scan(&parser, &token);
       switch(token.type)
       {
       /* Stream start/end */
       case YAML_STREAM_START_TOKEN: puts("STREAM START"); break;
       case YAML_STREAM_END_TOKEN:   puts("STREAM END");   break;
       /* Token types (read before actual token) */
       case YAML_KEY_TOKEN:   printf("(Key token)   "); break;
       case YAML_VALUE_TOKEN: printf("(Value token) "); break;
       /* Block delimeters */
       case YAML_BLOCK_SEQUENCE_START_TOKEN: puts("<b>Start Block (Sequence)</b>"); break;
       case YAML_BLOCK_ENTRY_TOKEN:          puts("<b>Start Block (Entry)</b>");    break;
       case YAML_BLOCK_END_TOKEN:            puts("<b>End block</b>");              break;
       /* Data */
       case YAML_BLOCK_MAPPING_START_TOKEN:  puts("[Block mapping]");            break;
       case YAML_SCALAR_TOKEN:  printf("scalar %s \n", token.data.scalar.value); break;
       /* Others */
       default:
         printf("Got token of type %d\n", token.type);
       }
       if(token.type != YAML_STREAM_END_TOKEN)
         yaml_token_delete(&token);
     } while(token.type != YAML_STREAM_END_TOKEN);
     yaml_token_delete(&token);

     /* Cleanup */
     yaml_parser_delete(&parser);
     fclose(fh);
}

typedef struct {
        char path[swc_max_path]; /**< path of the SVG file */
        float x;		 /**< x offset */
        float y;		 /**< y offset */
        float zstart;	 /**< y offset */
        float zend;	 /**< y offset */
} cut_file_t;

bool parseKeyScalar(yaml_parser_t *parser, const char *keyName){

    yaml_token_t  token;

    yaml_parser_scan(parser, &token);
    if(token.type != YAML_KEY_TOKEN)
        return false;

    yaml_parser_scan(parser, &token);
    bool isScalar = (token.type == YAML_SCALAR_TOKEN);
    if( !isScalar )
        return false;

    size_t a = strlen(keyName);
    size_t b = strlen(token.data.scalar.value);

    bool string_matches = ( strcmp((const char*)token.data.scalar.value, keyName) == 0);

    return string_matches;
}

/* @brief parseValueScalarString reads, from a Yaml parser, a scalar value as a string
** @param parser Yaml parser, initialized, will read a scalar value
** @param valueBuffer pointer to buffer where to save the read string scalar
** @param valueBufferSize size of valueBuffer
** @return true in case of success, false otherwise
*/
bool parseValueScalarString(yaml_parser_t *parser, char *buf, size_t bufLen){

    yaml_token_t  token;

    yaml_parser_scan(parser, &token);
    if(token.type != YAML_VALUE_TOKEN)
        return false;

    yaml_parser_scan(parser, &token);
    if(token.type != YAML_SCALAR_TOKEN)
        return false;

    size_t scalar_len = strlen((const char *)token.data.scalar.value);
    if(scalar_len > bufLen)
        return false;

    strncpy(buf, (char *)token.data.scalar.value, scalar_len);
    return true;
}

/* @brief parseValueScalarFloat reads, from a Yaml parser, a scalar value as a float
** @param parser Yaml parser, initialized, will read a scalar value
** @param val pointer to save the read float value
** @return true in case of success, false otherwise
*/
bool parseValueScalarFloat(yaml_parser_t *parser, float *valPtr){

    /* buffer to read the value in string format first */
    char buf[max_line];
    /* checking strtof result */
    char *endptr;
    int res = 0;

    /* read scalar string from yaml */
    if( ! parseValueScalarString(parser, buf, max_line) )
        return false;

    /* convert string to float */
    res = strtof(buf, &endptr);
    if( res == 0 && endptr == buf )
        return false;

    /* ok */
    *valPtr = res;
    return true;
}

bool parseCutFile(yaml_parser_t *parser, cut_file_t *cut){

    if( ! parseKeyScalar(parser, "file") )
        return false;
    if( ! parseValueScalarString(parser, cut->path, max_line) )
        return false;
    if( ! parseKeyScalar(parser, "x") )
        return false;
    if( ! parseValueScalarFloat(parser, &(cut->x)) )
        return false;
    if( ! parseKeyScalar(parser, "y") )
        return false;
    if( ! parseValueScalarFloat(parser, &(cut->y)) )
        return false;
    if( ! parseKeyScalar(parser, "zstart") )
        return false;
    if( ! parseValueScalarFloat(parser, &(cut->zstart)) )
        return false;
    if( ! parseKeyScalar(parser, "zend") )
        return false;
    if( ! parseValueScalarFloat(parser, &(cut->zend)) )
        return false;

    printf("Cut: %s - %.1f - %.1f - %.1f - %.1f\n",
           cut->path, cut->x, cut->y, cut->zstart, cut->zend);
    return true;
}

void my_read_params(){

    FILE *fh = fopen("./assets/cuts.yaml", "r");
    yaml_parser_t parser;
      yaml_token_t  token;   /* new variable */

     /* Initialize parser */
     if(!yaml_parser_initialize(&parser))
       err_quit("Failed to initialize parser!");
     if(fh == NULL)
       err_quit("Failed to open file!");

     /* Set input file */
     yaml_parser_set_input_file(&parser, fh);

     cut_file_t current_file;

     /* skipping to first YAML_BLOCK_MAPPING_START_TOKEN */
     bool keep = true; /* more readable way to step out of next loop */
     do{
        do{
             yaml_parser_scan(&parser, &token);
            if(token.type == YAML_STREAM_END_TOKEN) /* if end, exit */
                 keep = false;
        }while( keep && token.type != YAML_BLOCK_MAPPING_START_TOKEN );
     }while( keep && parseCutFile(&parser, &current_file) );

     yaml_token_delete(&token);

     /* Cleanup */
     yaml_parser_delete(&parser);
     fclose(fh);
}


int main()
{

	snd_err_t res = SWC_OK;

        /*------------- Reading parameters -----------------------*/
        {
            //read_params();
            my_read_params();
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
