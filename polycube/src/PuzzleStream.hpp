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

#ifndef PUZZLE_STREAM_HPP
#define PUZZLE_STREAM_HPP

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <fstream>

class PuzzleStream
{
    public:
        std::istream& inputStream;
        std::string inputName;
        int lineNumber;

        PuzzleStream(std::istream& inputStream, const std::string& inputName);
};


inline PuzzleStream::PuzzleStream(std::istream& inputStream, const std::string& inputName)
    :
        inputStream(inputStream),
        inputName(inputName),
        lineNumber(0)
{
}


#endif
