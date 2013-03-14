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

#ifndef BITVECTOR64_H
#define BITVECTOR64_H

#include "hash.h"
#include "util.h"

class BitVector64 {
	uint64_t v;

public:
	BitVector64() {
		this->v = 0;
	}

	explicit BitVector64(uint64_t v) {
		this->v = v;
	}
	
	inline char* String(char *buffer) const { // For debugging. Don't use.
		if (buffer == 0) {
			buffer = new char[64+1];
		}
		u64toa(this->v, buffer, 2);
		return buffer;
	}


	// Needed by lsh. Impacts performance greatly.
	inline float Dot(float *u) const {
		float sum = 0;
		uint64_t v = this->v;
		for(int i=0; v && i<64; i++, v>>=1) {
			if (v&1) {
				sum += u[i];
			}
		}
		return sum;
	}
	
	inline float Similarity(const BitVector64 &q) const {
		float dot = (float)__builtin_popcountll(this->v & q.v);
		float norm = (float)(__builtin_popcountll(this->v)*__builtin_popcountll(q.v)); 
		return dot/sqrtf(norm);
	}
	
	inline int NCopies() const {
		return 1;
	}


	// operators needed by sparsehash.
	inline bool operator==(const BitVector64 &q) const {
		return this->v == q.v;
	}
	inline size_t operator()(const  BitVector64 *p) const { 
		return hash(&p->v, sizeof(uint64_t), 1);
	}	
	inline bool operator()(const BitVector64 &p, const BitVector64 &q) const {
		return p == q;
	}
	inline bool operator()(const BitVector64 *p, const BitVector64 *q) const {
		return p == q;
	}
};

#endif  // BITVECTOR64_H
