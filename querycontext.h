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

#ifndef SLASH_QUERYCONTEXT_H
#define SLASH_QUERYCONTEXT_H

#include <vector>
#include <stdlib.h>
#include "float.h"

namespace slash {

// Max-similarity search context
template <class FeatureVector>
class QueryContext {
	float curmin;
	size_t curminIndex;
	std::vector<FeatureVector> neighbors;
	std::vector<float> similarities;
	std::vector<int> ncopies;
	int limit;
	int found;
	int uniques;
	
	inline void updateMin() {
		float curmin = FLT_MAX;
		size_t curminIndex = -1;
		for (int i=0; i<this->uniques; i++) {
			float s = this->similarities[i];
			if (s < curmin) {
				curmin = s;
				curminIndex = i;
			}
		}
		this->curmin = curmin;
		this->curminIndex = curminIndex;
	}
	
public:
	inline void Insert(const FeatureVector &q, float s, int n) {
		if (n <= 0) {
			return;
		}

		if (this->found >= this->limit && s <= this->curmin) {
			return;
		}
		
		if (this->found < this->limit) {
			this->neighbors.push_back(q);
			this->similarities.push_back(s);
			this->ncopies.push_back(n);
			this->found += n;
			this->uniques++;
			
			if (this->uniques == this->limit) {
				this->updateMin();
			}
			return;
		}
		
		this->found -= this->ncopies[this->curminIndex];
		
		this->neighbors[this->curminIndex] = q;
		this->similarities[this->curminIndex] = s;
		this->ncopies[this->curminIndex] = n;
		this->found += n;
		this->updateMin();
	}
	
	inline std::vector<FeatureVector> Neighbors() {
		return this->neighbors;
	}
	
	inline int Limit() {
		return this->limit;
	}

	// Used internally by LSH::Query. Don't use.
	// Removes the most similar item in a quick & dirty way.
	// Assumes that there will be no further operations other than Neighbors.
 	inline void shrink() {
		float curmax = -FLT_MAX;
		size_t curmaxIndex = -1;
		for (int i=0; i<this->uniques; i++) {
			float s = this->similarities[i];
			if (s > curmax) {
				curmax = s;
				curmaxIndex = i;
			}
		}

		if(this->ncopies[curmaxIndex]-1 > 0) {
			return;
		}

		this->neighbors[curmaxIndex] = this->neighbors.back();
		this->neighbors.pop_back();
	}
	
	explicit QueryContext(int limit) {
		this->limit = limit;
		this->neighbors.reserve(limit);
		this->similarities.reserve(limit);
		this->ncopies.reserve(limit);
		this->curmin = FLT_MAX;
		this->found = 0;
		this->uniques = 0;
	}
};

};

#endif  // SLASH_QUERYCONTEXT_H
