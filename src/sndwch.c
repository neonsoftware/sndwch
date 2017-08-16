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

static void openAndCopyRootNode(const char *filename, xmlNodePtr node_to_copy)
{
	xmlParserCtxtPtr ctxt; /* the parser context */
	xmlDocPtr doc;	 /* the resulting document tree */

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

		xmlNodePtr rootNode = xmlDocGetRootElement(doc);
		if (rootNode == NULL) {
			printf("testXmlwriterTree: Error creating the xml node\n");
			return;
		}
		
		xmlNodePtr node_copy = xmlCopyNode(rootNode, 1);
		if (node_copy == NULL) {
			printf("testXmlwriterTree: Error creating the xml node\n");
			return;
		}

		xmlAddChild(node_to_copy, node_copy);

		/* free up the resulting document */
		xmlFreeDoc(doc);
	}
	/* free up the parser context */
	xmlFreeParserCtxt(ctxt);
}

snd_err_t swc_merge(const char **in_paths, size_t in_paths_size, const char *out_path)
{

	LIBXML_TEST_VERSION

	xmlNodePtr rootCopy;

	xmlTextWriterPtr writer;
	int rc;
	xmlDocPtr doc;

	//	xmlNodePtr root;




    /* Create a new XML DOM tree, to which the XML document will be
     * written */
    doc = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
    if (doc == NULL) {
        printf
            ("testXmlwriterTree: Error creating the xml document tree\n");
        return SWC_OK;
    }

    /* Create a new XML node, to which the XML document will be
     * appended */
xmlNodePtr    node = xmlNewDocNode(doc, NULL, BAD_CAST "EXAMPLE", NULL);
    if (node == NULL) {
        printf("testXmlwriterTree: Error creating the xml node\n");
        return SWC_OK;
    }

    /* Make ELEMENT the root node of the tree */
    xmlDocSetRootElement(doc, node);


    /* Create a new XmlWriter for DOM tree, with no compression. */
    writer = xmlNewTextWriterTree(doc, node, 0);
    if (writer == NULL) {
        printf("testXmlwriterTree: Error creating the xml writer\n");
        return SWC_OK;
    }

    /* Start the document with the xml default for the version,
     * encoding ISO 8859-1 and the default for the standalone
     * declaration. */
    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    if (rc < 0) {
        printf("testXmlwriterTree: Error at xmlTextWriterStartDocument\n");
        return SWC_OK;
    }


      xmlNodePtr root = xmlDocGetRootElement(doc);
                if (root == NULL) {
                        printf("testXmlwriterTree: Error creating the xml node\n");
                        return SWC_OK;
                }

	
	for (size_t i = 0; i < in_paths_size; ++i) {
		// exampleFunc(in_paths[i]);
		openAndCopyRootNode(in_paths[i], root);
		//xmlAddChild(root, rootCopy);
		// writeOut(in_paths[i]);
	}

rc = xmlTextWriterEndDocument(writer);
                if (rc < 0) {
                        printf("testXmlwriterDoc: Error at xmlTextWriterEndDocument\n");
                        return SWC_OK;;
                }
	
	xmlFreeTextWriter(writer);
	xmlSaveFileEnc("./merged.svg", doc, MY_ENCODING);

	return SWC_OK;
}

static void exampleFunc(const char *filename)
{
	xmlParserCtxtPtr ctxt; /* the parser context */
	xmlDocPtr doc;	 /* the resulting document tree */

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



static void writeOut(const char *filename)
{
	xmlParserCtxtPtr ctxt; /* the parser context */
	xmlDocPtr doc;	 /* the resulting document tree */

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

		xmlTextWriterPtr writer;
		xmlNodePtr node;

		xmlDocPtr doc2;
		int rc;

		/* Create a new XML DOM tree, to which the XML document will be
		 * written */
		doc2 = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
		if (doc == NULL) {
			printf("testXmlwriterTree: Error creating the xml document tree\n");
			return;
		}

		/* Create a new XML node, to which the XML document will be
		 * appended */
		node = xmlNewDocNode(doc2, NULL, BAD_CAST "EXAMPLE", NULL);
		if (node == NULL) {
			printf("testXmlwriterTree: Error creating the xml node\n");
			return;
		}

		/* Make ELEMENT the root node of the tree */
		xmlDocSetRootElement(doc2, node);

		xmlNodePtr Node1Root = xmlDocGetRootElement(doc);
		if (Node1Root == NULL) {
			printf("testXmlwriterTree: Error creating the xml node\n");
			return;
		}

		xmlNodePtr Node2Root = xmlDocGetRootElement(doc2);
		if (Node2Root == NULL) {
			printf("testXmlwriterTree: Error creating the xml node\n");
			return;
		}

		xmlNodePtr new_node = xmlDocCopyNode(Node1Root, doc, 1);
		if (new_node == NULL) {
			printf("testXmlwriterTree: Error creating the xml node\n");
			return;
		}

		xmlAddChild(Node2Root, new_node);

		/* Create a new XmlWriter for DOM tree, with no compression. */
		writer = xmlNewTextWriterTree(doc2, node, 0);
		if (writer == NULL) {
			printf("testXmlwriterTree: Error creating the xml writer\n");
			return;
		}

		/* Create a new XmlWriter for DOM, with no compression. */
		// writer = xmlNewTextWriterDoc(&doc2, 0);
		if (writer == NULL) {
			printf("testXmlwriterDoc: Error creating the xml writer\n");
			return;
		}

		/* Start the document with the xml default for the version,
	     * encoding ISO 8859-1 and the default for the standalone
	     * declaration. */
		rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
		if (rc < 0) {
			printf("testXmlwriterDoc: Error at xmlTextWriterStartDocument\n");
			return;
		}

		/* Start an element named "EXAMPLE". Since thist is the first
	     * element, this will be the root element of the document. */
		rc = xmlTextWriterStartElement(writer, BAD_CAST "EXAMPLE");
		if (rc < 0) {
			printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
			return;
		}

		/* Here we could close the elements ORDER and EXAMPLE using the
	     * function xmlTextWriterEndElement, but since we do not want to
	     * write any other elements, we simply call xmlTextWriterEndDocument,
	     * which will do all the work. */
		rc = xmlTextWriterEndDocument(writer);
		if (rc < 0) {
			printf("testXmlwriterDoc: Error at xmlTextWriterEndDocument\n");
			return;
		}

		xmlFreeTextWriter(writer);

		xmlSaveFileEnc("./merged.svg", doc2, MY_ENCODING);

		// xmlFreeDoc(doc2);

		/* free up the resulting document */
		xmlFreeDoc(doc);
	}
	/* free up the parser context */
	xmlFreeParserCtxt(ctxt);
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
