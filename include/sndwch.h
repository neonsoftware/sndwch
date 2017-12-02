/*
**
**
*/

#include <stddef.h>
#include <stdint.h>
#include <strings.h>

/* @page API
**
** Types:
**
**   snd_err_t - Error code. Int, 0 for success, positive for errors. 
**
** Data structures:
**
**   swc_cut2d_t - an SVG descibes a 2D shape. Such SVG will placed at a (x,y) offset
**   swc_cut3d_t - an swc_cut2d_t is present in the 3D object at a z position zmin, and
**                 is extruded to position zmax 
**   swc_layer_t - ...
**
** Algorithms:
**
**   swc_import_SVG_elements_from_file - reads all the SVG elements of a file into a buffer
**   swc_merge - merges multiple SVG files into one SVG file
**
*/

/* Limits */
#define swc_max_path 256
#define swc_max_cuts 10

/* Custom error message type */
typedef int snd_err_t;

/* Custom error codes */
static const snd_err_t SWC_OK = 0;
static const snd_err_t SWC_ERR_IO_WRITE = -1;
static const snd_err_t SWC_ERR_IN_FILE = -2;
static const snd_err_t SWC_ERR_ALLOC = -3;


typedef struct {
        char path[swc_max_path]; /**< path of the SVG file */
        float x;		 /**< x offset */
        float y;		 /**< y offset */
        float zstart;	 /**< y offset */
        float zend;	 /**< y offset */
} cut_file_t;

/* @brief An SVG descibes a 2D shape. Such SVG will placed at a (x,y) offset
*/
typedef struct {
	char path[swc_max_path]; /**< path of the SVG file */
	float x;		 /**< x offset */
	float y;		 /**< y offset */
} swc_cut2d_t;

/* @brief An swc_cut2d_t is present in the 3D object at a z position zmin, and
** is extruded to position zmax
*/
typedef struct {
	swc_cut2d_t cut; /**< cut */
	float zmin;      /**< z position, start */
	float zmax;      /**< z position, end */
} swc_cut3d_t;

/*
**
*/
typedef struct {
        cut_file_t *cuts[swc_max_cuts]; /**< set of cuts */
        size_t cuts_len; /**< number of cuts in the set */
        float zstart; /**< z position, start */
        float zend; /**< z position, end */
} swc_layer_t;

#ifdef __cplusplus
extern "C" {
#endif

/* @brief reads all the SVG elements of a file into a buffer
**
** @return SWC_OK in case of success, other snd_err_t otherwise
*/
snd_err_t swc_import_SVG_elements_from_file(const char *path, char *buf, size_t buf_len);

/* @brief merges multiple SVG files into one SVG file
**
** @param in_paths an array of input SVG file names
** @param in_paths_size size of in_paths array
** @param out_path the resulting SVG file
** @return SWC_OK in case of success, other snd_err_t otherwise
*/
snd_err_t swc_merge(const char **in_paths, size_t in_paths_size, const char *out_path);

snd_err_t swc_read_conf_file(const char *filePath, cut_file_t ***cuts, size_t *cuts_found_ptr);

snd_err_t swc_translate_and_merge(cut_file_t **cuts, size_t cuts_len, const char *out_path);

/* @brief Given a set of slice cuts of same z height and different z start and end, groups them in layers,
** which are sets of cuts of same z position
*/
snd_err_t swc_layer(cut_file_t **cuts_in, size_t cuts_in_len, swc_layer_t*** layers_out, size_t *layers_len);

snd_err_t swc_minimize_layers(swc_layer_t** layers_in, size_t layers_in_len, swc_layer_t*** layers_out, size_t *layers_out_len);

snd_err_t swc_print_layers(swc_layer_t** layers_in, size_t layers_in_len, swc_layer_t*** layers_out, size_t *layers_out_len);

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
  
#ifdef __cplusplus
} /* extern "C" */
#endif
