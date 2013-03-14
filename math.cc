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

#include "math.h"

namespace slash {

// Calculates a d-dimensional random dvectortortor using d random number generators.
std::vector<dvector> randomRotation(int d, rng *r) {
	std::vector<dvector> R(d);
	
	for (size_t i=0; i<(size_t)d; i++) {
		R[i].random(d, r);
		dvector &u = R[i];
		for (size_t j=0; j<i; j++) {
			dvector &v = R[j];
			float vnorm = v.norm();
			if (vnorm == 0) {
				return randomRotation(d, r);
			}
			dvector vs(v);
			vs.scale(v.dot(u)/vnorm);
			u.sub(vs);
		}
		u.scale(1.0f/u.norm());
	}
	
	return R;
} 

};