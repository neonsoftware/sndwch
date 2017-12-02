#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <stdio.h>
#include "sndwch.h"

#define MY_ENCODING "ISO-8859-1"
#define MAX_TRANSLATION_LEN 30
static const size_t max_files = 200; /* max cut_file_t length */

/*************** static local functions, declarations *********************************/

/* @brief Deep equivalence of two swc_cut2d_t
*/
//static int isCutEquivalent(swc_cut2d_t *a, swc_cut2d_t *b);

/*
** @brief parseFileToGroupNode parses an XML file into a xmlNodePtr XML group element
** @param filePath path of the source XML file
** @param dst the destination XML group element
** @return SWC_OK in case of success, SWC_ERR_IN_FILE or SWC_ERR_ALLOC otherwise
*/
static snd_err_t parseFileToGroupNode(const char *filePath, xmlNodePtr *dst);

/*
** @brief xmlTranslateGrp Adds a transformation parameter to an XML group element
** @param grp the XML group element to translate
** @param x the translateion x coordinate
** @param y the translateion y coordinate
** @return SWC_OK in case of success, or SWC_ERR_ALLOC otherwise
**/
static snd_err_t xmlTranslateGrp(xmlNodePtr grp, float x, float y);

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

static snd_err_t xmlTranslateGrp(xmlNodePtr grp, float x, float y)
{
        if (grp == NULL) {
                return SWC_ERR_ALLOC; /* TODO provide a better error*/
        }

        char * translate_string = (char *) calloc(MAX_TRANSLATION_LEN, sizeof(char));
        sprintf(translate_string, "translate(%.1f, %.1f)", x, y);
        xmlNewProp(grp, BAD_CAST "transform", BAD_CAST translate_string);

        return SWC_OK;
}


/*
static int isCutEquivalent(swc_cut2d_t *a, swc_cut2d_t *b)
{
        if (strcmp(a->path, b->path) == 0 && a->x == b->x && a->y == b->y)
                return 0;
        else
                return 1;
}
*/

/******************** API functions, implementations ********************************************/

