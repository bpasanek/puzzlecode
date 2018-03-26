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

#ifndef PUZZLE_CONFIG_HPP
#define PUZZLE_CONFIG_HPP

#include "NamedPiece.hpp"

class PuzzleConfig
{
    public:
        int xDim;
        int yDim;
        int zDim;
        std::vector<NamedPiece> stationary;
        std::vector<NamedPiece> mobile;
        bool oneSide;

        PuzzleConfig(int xDim = 0, int yDim = 0, int zDim = 0);

        void clear();
};


inline PuzzleConfig::PuzzleConfig(int xDim, int yDim, int zDim)
    : xDim(xDim), yDim(yDim), zDim(zDim), oneSide(false)
{
}


inline void PuzzleConfig::clear()
{
    xDim = 0;
    yDim = 0;
    zDim = 0;
    stationary.clear();
    mobile.clear();
    oneSide = false;
}


#endif
