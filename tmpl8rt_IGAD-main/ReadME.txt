Inside the scene.h file you alter between BVH, Octree and Grid by commenting / uncommenting the option you want. The same for the different scenes you can select at the top of the file.

The following defines are added:

#define USESCENESIMPLE
#define USESCENECOMPLEX
#define USESCENETEAPOT

#define USEBVH
#define USEOCTREE
#define USEGRID

The grid implementation is slow. be aware :)

