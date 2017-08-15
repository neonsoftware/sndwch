#include "sndwch.h"
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>


/**
 * exampleFunc:
 * @filename: a filename or an URL
 *
 * Parse and validate the resource and free the resulting tree
makeg */
static void
exampleFunc(const char *filename) {
    xmlParserCtxtPtr ctxt; /* the parser context */
    xmlDocPtr doc; /* the resulting document tree */

    /* create a parser context */
    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
        fprintf(stderr, "Failed to allocate parser context\n");
	return;
    }
    /* parse the file, activating the DTD validation option */
    doc = xmlCtxtReadFile(ctxt, filename, NULL, 0);
    /* check if parsing suceeded */
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", filename);
    } else {
	/* check if validation suceeded */
        if (ctxt->valid == 0)
	    fprintf(stderr, "Failed to validate %s\n", filename);
	/* free up the resulting document */
	xmlFreeDoc(doc);
    }
    /* free up the parser context */
    xmlFreeParserCtxt(ctxt);
}

snd_err_t swc_merge(const char ** in_paths, size_t in_paths_size, const char* out_path){

  LIBXML_TEST_VERSION

  for (size_t i = 0; i < in_paths_size; ++i) {
    exampleFunc(in_paths[i]); 
  }

  return SWC_OK;
}

/* reads all the SVG elements of a file into a buffer */
snd_err_t importSVGElementsFromFile(const char *path, char *buf, size_t buf_len)
{
	return SWC_OK;
}

/* */
int isCutEquivalent(cut_2d_t *a, cut_2d_t *b)
{
	if (strcmp(a->path, b->path) == 0 && a->x == b->x && a->y == b->y)
		return 0;
	else
		return 1;
}

snd_err_t svg_group_end(char *in) { return SWC_OK; }



