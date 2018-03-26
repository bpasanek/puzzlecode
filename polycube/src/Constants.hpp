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

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

extern const std::string REDUNDANCY_FILTER_OFF_NAME;
extern const std::string REDUNDANCY_FILTER_AUTO_NAME;

extern const std::string OPEN_HOLE_NAME;

const int REDUNDANCY_FILTER_OFF_INDEX = -1;
const int REDUNDANCY_FILTER_AUTO_INDEX = -2;

const int FILTER_ONCE = -1;
const int FILTER_OFF  = 0;

const double PI = 3.14159265358979;

#endif
