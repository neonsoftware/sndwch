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

void read_params()
{

	FILE *fh = fopen("./assets/cuts.yaml", "r");
	yaml_parser_t parser;
	yaml_token_t token; /* new variable */

	/* Initialize parser */
	if (!yaml_parser_initialize(&parser))
		err_quit("Failed to initialize parser!");
	if (fh == NULL)
		err_quit("Failed to open file!");

	/* Set input file */
	yaml_parser_set_input_file(&parser, fh);

	do {
		yaml_parser_scan(&parser, &token);
		switch (token.type) {
		/* Stream start/end */
		case YAML_STREAM_START_TOKEN:
			puts("STREAM START");
			break;
		case YAML_STREAM_END_TOKEN:
			puts("STREAM END");
			break;
		/* Token types (read before actual token) */
		case YAML_KEY_TOKEN:
			printf("(Key token)   ");
			break;
		case YAML_VALUE_TOKEN:
			printf("(Value token) ");
			break;
		/* Block delimeters */
		case YAML_BLOCK_SEQUENCE_START_TOKEN:
			puts("<b>Start Block (Sequence)</b>");
			break;
		case YAML_BLOCK_ENTRY_TOKEN:
			puts("<b>Start Block (Entry)</b>");
			break;
		case YAML_BLOCK_END_TOKEN:
			puts("<b>End block</b>");
			break;
		/* Data */
		case YAML_BLOCK_MAPPING_START_TOKEN:
			puts("[Block mapping]");
			break;
		case YAML_SCALAR_TOKEN:
			printf("scalar %s \n", token.data.scalar.value);
			break;
		/* Others */
		default:
			printf("Got token of type %d\n", token.type);
		}
		if (token.type != YAML_STREAM_END_TOKEN)
			yaml_token_delete(&token);
	} while (token.type != YAML_STREAM_END_TOKEN);
	yaml_token_delete(&token);

	/* Cleanup */
	yaml_parser_delete(&parser);
	fclose(fh);
}

bool parseKeyScalar(yaml_parser_t *parser, const char *keyName)
{

	yaml_token_t token;

	yaml_parser_scan(parser, &token);
	if (token.type != YAML_KEY_TOKEN)
		return false;

	yaml_parser_scan(parser, &token);
	bool isScalar = (token.type == YAML_SCALAR_TOKEN);
	if (!isScalar)
		return false;

	size_t a = strlen(keyName);
	size_t b = strlen(token.data.scalar.value);

	bool string_matches = (strcmp((const char *)token.data.scalar.value, keyName) == 0);

	return string_matches;
}

/* @brief parseValueScalarString reads, from a Yaml parser, a scalar value as a string
** @param parser Yaml parser, initialized, will read a scalar value
** @param valueBuffer pointer to buffer where to save the read string scalar
** @param valueBufferSize size of valueBuffer
** @return true in case of success, false otherwise
*/
bool parseValueScalarString(yaml_parser_t *parser, char *buf, size_t bufLen)
{

	yaml_token_t token;

	yaml_parser_scan(parser, &token);
	if (token.type != YAML_VALUE_TOKEN)
		return false;

	yaml_parser_scan(parser, &token);
	if (token.type != YAML_SCALAR_TOKEN)
		return false;

	size_t scalar_len = strlen((const char *)token.data.scalar.value);
	if (scalar_len > bufLen)
		return false;

	strncpy(buf, (char *)token.data.scalar.value, scalar_len);
	return true;
}

/* @brief parseValueScalarFloat reads, from a Yaml parser, a scalar value as a float
** @param parser Yaml parser, initialized, will read a scalar value
** @param val pointer to save the read float value
** @return true in case of success, false otherwise
*/
bool parseValueScalarFloat(yaml_parser_t *parser, float *valPtr)
{

	/* buffer to read the value in string format first */
	char buf[max_line];
	/* checking strtof result */
	char *endptr;
	int res = 0;

	/* read scalar string from yaml */
	if (!parseValueScalarString(parser, buf, max_line))
		return false;

	/* convert string to float */
	res = strtof(buf, &endptr);
	if (res == 0 && endptr == buf)
		return false;

	/* ok */
	*valPtr = res;
	return true;
}

