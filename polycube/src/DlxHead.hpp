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

#ifndef DLX_HEAD_HPP
#define DLX_HEAD_HPP

#include "DlxNode.hpp"
#include <iostream>

class DlxHead : public DlxNode
{
    public:
        int numRow;
        int count;

        virtual bool isGridPoint();
        DlxHead();
        std::ostream& print(std::ostream& os) const;
};


inline bool DlxHead::isGridPoint()
{
    return false;
}


inline DlxHead::DlxHead() : numRow(0), count(1)
{
}


inline std::ostream& DlxHead::print(std::ostream& os) const
{
    return os << "numRow=" << numRow << ":count=" << count;
}

#endif
