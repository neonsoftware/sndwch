#ifndef SNDWCH_DEV_H
#define SNDWCH_DEV_H

#include "sndwch.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>


/*************** static local functions, declarations *********************************/




/* @brief Deep equivalence of two swc_cut2d_t
*/
//static int isCutEquivalent(swc_cut2d_t *a, swc_cut2d_t *b);

/* @brief user provide either an absolute path or relative to the config file. If
** path is relative, then retrieve the provided by the user, retrieve the absolute
** @param filePath path of the source XML file
** @param dst the destination XML group element
** @return SWC_OK in case of success, SWC_ERR_IN_FILE or SWC_ERR_ALLOC otherwise
*/
snd_err_t makeSVGPathAbsolute(char *svgFilePath, size_t svgFilePath_buf_len, const char *configFilePath);

/*
** @brief parseFileToGroupNode parses an XML file into a xmlNodePtr XML group element
** @param filePath path of the source XML file
** @param dst the destination XML group element
** @return SWC_OK in case of success, SWC_ERR_IN_FILE or SWC_ERR_ALLOC otherwise
*/
snd_err_t parseFileToGroupNode(const char *filePath, xmlNodePtr *dst);

/*
** @brief xmlTranslateGrp Adds a transformation parameter to an XML group element
** @param grp the XML group element to translate
** @param x the translateion x coordinate
** @param y the translateion y coordinate
** @return SWC_OK in case of success, or SWC_ERR_ALLOC otherwise
**/
snd_err_t xmlTranslateGrp(xmlNodePtr grp, float x, float y);

/* @brief reads all the SVG elements of a file into a buffer
**
** @return SWC_OK in case of success, other snd_err_t otherwise
*/
snd_err_t swc_import_SVG_elements_from_file(const char *path, char *buf, size_t buf_len);

snd_err_t swc_read_conf_file(const char *filePath, cut_file_t ***cuts, size_t *cuts_found_ptr);

snd_err_t swc_translate_and_merge(cut_file_t **cuts, size_t cuts_len, const char *out_path);

/* @brief Given a set of slice cuts of same z height and different z start and end, groups them in layers,
** which are sets of cuts of same z position
*/
snd_err_t swc_layer(cut_file_t **cuts_in, size_t cuts_in_len, swc_layer_t*** layers_out, size_t *layers_len);

snd_err_t swc_minimize_layers(swc_layer_t** layers_in, size_t layers_in_len, swc_layer_t*** layers_out, size_t *layers_out_len);

snd_err_t swc_print_layers(swc_layer_t** layers_in, size_t layers_in_len, const char *out_path);

/* @brief function used for ordering. First path is compared with strcmp, then x, then y. Returns -2 if a or b is NULL.
*/
int swc_cutcmp(cut_file_t * a, cut_file_t * b);

/* @brief given an array of cut_file_t of length cuts_len, sorts them in ascendent order according to
 * swc_cutcmp comparison
*/
void swc_cutsort(cut_file_t ** cuts, size_t cuts_len);

/* @brief return 0 is a and b are same, -1 if a < b, 1 if b < a. Returns -2 if a or b is NULL.
*/
int swc_layercmp(swc_layer_t * a, swc_layer_t * b);

/* @brief return -2 if a or b are NULL. returns 0 if layers are neighbors (contiguous on z), and have exact same cuts.
 * Returns -1 otherwise.
*/
int swc_layer_equivalent_neighbors(swc_layer_t * a, swc_layer_t * b);

/* @brief given an array of swc_layer_t of length layers_in_len, sorts them in ascendent order according to
 * swc_layercmp comparison
*/
void swc_layersort(swc_layer_t** layers, size_t layers_len);

/*
** @brief swc_slice Cuts every cut_file_t passed in an input array into many cut_file_t of 0.5 mm thckness
** @param cuts_in array of input cuts
** @param cuts_in_len length of cuts_in
** @param cuts_out output cuts destination
** @param cuts_out_len pointer number of output cuts created
** @return
*/
snd_err_t swc_slice(cut_file_t **cuts_in, size_t cuts_in_len, cut_file_t ***cuts_out, size_t *cuts_out_len);

#endif /* ifndef SNDWCH_H */
