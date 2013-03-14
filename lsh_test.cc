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

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "lsh.h"
#include "slsh.h"
#include "bitvector64.h"

#define SEED time(0)

#define BAD_LINEAR_SEARCH_FRACTION 1e-2

#define NPOINTS 1e5
#define NQUERIES 1e5

int d = 64, k = 6, L = 2, limit = 10;
slash::SLSH<BitVector64> *slsh;
slash::LSH<BitVector64, slash::SLSH<BitVector64> > *lsh;
char buf[256];
std::vector<BitVector64> points;

void init() {
	srandom(SEED);

	for (size_t i = 0; i < NPOINTS; i++) {
		BitVector64 r((uint64_t)random());
		points.push_back(r);
	}

	slsh = new slash::SLSH<BitVector64>(d, k, L);
	lsh = new slash::LSH<BitVector64, slash::SLSH<BitVector64> >(d, k, L, slsh);
}

void TestLinearSearch() {
	printf("==== %s\n", __func__);

	BitVector64 &p = points[0];
	timespec start, end;
	double del;
	slash::QueryContext<BitVector64> c(limit+1);

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (size_t i = 0; i < NPOINTS; i++) {
			BitVector64 &q = points[i];
			c.Insert(q, p.Similarity(q), q.NCopies());
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	del = 1e9*(double)(end.tv_sec-start.tv_sec)+(double)(end.tv_nsec-start.tv_nsec);
	printf("%g ns/op\n", del);
	
	c.shrink();
	auto neighbors = c.Neighbors();

	for(size_t i=0; i<neighbors.size(); i++) {
		printf("n%d: %g, %s\n", (int)i, p.Similarity(neighbors[i]), neighbors[i].String(buf));
	}
}

void TestInsert() {
	printf("==== %s\n", __func__);

	timespec start, end;
	double del;

	clock_gettime(CLOCK_MONOTONIC, &start);

	lsh->Insert(points);

	clock_gettime(CLOCK_MONOTONIC, &end);
	del = 1e9*(double)(end.tv_sec-start.tv_sec)+(double)(end.tv_nsec-start.tv_nsec);
	printf("%g ns/op\n", del);
}

void TestQuery() {
	printf("==== %s\n", __func__);
	
	BitVector64 &p = points[0];
	size_t linearSearchSize = 0;

	printf("p: %s\n", p.String(buf));

	auto neighbors = lsh->Query(p, limit, &linearSearchSize);
	printf("SLSH\nd=%d, k=%d, L=%d, #points=%llu, linearSearch=%g\n", d, k, L, (unsigned long long)NPOINTS, (double)linearSearchSize);
	for(size_t i=0; i<neighbors.size(); i++) {
		printf("n%d: %g, %s\n", (int)i, p.Similarity(neighbors[i]), neighbors[i].String(buf));
	}
}

void BenchmarkQuery() {
	printf("==== %s\n", __func__);
	
	timespec start, end;
	double del;
	double totalLinearSearchSize = 0;
	double totalFoundNeighbors = 0;
	size_t badLinearSearch = 0;
	size_t fewNeighbors = 0;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (size_t i=0; i<NQUERIES; i++) {
		size_t linearSearchSize = 0;
		
		auto neighbors = lsh->Query(points[i % (size_t)NPOINTS], limit, &linearSearchSize);
		
		totalLinearSearchSize += (double)linearSearchSize;
		if ((double)linearSearchSize > NPOINTS*BAD_LINEAR_SEARCH_FRACTION) {
			badLinearSearch++;
		}
		
		size_t size = neighbors.size();
		totalFoundNeighbors += (double)size;
		if (size < (size_t)limit/3) {
			fewNeighbors++;
		}
	}
	
	clock_gettime(CLOCK_MONOTONIC, &end);
	del = 1e9*(double)(end.tv_sec-start.tv_sec)+(double)(end.tv_nsec-start.tv_nsec);
	printf("%g ns/op\n", del/NQUERIES);
	printf("%g linearly searched neighbors/op\n", totalLinearSearchSize/NQUERIES);
	printf("average # of neighbors: %g\n", totalFoundNeighbors/NQUERIES);
	printf("# of points with huge linear search (>%g points): %g (%g%%)\n", (double)NPOINTS*BAD_LINEAR_SEARCH_FRACTION, (double)badLinearSearch, 100.0*(double)badLinearSearch/(double)NPOINTS);
	printf("# of points with few neighbors (<%d): %g (%g%%)\n", limit/3, (double)fewNeighbors, 100.0*(double)fewNeighbors/(double)NPOINTS);
}

int main() {
	init();

	TestLinearSearch();
	TestInsert();
	TestQuery();
	
	BenchmarkQuery();

	delete slsh;
	delete lsh;
	
	return 0;
}
