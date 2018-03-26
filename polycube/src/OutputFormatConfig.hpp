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

#ifndef OUTPUTFORMATCONFIG_HPP
#define OUTPUTFORMATCONFIG_HPP

#include "Printable.hpp"
#include "Scanable.hpp"

enum PuzzleFormat { BRIEF = 0, FULL, SUBPUZZLE };
enum PieceFormat  { LAYOUT = 0, COORDINATE };

const PuzzleFormat DEFAULT_PUZZLE_FORMAT = BRIEF;
const PieceFormat  DEFAULT_PIECE_FORMAT  = LAYOUT;

class OutputFormatConfig : public Printable, public Scanable
{
    private:
        PuzzleFormat puzzleFormat;
        PieceFormat pieceFormat;

    public:
        OutputFormatConfig();
        std::ostream& print(std::ostream& os) const;
        std::istream& scan(std::istream& is);

        PuzzleFormat getPuzzleFormat() const;
        void setPuzzleFormat(PuzzleFormat puzzleFormat);

        PieceFormat getPieceFormat() const;
        void setPieceFormat(PieceFormat pieceFormat);

    private:
        void throwSyntax(const std::string& spec);
};


inline OutputFormatConfig::OutputFormatConfig()
    :
        puzzleFormat(DEFAULT_PUZZLE_FORMAT),
        pieceFormat(DEFAULT_PIECE_FORMAT)
{
}

inline PuzzleFormat OutputFormatConfig::getPuzzleFormat() const
{
    return puzzleFormat;
}


inline void OutputFormatConfig::setPuzzleFormat(PuzzleFormat puzzleFormat)
{
    this->puzzleFormat = puzzleFormat;
}


inline PieceFormat OutputFormatConfig::getPieceFormat() const
{
    return pieceFormat;
}


inline void OutputFormatConfig::setPieceFormat(PieceFormat pieceFormat)
{
    this->pieceFormat = pieceFormat;
}

#endif
