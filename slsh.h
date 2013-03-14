// slash - a locality sensitive hashing library.
// Copyright (c) 2013 Utkan Güngördü <utkan@freeconsole.org>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SLASH_SLSH_H
#define SLASH_SLSH_H

#include "math.h"
#include "hash.h"
#include "types.h"


namespace slash {

// Class SLSH implements Spherical Locality-Sensitive Hashing algorithm.
// ``Terasawa, K., Tanaka, Y., 2007. Spherical LSH for Approximate Nearest-Neighbor Search on Unit Hypersphere. Springer. pp. 27–38''.
// This implentation uses vertices of a d-dimensional orthoplex as lattice points.
//
// SLSH is equivalent to an ε-nearest neighbor search using cosine similarity, and does not suffer from the curse of dimensionality.
template <class FeatureVector>
class SLSH {
	std::vector<std::vector<dvector> > vAll; // vAll[i][j] is the vector $A_i \tilde v_j$ from the article.
	unsigned int hbits;     // Ceil(Log2(2*d)).
	int d;       // the dimension of the feature space.
	int k;       // number of elementary hash functions (h) to be concataneted to obtain a reliable enough hash function (g). LSH queries becomes more selective with increasing k, due to the reduced the probability of collision.
	int l;       // number of "copies" of the bins (with a different random matrices). Increasing L will increase the number of points the should be scanned linearly during query.
public:
	SLSH(int d, int k, int L) : d(d), k(k), l(L) {
		double nvertex = 2.0 * this->d;
		this->hbits = (unsigned int)ceil(log2(nvertex));
		int kmax = static_cast<int>(HashBits/this->hbits);
		if (this->k > kmax) {
			this->k = kmax;
			printf("k is too big, chopping down (%d->%d)\n", k, kmax);
		}
		
		rng *r = new rng[d];
		
		// For orthoplex, the basis Vectortors v_i are permutations of the Vectortor (1, 0, ..., 0),
		// and -(1, 0, ..., 0).
		// Thus R v_i simply picks up the ith row of the rotation matrix, up to a sign.
		// This means we don't need any matrix multiplication; R matrix is the list of
		// rotated vectors itself!
		this->vAll = std::vector<std::vector<dvector> >(this->k*this->l);  // random rotation matrices
		auto &rotationMatrices = this->vAll;
		
		for (size_t i=0; i<rotationMatrices.size(); i++) {
			rotationMatrices[i] = randomRotation(this->d, r);
		}
		delete [] r;
	}
	
	inline int argmaxi(const FeatureVector &p, const std::vector<dvector> &vs) {
		int maxi = 0;
		float max = 0;

		for (int i=0; i<this->d; i++) {
			float dot = p.Dot(vs[i].v);
			
			float abs = dot>=0?dot:-dot;
			if (abs < max) {
				continue;
			}
			
			max = abs;
			maxi = dot >= 0 ? i : i*2;
		}
		return maxi;
	}

	// Hashes a single point slsh.l times, using a different set of
	// random matrices created and stored by the constructor for each.
	// Stores the result in g to avoid unnecessary allocations.
	//
	// SLSH requires that all vectors lie on a d-dimensional hypershpere,
	// thus having the same norm. Only the Similarity method of FeatureVector
	// is required to take the normalization into account.
	//
	// The complexity of this function is O(nL)
	void Hash(const FeatureVector &p, HashType *g) {
		int ri=0;
		HashType h;

		for (int i=0; i<this->l; i++) {
			g[i] = 0;
			for (int j=0; j<this->k; j++) {
				auto &vs = this->vAll[ri]; // See the comment in init.
				h = (HashType)this->argmaxi(p,vs);
				g[i] |= h << (HashType)(this->hbits*j);
				ri++;
			}
		}
	}

	~SLSH() {
	}
};

};

#endif  // SLASH_SLSH_H
