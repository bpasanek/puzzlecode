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

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>

#include "Printable.hpp"
#include "GridPoint.hpp"
#include "Shape.hpp"

class Image : public Printable
{
    public:

        /** Parent shape.
         **/
        Shape& shape;

        /** List of grid points this image occupies if placed in the puzzle.
         **/
        std::vector<GridPoint*> layout;

        puzzlemask_t layoutMask;

        /** The parity of this image.
         **/
        int parity;

        /** During puzzle processing the relationship between the images
         ** of a shape and the name of the specific instance of that shape
         ** is not maintained.  During a call to Puzzle::getState, these
         ** relationships are resolved by binding a specific piece to
         ** each placed image.  This relationship becomes meaningless
         ** if the puzzle state is modified.
         **/
        mutable NamedPiece* piece;

        Image(Shape& shape, const Piece& orientation, GridPoint*** grid);
        int getNumDlxColumn() const;
        DlxHead* getDlxColumn(int col);
        std::ostream& print(std::ostream& os) const;
        std::ostream& printLayout(std::ostream& os) const;
};


inline int Image::getNumDlxColumn() const
{
    return layout.size() + 1;
}


inline DlxHead* Image::getDlxColumn(int col)
{
    if(col < layout.size())
        return layout[col];
    else
        return &shape;
}

#endif
