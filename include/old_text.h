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

. read the input .yaml file into list of swc_cut2d_t objects
    swc_error_t swc_read_cut2d_config( swc_cut2d_t * buf, size_t buf_size );
. parse into list of 3Dcut objects
    swc_error_t swc_load_cut3d( swc_cut2d_t * buf2d, size_t buf2d_size, swc_cut3d_t * buf3d, size_t buf3d_size );
. slice together the 3Dcut objects into a list of layers
    swc_merge_cut3d( swc_cut3d_t * inbuf, size_t inbuf_size, swc_cut3d_t * outbuf, size_t outbuf_size )
. merge together equal layers (MergeEqualLayers)
. write the layers to file (WriteLayersToFile)
. ext: write visual makes a svg file that shows them one above the other

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
