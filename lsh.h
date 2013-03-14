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

#ifndef SLASH_LSH_H
#define SLASH_LSH_H

#include <assert.h>
#include <stdint.h>
#include <vector>
#include <google/sparse_hash_map>
#include "types.h"
#include "querycontext.h"

namespace slash {

template <class FeatureVector>
class bin : public
google::sparse_hash_map<HashType, std::vector<const FeatureVector*> > {
};

template <class FeatureVector>
class hashCache : public
google::sparse_hash_map<const FeatureVector*, HashType*, FeatureVector, FeatureVector> {
};


// Class lsh implements Locality-Sensitive Hashing algorithm.
// A. Gionis, P. Indyk and R. Motwani, ``Similarity Search in High Dimensions via Hashing'',
// Proc. 25th International Conference on Very Large Data Bases, VLDB1999, pp.518-529, 1999.
template <class FeatureVector, class Hasher>
class LSH {
 public:
	// d, k and L refer to the variables found in the LSH literature:
	//
	// d is the dimension of the feature space.
	// k is the number of elementary hash functions (h) to be concataneted to obtain a reliable enough hash function (g). LSH queries becomes more selective with increasing k, due to the reduced the probability of collision.
	// L is the number of "copies" of the bins (with a different random matrices). Increasing L will increase the number of points the should be scanned linearly during query.
	// cacheHashes enables caching of hashes, which speeds up queries at the expense of extra memory. It also reduces the strain on memory allocator.
	LSH(int d, int k, int L, Hasher *hasher) : d(d), k(k), l(L), hasher(hasher) {
		this->bins = new bin<FeatureVector>[this->l];
	}
	
	~LSH() {
		for(auto &item: this->cache) {
			delete [] item.second;
		}
		delete [] this->bins;
	}

	// Hashes given points from the feature space, making them avaiable
	// for queries.
	// A FeatureVector must not be inserted more than once.
	void Insert(const std::vector<FeatureVector> &points) {
		size_t nPoints = points.size();
		for (size_t j = 0; j < nPoints; j++) {
			const FeatureVector& p = points[j];

			
			assert(this->cache[&p] == nullptr);
			/* if (this->cache[&p] != nullptr) {
				continue;
			} */

			auto g = new HashType[this->l];
			this->cache[&p] = g;
			this->hasher->Hash(p, g);

			for (size_t i = 0; i < (size_t)this->l; i++) {
				this->bins[i][g[i]].push_back(&p);
			}
		}

		for (size_t i = 0; i < (size_t)this->l; i++) {
			for (auto &item: bins[i]) {
				item.second.shrink_to_fit();
			}
		}
	}

	// Returns nearest neighbors of p; at most limit entries.
	// Runs in sublinear time O(n^ρ). The exponent ρ depends on the hashing function,
	// and the parameters d, k, L.
	// p must be Insert'ed before the Query.
	std::vector<FeatureVector> Query(const FeatureVector &p, int limit, size_t *linearSearchSize = nullptr) {
		QueryContext<FeatureVector> c(limit+1);
		
		auto g = this->cache[&p];
		if (g == nullptr) {
			return c.Neighbors();
		}

		for (size_t i = 0; i < (size_t)this->l; i++) {
			auto &v = bins[i][g[i]];
			size_t vSize = v.size();

			if (linearSearchSize != nullptr) {
				*linearSearchSize += vSize;
			}

			for (size_t j = 0; j < vSize; j++) {
				auto &q = *v[j];
				c.Insert(q, p.Similarity(q), q.NCopies());
			}
		}

		c.shrink();
		return c.Neighbors();
	}

 private:
	int d;  // the dimension of the feature space.
	int k;  // number of elementary hash functions (h) to be concataneted to obtain a reliable enough hash function (g). LSH queries becomes more selective with increasing k, due to the reduced the probability of collision.
	int l;  // number of "copies" of the bins (with a different random matrices). Increasing L will increase the number of points the should be scanned linearly during query.
	Hasher *hasher;
	bin<FeatureVector> *bins;  // bins[bin][hash] gives the FeatureVector that is hashed to hash in the bin bins[bin].
	hashCache<FeatureVector> cache;
};

};

#endif  // SLASH_LSH_H
