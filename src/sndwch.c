#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <stdio.h>
#include "sndwch.h"

#define MY_ENCODING "ISO-8859-1"
#define MAX_TRANSLATION_LEN 30

/*************** static local functions, declarations *********************************/

/* @brief Opens an SVG file, and copies the XML content into an xmlNodePtr
**
** @param filename A filename or an URL
** @param node_to_copy the destination xmlNodePtr
**
** @return SWC_OK in case of success, SWC_ERR_IN_FILE for file issues, 
** SWC_ERR_ALLOC for no space left. 
*/
static snd_err_t openAndCopyRootNode(const char *filename, xmlNodePtr node_to_copy);

/* @brief Deep equivalence of two swc_cut2d_t
*/
static int isCutEquivalent(swc_cut2d_t *a, swc_cut2d_t *b);

/*************** static local functions, implementations ******************************/

static snd_err_t parseFileToGroupNode(const char *filePath, xmlNodePtr *dst)
{
        xmlParserCtxtPtr ctxt; /* the parser context */
        xmlDocPtr doc;	 /* the resulting document tree */

        /* create a parser context */
        ctxt = xmlNewParserCtxt();
        if (ctxt == NULL) {
          return SWC_ERR_ALLOC;
        }
        /* parse the file, activating the DTD validation option */
        doc = xmlCtxtReadFile(ctxt, filePath, NULL, 0);
        /* check if parsing suceeded */
        if (doc == NULL) {
                fprintf(stderr, "Failed to parse %s\n", filePath);
        } else {
                /* check if validation suceeded */
                if (ctxt->valid == 0)
                        fprintf(stderr, "Failed to validate %s\n", filePath);

                xmlNodePtr rootNode = xmlDocGetRootElement(doc);
                if (rootNode == NULL) {
                        printf("testXmlwriterTree: Error creating the xml node\n");
                        return SWC_ERR_IN_FILE;
                }

                *dst = xmlNewNode(NULL, BAD_CAST "g");
                if (*dst == NULL) {
                        return SWC_ERR_IN_FILE;
                }

                xmlNodePtr content = xmlCopyNode(rootNode, 1);
                if (content == NULL) {
                        printf("testXmlwriterTree: Error creating the xml node\n");
                        return SWC_ERR_IN_FILE ;
                }
                xmlAddChildList(*dst, content->children);

                /* free up the resulting document */
                xmlFreeDoc(doc);
        }
        /* free up the parser context */
        xmlFreeParserCtxt(ctxt);
        return SWC_OK;
}

/**
 * @brief parseFileToNode
 * @param filename source file path
 * @param dst destination XML Node
 * @return
 */
static snd_err_t parseFileToNode(const char *filePath, xmlNodePtr *dst)
{
        xmlParserCtxtPtr ctxt; /* the parser context */
        xmlDocPtr doc;	 /* the resulting document tree */

        /* create a parser context */
        ctxt = xmlNewParserCtxt();
        if (ctxt == NULL) {
          return SWC_ERR_ALLOC;
        }
        /* parse the file, activating the DTD validation option */
        doc = xmlCtxtReadFile(ctxt, filePath, NULL, 0);
        /* check if parsing suceeded */
        if (doc == NULL) {
                fprintf(stderr, "Failed to parse %s\n", filePath);
        } else {
                /* check if validation suceeded */
                if (ctxt->valid == 0)
                        fprintf(stderr, "Failed to validate %s\n", filePath);

                xmlNodePtr rootNode = xmlDocGetRootElement(doc);
                if (rootNode == NULL) {
                        printf("testXmlwriterTree: Error creating the xml node\n");
                        return SWC_ERR_IN_FILE;
                }

                *dst = xmlCopyNode(rootNode, 1);

                /* free up the resulting document */
                xmlFreeDoc(doc);
        }
        /* free up the parser context */
        xmlFreeParserCtxt(ctxt);
        return SWC_OK;
}

/**
 * @brief openAndCopyRootNode
 * @param filename source XML file
 * @param node_to_copy root where to append the read
 * @return
 */
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
	return SWC_OK;
}

int isCutEquivalent(swc_cut2d_t *a, swc_cut2d_t *b)
{
        if (strcmp(a->path, b->path) == 0 && a->x == b->x && a->y == b->y)
                return 0;
        else
                return 1;
}

/******************** API functions, implementations ********************************************/

snd_err_t swc_import_SVG_elements_from_file(const char *path, char *buf, size_t buf_len) { return SWC_OK; }

snd_err_t swc_translate_grp(xmlNodePtr grp, int x, int y)
{
        if (grp == NULL) {
                return SWC_ERR_ALLOC; /* TODO provide a better error*/
        }

        char * translate_string = (char *) calloc(MAX_TRANSLATION_LEN, sizeof(char));
        sprintf(translate_string, "translate(%d, %d)", x, y);
        xmlNewProp(grp, BAD_CAST "transform", BAD_CAST translate_string);

        return SWC_OK;
}

snd_err_t swc_translate_and_merge(cut_file_t **cuts, int cuts_len, const char *out_path){

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

    for (size_t i = 0; i < cuts_len; i++) {
            cut_file_t *cut_ptr = cuts[i];
            xmlNodePtr n;
            xmlNodePtr n_translated;
            parseFileToGroupNode(cuts[i]->path, &n);
            swc_translate_grp(n, 3,4);
            xmlAddChild(svgRoot, n);
    }

    /* save file */
    res = xmlSaveFileEnc(out_path, doc, MY_ENCODING);
    if (res == -1) {
            return SWC_ERR_IO_WRITE;
    }

    return SWC_OK;
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
                xmlNodePtr n;
                parseFileToGroupNode(in_paths[i], &n);
                xmlAddChild(svgRoot, n);
	}

	/* save file */
        res = xmlSaveFileEnc(out_path, doc, MY_ENCODING);
	if (res == -1) {
		return SWC_ERR_IO_WRITE;
	}
	
	return SWC_OK;
}