snd_err_t swc_translate_and_merge(cut_file_t **cuts, size_t cuts_len, const char *out_path){

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
            xmlNodePtr n;
            parseFileToGroupNode(cuts[i]->path, &n);
            xmlTranslateGrp(n, cuts[i]->x, cuts[i]->y);
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

snd_err_t swc_slice(cut_file_t **cuts_in, size_t cuts_in_len, cut_file_t ***cuts_out_ptr, size_t *cuts_out_len_ptr){

        *cuts_out_len_ptr = 0;
        *cuts_out_ptr = (cut_file_t **)calloc(max_files, sizeof(cut_file_t *));

        for (size_t i = 0; i < cuts_in_len; i++) {
                cut_file_t *cut_ptr = cuts_in[i];

                /* TODO I should convert to int to avoid float problems on some platforms */

                /* for every cut create N cuts of 0.5mm z height */
                for (float z = cut_ptr->zstart; z < cut_ptr->zend; z += 0.5){
                    /* create a new 0.5mm slice */
                    cut_file_t *new_cut_slice_ptr = (cut_file_t *)calloc(1, sizeof(cut_file_t));

                    if(new_cut_slice_ptr == NULL){
                        return SWC_ERR_ALLOC;
                    }

                    /* except for z, it is a copy */
                    memcpy(new_cut_slice_ptr, cut_ptr, sizeof(cut_file_t));
                    /* the new slice has a z height of 0.5mm */
                    new_cut_slice_ptr->zstart = z;
                    new_cut_slice_ptr->zend = z + 0.5;
                    /* storing the new cut to out array, and increase counter */
                    (*cuts_out_ptr)[*cuts_out_len_ptr] = new_cut_slice_ptr; /* storing new file */
                    *cuts_out_len_ptr = (*cuts_out_len_ptr) + 1;
                }
        }

        return SWC_OK;
}

/************* sort and comparisons **********/

int swc_cutcmp(cut_file_t * a, cut_file_t * b){

    if(a == NULL || b == NULL)
        return -2;

    /* comparison order is: path. if is equal, then comparing x, then y. */

    int pathcmp = strcmp(a->path, b->path);

    /* if path differs, the difference is a valid comparison */
    if(pathcmp != 0)
        return pathcmp;

    /* path is the same, maybe x differs */
    if (a->x < b->x)
        return -1;
    else if (a->x > b->x)
        return 1;

    /* no luck, x is the same, checking y */
    if (a->y < b->y)
        return -1;
    else if (a->y > b->y)
        return 1;

    /* ok, they are equal */
    return 0;
}

void swc_cutsort(cut_file_t ** cuts, size_t cuts_len){

    size_t i, j;
    cut_file_t *temp = NULL;

    if(cuts == NULL || cuts_len <= 1)
        return;


for (i = 0; i < cuts_len; i++)
{
    for (j = 0; j < cuts_len - 1 - i; j++)
    {
        if ( swc_cutcmp(cuts[j + 1], cuts[j]) == -1)
        {
            temp = cuts[j];
            cuts[j] = cuts[j + 1];
            cuts[j + 1] = temp;
        }
    }
}

}

int swc_layercmp(swc_layer_t * a, swc_layer_t * b){

    if(a == NULL || b == NULL)
        return -2;

    if (a->zstart == b->zstart)
        return 0;
    else if (a->zstart < b->zstart)
        return -1;
    else
        return 1;
}



void swc_layersort(swc_layer_t** layers, size_t layers_len){

    size_t i, j;
    swc_layer_t *temp = NULL;

    if(layers == NULL || layers_len <= 1)
        return;


    for (i = 0; i < layers_len; i++)
    {
        for (j = 0; j < layers_len - 1 - i; j++)
        {
            if ( swc_layercmp(layers[j + 1], layers[j]) == -1)
            {
                temp = layers[j];
                layers[j] = layers[j + 1];
                layers[j + 1] = temp;
            }
        }
    }
}

int swc_layer_equivalent_neighbor(swc_layer_t * a, swc_layer_t * b){

    if(a == NULL || b == NULL)
        return -2;

    if( a->zend != b->zstart && b->zend != a->zstart )
        return -1;

    if( a->cuts_len != b->cuts_len )
        return -1;

    /* TODO, for now requires order */
    for( size_t i = 0; i < b->cuts_len; i++ )
        if(swc_cutcmp(a->cuts[i], b->cuts[i]) != 0)
            return -1;

    return 0;
}

snd_err_t swc_layer(cut_file_t **cuts_in, size_t cuts_in_len, swc_layer_t*** layers_ptr, size_t *layers_len_ptr){

    *layers_len_ptr = 0;
    *layers_ptr = (swc_layer_t **)calloc(max_files, sizeof(swc_layer_t *));

    /* adding every cut to its corresponding layer (by zmin) */
    for (size_t i = 0; i < cuts_in_len; i++) {

            cut_file_t *cut_ptr = cuts_in[i];
            swc_layer_t* corresponding_layer = NULL;

            /* Append to layer */

            /* does layer exist already ? */
            for(size_t j = 0; i < (*layers_len_ptr); j++)
                if( (*layers_ptr)[j]->zmin == cut_ptr->zmin)
                    corresponding_layer = *layers_ptr)[j];

            /* if not, creating a new one and appending it to layers */
            if(corresponding_layer == NULL){
                corresponding_layer = (swc_layer_t*) calloc(1, sizeof(swc_layer_t*));
                (*cuts_ptr)[*cuts_found_ptr] = corresponding_layer;
                *cuts_found_ptr = (*cuts_found_ptr) + 1;
            }

            /* adding cut to corresponding layer */
            corresponding_layer->cuts[corresponding_layer->cuts_len] = cut_ptr;
            corresponding_layer->cuts_len = corresponding_layer->cuts_len + 1;
    }
}

//snd_err_t swc_minimize_layers(swc_layer_t** layers_in, size_t layers_in_len, swc_layer_t*** layers_out, size_t *layers_out_len){

//}

//snd_err_t swc_print_layers(swc_layer_t** layers_in, size_t layers_in_len, swc_layer_t*** layers_out, size_t *layers_out_len){

//}

