#include <stddef.h>
#include <strings.h>

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

/* Config */
static const size_t PATH_MAX = 256;
static const size_t CUTS_MAX = 10;

/* Custom error message type */
typedef int snd_err_t;

/* Custom error codes */
static const snd_err_t SWC_OK = 0;
static const snd_err_t SWC_NO_SPACE = -1;
static const snd_err_t SWC_ERROR = -2;

typedef struct {
	char path[PATH_MAX];
	float x;
	float y;
} cut_2d_t;

typedef struct {
	cut_2d_t cut;
	float zmin;
	float zmax;
} cut_3d_t;

typedef struct {
	cut_2d_t cut[CUTS_MAX];
	float zmin;
	float zmax;
} layer_t;

/* reads all the SVG elements of a file into a buffer */
snd_err_t importSVGElementsFromFile(const char *path, char *buf, size_t buf_len);

/* */
int isCutEquivalent(cut_2d_t *a, cut_2d_t *b);

snd_err_t svg_group_end(char *in);

snd_err_t swc_merge(const char **in_paths, size_t in_paths_size, const char *out_path);
