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

#ifndef GRIDPOINT_HPP
#define GRIDPOINT_HPP

#include "DlxHead.hpp"
#include "Point.hpp"
#include "makeArray.hpp"

typedef unsigned long long puzzlemask_t;
const int puzzlemask_size = sizeof(puzzlemask_t);

inline std::ostream& printBinary(std::ostream& os, puzzlemask_t subject)
{
    puzzlemask_t bit = 0x1;
    bit <<= (sizeof(bit) * 8 - 1);
    while(bit)
    {
        os << ((bit & subject) ? '1' : '0');
        bit >>= 1;
    }
    return os;
}

/** An important model element of the Puzzle is the GridPoint which represents
 ** a hole in the puzzle.  GridPoints are not instantiated at puzzle locations
 ** occupied by stationary pieces.  ** GridPoints are accessible in a number of ways:
 **
 ** 1. Given the (x,y,z) coordinate, you can access the GridPoint at that
 **    coordinate via *Puzzle.grid[x][y][z].  Note that Puzzle.grid[x][y][z]
 **    is NULL for those puzzle locations occupied by stationary pieces.
 **
 ** 2. All GridPoints are initially allocated as a single contiguous array
 **    whose location in memory is given by Puzzle.gridStore.
 **
 ** 3. A GridPoint is a DlxHead object and so the list of currently unoccupied
 **    GridPoints is maintained by the Dlx search algorithm.
 **
 ** 4. Each Image has a layout member which is a vector of pointers to the
 **    GridPoints it occupies.
 **/
class GridPoint : public Point, public DlxHead
{
    public:
        /** All GridPoints are given a fixed sequential 0-based index in an
         ** (x, y, z) sorting priority.  This id and has no direct
         ** relationship to bit.
         **/
        int                        id;

        /** A bit field with a single bit turned on.  This field is only set
         ** and used if Tiling is enabled.  If the number of GridPoints in the
         ** puzzle does not exceed the size of the field, then this bit
         ** property is set up front and is permanent.  But if the size of the
         ** puzzle is too large to be modeled by puzzlemask_t, then bit field
         ** must be reassigned each time the number of remaining holes in the
         ** puzzle is reduced to the number of bits in puzzlemask_t.  These
         ** bits in turn are used to generate the layoutMask for each Image
         ** (i.e., each Dlx row) that remains in the DLX matrix.
         **/
        puzzlemask_t               bit;

        /** This field is used to verify that user-defined stationary pieces
         ** don't collide and also to support volume checks.  Any negative
         ** value for the fill field means the GridPoint is unoccupied.  The
         ** exact negative value used to indicate an unoccupied GridPoint is
         ** either -1 or -2 and is given by the unoccupiedFill variable.  (I
         ** explain why it's done this way shortly -- bear with me.)
         ** Initially, unoccupiedFill is set to -1 and accordingly the fill
         ** field for all GridPoints is initialized to -1.  When an image is
         ** pushed to the imageStack, the fill field for the GridPoints that
         ** image occupies is updated to the index of that image in the stack.
         ** When an image is popped from the stack, the fill field for those
         ** same GridPoints is set to the current value of unoccupiedFill.
         ** Before a stationary piece is pushed to the stack, it is verified
         ** that it only occupies GridPoints that are not yet filled.  Volume
         ** checks are performed through a simple fill algorithm that changes
         ** the value of unoccupiedFill from -1 to -2 (or from -2 to -1) and
         ** measures the volume of subspaces as it goes.  Upon completion all
         ** unoccupied GridPoints will have changed state from -1 to -2 (or
         ** vice-versa), and the unoccupiedFill setting is updated
         ** accordingly.  (So by allowing either -1 or -2 to represent the
         ** unoccupied state, the fill algorithm doesn't have to revert the
         ** state of the fill field for unoccupied GridPoints to their
         ** previous setting improving the performance of volume checks.  The
         ** fill updates required with each piece placement and unplacement is
         ** expensive relative to other processing that takes place for the
         ** bruijn algortihm, but all such fill processing is disabled once
         ** the numRemainingPieces falls below both the volumeCheck and
         ** volumePrune thresholds.
         **/
        int                        fill;

        std::vector<GridPoint*>    neighborList;

        /** Pointer to array of vectors of image pointers.  The array is sized
         ** to the number of mobile pieces.  So given a mobile piece id you
         ** can find the images of that piece suitable for the brujin algorithm.
         **/
        std::vector<Image*>*       bruijnImageList;

        /** Pointer to array of vectors of image pointers.  The array is sized
         ** to the number of mobile pieces.  So given a mobile piece id you
         ** can find the images of that piece suitable for the mch algorithm.
         **/
        std::vector<Image*>*       mchImageList;

        GridPoint();
        virtual ~GridPoint();
        bool isGridPoint();
        std::ostream& print(std::ostream& os) const;
};


inline GridPoint::GridPoint()
    :   id(-1),
        bit(0),
        fill(-1),
        bruijnImageList(NULL),
        mchImageList(NULL)
{
}

inline GridPoint::~GridPoint()
{
    deleteArray(bruijnImageList, 0);
    deleteArray(mchImageList, 0);
}

inline bool GridPoint::isGridPoint()
{
    return true;
}

inline std::ostream& GridPoint::print(std::ostream& os) const
{
    Point::print(os) << ":";
    DlxHead::print(os) << ":id=" << id << ":bit=";
    printBinary(os, bit);
    return os;
}


inline std::ostream& operator << (std::ostream& os, const GridPoint& gp)
{
    return gp.print(os);
}

#endif
