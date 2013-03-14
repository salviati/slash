# slash - a locality sensitive hashing library.
# Copyright (c) 2013 Utkan Güngördü <utkan@freeconsole.org>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CXXFILES = lsh_test.cc hash.cc math.cc util.cc
CXXFLAGS = -pipe -Ofast -ffast-math -funroll-loops -std=c++11 -march=native -mtune=native -Wall -ggdb -flto
LD = g++
LDFLAGS = -flto -lrt -ltcmalloc -lprofiler
BIN = lsh_test
CXX=g++

OFILES = $(CXXFILES:.cc=.o)

all: $(BIN)

$(BIN): $(OFILES)
	$(LD) $(OFILES) -o $(BIN) $(LDFLAGS)

clean:
	rm -f $(OFILES) $(BIN)

test: $(BIN)
	PPROF_PATH=`which pprof` HEAPCHECK=strict ./$(BIN)

profile: $(BIN)
	CPUPROFILE=cpu.out ./$(BIN)
	pprof $(BIN) cpu.out