bool parseCutFile(yaml_parser_t *parser, cut_file_t *cut)
{

	if (!parseKeyScalar(parser, "file"))
		return false;
	if (!parseValueScalarString(parser, cut->path, max_line))
		return false;
	if (!parseKeyScalar(parser, "x"))
		return false;
	if (!parseValueScalarFloat(parser, &(cut->x)))
		return false;
	if (!parseKeyScalar(parser, "y"))
		return false;
	if (!parseValueScalarFloat(parser, &(cut->y)))
		return false;
	if (!parseKeyScalar(parser, "zstart"))
		return false;
	if (!parseValueScalarFloat(parser, &(cut->zstart)))
		return false;
	if (!parseKeyScalar(parser, "zend"))
		return false;
	if (!parseValueScalarFloat(parser, &(cut->zend)))
		return false;

	printf("Cut: %s - %.1f - %.1f - %.1f - %.1f\n", cut->path, cut->x, cut->y, cut->zstart,
	       cut->zend);
	return true;
}

/**
 * @brief read_cut_files_from_yaml
 * @param filePath
 * @param cuts pointer to an array of cuts, allocated by the function, which ownership goes to the
 * caller.
 * NULL in case no cut is found
 * @param cuts_len output, number of cuts found
 * @todo of course. error checking
 */
void read_cut_files_from_yaml(const char *filePath, cut_file_t ***cuts, int *cuts_found_ptr)
{

	*cuts_found_ptr = 0;
	*cuts = (cut_file_t **)calloc(max_files, sizeof(cut_file_t *));

	/* yaml parsing structs */
	yaml_parser_t parser;
	yaml_token_t token;

	/* parsing flags */
	bool end_reached = false;	/* more readable way to step out of next loop */
	bool file_parse_success = false; /* more readable way to step out of next loop */

	/* File to parse */
	FILE *fh = NULL;

	/* Open file and initialize parser */
	fh = fopen(filePath, "r");
	if (!yaml_parser_initialize(&parser))
		err_quit("Failed to initialize parser!");
	if (fh == NULL)
		err_quit("Failed to open file!");
	yaml_parser_set_input_file(&parser, fh);

	/* attempt to read a cut_file_t. one per iteration */
	do {
		/* skipping to first YAML_BLOCK_MAPPING_START_TOKEN */
		do {
			yaml_parser_scan(&parser, &token);
			if (token.type == YAML_STREAM_END_TOKEN) /* if end, exit */
				end_reached = true;
		} while ((!end_reached) && token.type != YAML_BLOCK_MAPPING_START_TOKEN);

		/* ready to read next file. allocating cut and read the file */
		cut_file_t *new_cut_file = (cut_file_t *)calloc(1, sizeof(cut_file_t));
		file_parse_success = parseCutFile(&parser, new_cut_file);
		if (file_parse_success) {
			(*cuts)[*cuts_found_ptr] = new_cut_file; /* storing new file */
			*cuts_found_ptr = (*cuts_found_ptr) + 1; /* increasing found count */
		} else {
			/* deallocating cut that was ready to be read, but was not */
			free(new_cut_file);
		}
	} while ((!end_reached) && file_parse_success);

	/* Cleanup */
	yaml_token_delete(&token);
	yaml_parser_delete(&parser);
	fclose(fh);
}

/*
 *
 *
 *
 */

int main()
{

	snd_err_t res = SWC_OK;

	/*------------- Reading parameters -----------------------*/
	{
		cut_file_t **cuts;
		int cuts_len;
		read_cut_files_from_yaml("./assets/cuts.yaml", &cuts, &cuts_len);

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
