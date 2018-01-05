#ifndef SNDWCH_H
#define SNDWCH_H

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

#include "stdio.h"

/* Custom error message type */
typedef int snd_err_t;

/* Custom error codes */
static const snd_err_t SWC_OK = 0;
static const snd_err_t SWC_ERR_IO_WRITE = -1;
static const snd_err_t SWC_ERR_IN_FILE = -2;
static const snd_err_t SWC_ERR_ALLOC = -3;


typedef struct {
        char path[FILENAME_MAX]; /**< path of the SVG file */
        float x;		 /**< x offset */
        float y;		 /**< y offset */
        float zstart;	 /**< y offset */
        float zend;	 /**< y offset */
} cut_file_t;

/* @brief An SVG descibes a 2D shape. Such SVG will placed at a (x,y) offset
*/
typedef struct {
        char path[FILENAME_MAX]; /**< path of the SVG file */
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

/* @brief swc_sandwich Given a .yaml config file, processes the cuts and writes resulting .svg file in dir outDirPath
** @param configFilePat path to the .yaml input config file
** @param outDirPath path to teh output directory. If not existing it will be created
** @return SWC_OK in case of success. Other snd_err_t values in case of errors.
*/
snd_err_t swc_sandwich(const char *configFilePat, const char *outDirPath);


/* @brief merges multiple SVG files into one SVG file
**
** @param in_paths an array of input SVG file names
** @param in_paths_size size of in_paths array
** @param out_path the resulting SVG file
** @return SWC_OK in case of success, other snd_err_t otherwise
*/
snd_err_t swc_merge(const char **in_paths, size_t in_paths_size, const char *out_path);

  
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifndef SNDWCH_H */
