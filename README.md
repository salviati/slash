# Introduction
slash is a high-performance locality-sensitive hashing (LSH) [1] library,
written in C++. It currently implements Spherical LSH (SLSH) [2] which is
suitable for high dimensional feature-spaces.

# Dependencies
gperftools, sparsehash.

# Usage
Simply copy the files `lsh.h`, `slsh.h`, `querycontext.h`, `math.h` and
`math.cc` into your source tree.
To start using the library, you need to define a class satisfying an
interface. (see BitVector64 class defined in bitvector64.h for a working
example) and a hash function (see hash.*). The file `lsh_test.cc`
contains a benchmark suite.

# License
slash is released under GNU General Public License version 3.

# References
[1]  A. Gionis, P. Indyk and R. Motwani,
``Similarity Search in High Dimensions via Hashing'',
Proc. 25th International Conference on Very Large Data Bases, VLDB1999, pp.518-529, 1999.

[2] `Terasawa, K., Tanaka, Y., 2007.
Spherical LSH for Approximate Nearest-Neighbor Search on Unit Hypersphere.
Springer. pp. 27–38''.