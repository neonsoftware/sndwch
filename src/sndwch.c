#include "sndwch.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <stdio.h>

#define MY_ENCODING "ISO-8859-1"

/**
 * exampleFunc:
 * @filename: a filename or an URL
 *
 * Parse and validate the resource and free the resulting tree
makeg */

static snd_err_t openAndCopyRootNode(const char *filename, xmlNodePtr node_to_copy)
{
	xmlParserCtxtPtr ctxt; /* the parser context */
	xmlDocPtr doc;	 /* the resulting document tree */

	/* create a parser context */
	ctxt = xmlNewParserCtxt();
	if (ctxt == NULL) {
	  return SWC_ERR_ALLOC;
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

		xmlNodePtr rootNode = xmlDocGetRootElement(doc);
		if (rootNode == NULL) {
			printf("testXmlwriterTree: Error creating the xml node\n");
			return SWC_ERR_IN_FILE;
		}

		xmlNodePtr node_copy = xmlCopyNode(rootNode, 1);
		if (node_copy == NULL) {
			printf("testXmlwriterTree: Error creating the xml node\n");
			return SWC_ERR_IN_FILE ;
		}
		xmlAddChildList(node_to_copy, node_copy->children);

		/* free up the resulting document */
		xmlFreeDoc(doc);
	}
	/* free up the parser context */
	xmlFreeParserCtxt(ctxt);
}

snd_err_t swc_merge(const char **in_paths, size_t in_paths_size, const char *out_path)
{
	int res;
	xmlDocPtr doc;
	xmlNodePtr svgRoot;

	LIBXML_TEST_VERSION

	/* creating base document */
	doc = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
	if (doc == NULL) {
		printf("testXmlwriterTree: Error creating the xml document tree\n");
		return SWC_ERR_ALLOC;
	}
	svgRoot = xmlNewDocNode(doc, NULL, BAD_CAST "svg", NULL);
	xmlNewProp(svgRoot, BAD_CAST "xmlns", BAD_CAST "http://www.w3.org/2000/svg");
	xmlNewProp(svgRoot, BAD_CAST "xmlns:xlink", BAD_CAST "http://www.w3.org/1999/xlink");
	xmlDocSetRootElement(doc, svgRoot);

	/* add all files' nodes */
	for (size_t i = 0; i < in_paths_size; ++i) {
		openAndCopyRootNode(in_paths[i], svgRoot);
	}

	/* save file */
	res = xmlSaveFileEnc("./merged.svg", doc, MY_ENCODING);
	if (res == -1) {
		return SWC_ERR_IO_WRITE;
	}
	
	return SWC_OK;
}

/* reads all the SVG elements of a file into a buffer */
snd_err_t importSVGElementsFromFile(const char *path, char *buf, size_t buf_len) { return SWC_OK; }

/* */
int isCutEquivalent(cut_2d_t *a, cut_2d_t *b)
{
	if (strcmp(a->path, b->path) == 0 && a->x == b->x && a->y == b->y)
		return 0;
	else
		return 1;
}

snd_err_t svg_group_end(char *in) { return SWC_OK; }
