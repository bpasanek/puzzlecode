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

#ifndef PUZZLE_PARSER_HPP
#define PUZZLE_PARSER_HPP

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <map>
#include "NamedPiece.hpp"
#include "PolyPerf.hpp"
#include "PuzzleConfig.hpp"
#include "PuzzleStream.hpp"

class PuzzleParser
{
        PuzzleStream* puzzleStream;
        PuzzleConfig* puzzleConfig;
        std::map<std::string,NamedPiece> pieceMap;
        int pieceId;
        PerformanceMeter* parseMeter;

    public:
        PuzzleParser();

        bool parse(PuzzleStream& puzzleStream, PuzzleConfig& puzzleConfig);

    private:
        void parseD(const std::vector<std::string>& fields, const std::string& line);
        void parseC(const std::vector<std::string>& fields, const std::string& line);
        void parseL(const std::vector<std::string>& fields, const std::string& line);
        NamedPiece& genPiece(std::map<std::string,NamedPiece>& pieceMap, const std::string& name, Mobility mobility);

        void verifyNoDuplicateSettings(const std::vector<std::string>& fields);
        void verifySetting(const std::vector<std::string>& setting, bool required);
};

#endif
