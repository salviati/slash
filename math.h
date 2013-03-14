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

#ifndef SLASH_MATH_H
#define SLASH_MATH_H

#include <math.h>
#include <stdlib.h>
#include <vector>
#include <random>
#include <assert.h>

namespace slash {

class rng {
  std::default_random_engine generator;
  std::normal_distribution<float> distribution;
public:
 	rng() {
		this->generator.seed(rand());
		this->distribution = std::normal_distribution<float>(0.0f, 1.0f);
	}

	float Float() {
		return this->distribution(this->generator);
	}
};

class dvector {
public:
	float *v;
	size_t l;
	float norm() {
		float n=0;
		for (size_t i=0; i<this->l; i++) {
			n += this->v[i]*this->v[i];
		}
		return sqrtf(n);
	}
	
	void scale(float s) {
		for (size_t i=0; i<this->l; i++) {
			v[i] *= s;
		}
	}
	
	float dot(const dvector &u) {
		float s=0;
		for (size_t i=0; i<this->l; i++) {
			s += this->v[i] * u.v[i];
		}
		return s;
	}
	
	void sub(const dvector &u) {
		for (size_t i=0; i<this->l; i++) {
			this->v[i] -= u.v[i];
		}
	}
	
	void random(size_t d, rng *r)  {
		if (this->v != 0) {
			delete [] this->v;
			this->v = 0;
		}

		this->v = new float[d];
		this->l = d;

		for (size_t i=0; i<d; i++) {
			this->v[i] = r[i].Float();
		}
	}

	dvector() {
		this->v = 0;
		this->l = 0;
	}

	explicit dvector(size_t d) {
		this->v = new float[d];
		this->l = d;
	}
	
	dvector(const dvector &u) {
		this->v = new float[u.l];
		for (size_t i = 0; i < u.l; i++) {
			this->v[i] = u.v[i];
		}
		this->l = u.l;
	}
	
	~dvector() {
		if (this->l) {
			delete [] this->v;
		}
	}

};

std::vector<dvector> randomRotation(int d, rng *r);

};

#endif  // SLASH_MATH_H
