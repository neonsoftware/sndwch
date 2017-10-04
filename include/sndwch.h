#include <stddef.h>
#include <stdint.h>
#include <strings.h>

/* Limits */
static const size_t swc_max_path = 256;
static const size_t swc_max_cuts = 10;

/* Custom error message type */
typedef int snd_err_t;

/* Custom error codes */
static const snd_err_t SWC_OK = 0;
static const snd_err_t SWC_ERR_IO_WRITE = -1;
static const snd_err_t SWC_ERR_IN_FILE = -2;
static const snd_err_t SWC_ERR_ALLOC = -3;

typedef struct {
	char path[swc_max_path];
	float x;
	float y;
} swc_cut2d_t;

typedef struct {
	swc_cut2d_t cut;
	float zmin;
	float zmax;
} swc_cut3d_t;

typedef struct {
	swc_cut2d_t cut[swc_max_cuts];
	float zmin;
	float zmax;
} swc_layer_t;

/* reads all the SVG elements of a file into a buffer */
snd_err_t swc_import_SVG_elements_from_file(const char *path, char *buf, size_t buf_len);

/* */
int isCutEquivalent(swc_cut2d_t *a, swc_cut2d_t *b);

snd_err_t svg_group_end(char *in);

snd_err_t swc_merge(const char **in_paths, size_t in_paths_size, const char *out_path);

/*
 *

what do i want:


i have some fig-h_min-h_max.svg files

place at (x,y) and h_min, h_max


-cut: /evefa/vadfa/
  x:      57
  y:      10
  z_min:  4
  z_max:  6


> cuts.yaml

the library should:
. read the input .yaml file
. parse into list of 3Dcut objects
. slice together the 3Dcut objects into a list of layers
. merge together equal layers (MergeEqualLayers)
. write the layers to file (WriteLayersToFile)


. ext: write visual makes a svg file that shows them one above the other


layers_t

SliceByMM : 3dcuts to lauers

MergeEqualLayers : given a list of layers, merges the same

writeLayersToFile

writeVisual : write visual makes a svg file that shows them one above the other

MakeSandwich :



 *
 * make_sandwich(char *out_dir,  , );
 *
 *
 */
