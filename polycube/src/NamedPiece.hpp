// polycube:  A polyomino and polycube puzzle solver.
// Copyright 2010 Matthew T. Busche.
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

#ifndef NAMEDPIECE_HPP
#define NAMEDPIECE_HPP

#include "Piece.hpp"
#include <stdint.h>


// To minimize the amount of memory used by the solution filter, encode the id
// of a NamedPiece as an unsigned char.  This works fine for any puzzle with
// 255 or fewer pieces.  (Id 0 is reserved -- so it's really 255 not 256.) But
// if you're wanting to model larger puzzles, you can recompile with
// -DPIECEID_SIZE=16 or -DPIECEID_SIZE=32
//
#ifndef PIECEID_SIZE
#define PIECEID_SIZE 8
#endif

#if PIECEID_SIZE == 8
#define PIECEID_T uint8_t
#elif PIECEID_SIZE == 16
#define PIECEID_T uint16_t
#elif PIECEID_SIZE == 32
#define PIECEID_T uint32_t
#else
#error Preprocessor definition of PIECEID_SIZE must be one of 8, 16 or 32.
#endif

const unsigned int MAX_NUM_PIECES = (unsigned int) ((PIECEID_T) (-1));

class NamedPiece : public Piece
{
    private:
        /** Because I use a zero-value in Puzzle state variables to represent
         ** the absence of a piece at some Puzzle location, this id (unlike
         ** most other ids in this software) is 1-based.  This is confusing.
         ** I should look again at why I can't use -1 to represent the absence
         ** of a piece in a puzzle state variable.
         **/
        int id;
        std::string name;

    public:
        NamedPiece(int id, const std::string& name, Mobility mobility);
        virtual ~NamedPiece();
        int getId() const;
        const std::string& getName() const;
        std::ostream& print(std::ostream& os) const;
};


inline NamedPiece::NamedPiece(int id, const std::string& name, Mobility mobility)
    : Piece(mobility), id(id), name(name)
{
}


inline NamedPiece::~NamedPiece()
{
}


inline int NamedPiece::getId() const
{
    return id;
}


inline const std::string& NamedPiece::getName() const
{
    return name;
}


inline std::ostream& NamedPiece::print(std::ostream& os) const
{
    Piece::print(os);
    return os << ":name=" << name;
}


#endif
