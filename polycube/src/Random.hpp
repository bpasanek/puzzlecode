// polycube:  A polyomino and polycube puzzle solver.
// Copyright 2011 Matthew T. Busche.
//
// This program is free software:  you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef RANDOM_HPP
#define RANDOM_HPP

extern "C" {
#include "dSFMT.h"
}

class Random
{
    private:
        dsfmt_t dsfmt;
        double data[DSFMT_N64];
        int pos;
        
    public:
        Random(unsigned int seed = 0);
        void setSeed(unsigned int seed);
        double gen();
};

inline Random::Random(unsigned int seed) : pos(DSFMT_N64)
{
    dsfmt_init_gen_rand(&dsfmt, seed);
}

inline void Random::setSeed(unsigned int seed)
{
    dsfmt_init_gen_rand(&dsfmt, seed);
}

inline double Random::gen()
{
    if(pos == DSFMT_N64)
    {
        dsfmt_fill_array_close_open(&dsfmt, data, DSFMT_N64);
        pos = 0;
    }
    return data[pos++];
}

#endif
