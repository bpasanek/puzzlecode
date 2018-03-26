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

#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>

template <class InputIterator>
inline int countAll(InputIterator first, InputIterator last)
{
    int c = 0;
    for( ; first != last; ++first)
        ++c;
    return c;
}

inline void trim(std::string& s)
{
    size_t start = s.find_first_not_of(" \t");
    if(start == std::string::npos)
        s = "";
    else
        s = s.substr(start, s.find_last_not_of(" \t") + 1);
}

#endif
