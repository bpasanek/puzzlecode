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

#include "OutputFormatConfig.hpp"
#include <sstream>
#include <stdexcept>

static const char puzzleFormatName[] = "BFS";
static const char pieceFormatName[] = "LC";

std::ostream& OutputFormatConfig::print(std::ostream& os) const
{
    return os << puzzleFormatName[puzzleFormat] << pieceFormatName[pieceFormat];
}

std::istream& OutputFormatConfig::scan(std::istream& is)
{
    std::string spec;
    is >> spec;

    if(spec.size() > 2)
        throwSyntax(spec);

    bool zfset = false;
    bool cfset = false;
    PuzzleFormat zf = DEFAULT_PUZZLE_FORMAT;
    PieceFormat cf  = DEFAULT_PIECE_FORMAT;
    for(int i = 0; i < spec.size(); ++i)
    {
        char c = spec[i];
        switch(c)
        {
            case 'B':
            case 'b':
                if(zfset)
                    throwSyntax(spec);
                zfset = true;
                zf = BRIEF;
                break;
            case 'F':
            case 'f':
                if(zfset)
                    throwSyntax(spec);
                zfset = true;
                zf = FULL;
                break;
            case 'S':
            case 's':
                if(zfset)
                    throwSyntax(spec);
                zfset = true;
                zf = SUBPUZZLE;
                break;
            case 'L':
            case 'l':
                if(cfset)
                    throwSyntax(spec);
                cfset = true;
                cf = LAYOUT;
                break;
            case 'C':
            case 'c':
                if(cfset)
                    throwSyntax(spec);
                cfset = true;
                cf = COORDINATE;
                break;
            default:
                throwSyntax(spec);
                break;
        }
    }

    setPuzzleFormat(zf);
    setPieceFormat(cf);

    return is;
}

void OutputFormatConfig::throwSyntax(const std::string& spec)
{
    std::ostringstream errMsg;
    errMsg <<
        "***Output Format Configuration Syntax Error:  '" << spec << "'\n\n"
        "Valid output format configuration settings consist of a one or\n"
        "or two character code.  One character specfies the overall\n"
        "puzzle format, and the other the format of the pieces within\n"
        "the puzzle.  Puzzle format settings are:\n"
        "  B - Brief (default)\n"
        "  F - Full\n"
        "  S - Sub-puzzle\n"
        "Piece format settings are:\n"
        "  L - Layout (default)\n"
        "  C - Coordinate\n";
        
    throw std::runtime_error(errMsg.str());
}
