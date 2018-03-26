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

#ifndef PRINTABLE_HPP
#define PRINTABLE_HPP

#include <ostream>

class Printable
{
    public:
        virtual ~Printable();
        virtual std::ostream& print(std::ostream& os) const = 0;
};


inline Printable::~Printable()
{
}


inline std::ostream& operator << (std::ostream& os, const Printable& p)
{
    return p.print(os);
}

#endif
