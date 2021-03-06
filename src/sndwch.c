#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <stdio.h>
#include <string.h>
#include "sndwch.h"
#include "sndwch_dev.h"

#define MY_ENCODING "ISO-8859-1"
#define MAX_TRANSLATION_LEN 30
static const size_t max_files = 200; /* max cut_file_t length */

/*************** static local functions, implementations ******************************/

snd_err_t parseFileToGroupNode(const char *filePath, xmlNodePtr *dst)
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

snd_err_t xmlTranslateGrp(xmlNodePtr grp, float x, float y)
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

int swc_layer_equivalent_neighbors(swc_layer_t * a, swc_layer_t * b){

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

void swc_layer_init(swc_layer_t *l){
    if(l == NULL)
        return;


    l->zstart = 0.0;
    l->zend = 0.0;
}


snd_err_t swc_layer(cut_file_t **cuts_in, size_t cuts_in_len, swc_layer_t*** layers_ptr, size_t *layers_len_ptr){

    *layers_len_ptr = 0;
    *layers_ptr = (swc_layer_t **)calloc(max_files, sizeof(swc_layer_t *));
    if(*layers_ptr == NULL)
        return SWC_ERR_ALLOC;

    /* adding every cut to its corresponding layer (by zstart) */
    for (size_t i = 0; i < cuts_in_len; i++) {

            cut_file_t *cut_ptr = cuts_in[i];
            swc_layer_t* corresponding_layer = NULL;

            /* Append to layer */

            /* does layer exist already ? */
            for(size_t j = 0; j < (*layers_len_ptr); j++)
                if( (*layers_ptr)[j]->zstart == cut_ptr->zstart)
                    corresponding_layer = (*layers_ptr)[j];

            /* if not, creating a new one and appending it to layers */
            if(corresponding_layer == NULL){
                /* creating new layer */
                corresponding_layer = (swc_layer_t*) calloc(1, sizeof(swc_layer_t));
                if(corresponding_layer == NULL)
                    return SWC_ERR_ALLOC;
                /* initializing new layer */
                memset(corresponding_layer->cuts, 0, sizeof(corresponding_layer->cuts));
                corresponding_layer->cuts_len = 0;
                corresponding_layer->zstart = cut_ptr->zstart;
                corresponding_layer->zend = cut_ptr->zend;
                /* adding new layer to output */
                (*layers_ptr)[*layers_len_ptr] = corresponding_layer;
                *layers_len_ptr = (*layers_len_ptr) + 1;
            }

            /* adding cut to corresponding layer */
            corresponding_layer->cuts[corresponding_layer->cuts_len] = cut_ptr;
            corresponding_layer->cuts_len = corresponding_layer->cuts_len + 1;
    }

    return SWC_OK;
}

snd_err_t swc_minimize_layers(swc_layer_t** layers_in, size_t layers_in_len, swc_layer_t*** layers_out_ptr, size_t *layers_out_len_ptr){

    *layers_out_len_ptr = 0;
    *layers_out_ptr = (swc_layer_t **)calloc(max_files, sizeof(swc_layer_t *));
    if(*layers_out_ptr == NULL)
        return SWC_ERR_ALLOC;

    //swc_layersort(layers_in, layers_in_len);

    size_t current_layer_idx = 0;

    while(current_layer_idx < layers_in_len){

        size_t next_layer_idx = current_layer_idx + 1;

        if( next_layer_idx < layers_in_len &&
                swc_layer_equivalent_neighbors(layers_in[ current_layer_idx ], layers_in[ next_layer_idx ]) == 0){
                /* if current and next are equivalent they merge, by making the next start from current's zstart */
                layers_in[ next_layer_idx ] -> zstart = layers_in[ current_layer_idx ] -> zstart;
        }else{
                /* if this layer is unique, goes to output array */
                (*layers_out_ptr)[*layers_out_len_ptr] = layers_in[ current_layer_idx ];
                *layers_out_len_ptr = *layers_out_len_ptr + 1;
        }

        /* iterate */
        current_layer_idx = next_layer_idx;
    }

    return SWC_OK;
}

snd_err_t swc_print_layers(swc_layer_t** layers_in, size_t layers_in_len, const char *out_dir){

    /* adding every cut to its corresponding layer (by zstart) */
    for (size_t i = 0; i < layers_in_len; i++) {
        snd_err_t result = SWC_OK;
        char file_path[swc_max_path];
        sprintf(file_path, "%s/file#%.1f#%.1f.svg", out_dir, layers_in[i]->zstart, layers_in[i]->zend);
        result = swc_translate_and_merge(layers_in[i]->cuts, layers_in[i]->cuts_len, file_path);
        if(result != SWC_OK)
            return result;
    }
    return SWC_OK;
}

snd_err_t swc_sandwich(const char *configFilePat, const char *outDirPath){

    /* storage variables */
    snd_err_t result = SWC_OK;
    cut_file_t **cuts = NULL;
    size_t cuts_len = 0;
    cut_file_t **cuts_sliced = NULL;
    size_t cuts_sliced_len = 0;
    swc_layer_t **layers = NULL;
    size_t layers_len = 0;
    swc_layer_t **layers_mini = NULL;
    size_t layers_mini_len = 0;

    /* translate to absolute paths */
    /* TODO */

    /* check that paths exist */
    /* TODO */
    
    result = swc_read_conf_file(configFilePat, &cuts, &cuts_len);
    if(result != SWC_OK)
        return result;
    result = swc_slice(cuts, cuts_len, &cuts_sliced, &cuts_sliced_len);
    if(result != SWC_OK)
        return result;
    result = swc_layer(cuts_sliced, cuts_sliced_len, &layers, &layers_len);
    if(result != SWC_OK)
        return result;
    result = swc_minimize_layers(layers, layers_len, &layers_mini, &layers_mini_len);
    if(result != SWC_OK)
        return result;
    result = swc_print_layers(layers_mini, layers_mini_len, outDirPath);
    if(result != SWC_OK)
        return result;

    return SWC_OK;
}

