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
	swc_cut2d_t cut[swc_max_cuts] /**< set of cuts */;
	float zmin; /**< z position, start */
	float zmax; /**< z position, end */
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

snd_err_t swc_read_conf_file(const char *filePath, cut_file_t ***cuts, int *cuts_found_ptr);

snd_err_t swc_translate_and_merge(cut_file_t **cuts, int cuts_len, const char *out_path);
  
#ifdef __cplusplus
} /* extern "C" */
#endif
