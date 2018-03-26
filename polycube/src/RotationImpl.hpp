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

#ifndef ROTATIONIMPL_HPP
#define ROTATIONIMPL_HPP

#include <ostream>
#include <vector>
#include <string>
#include "Printable.hpp"

class RotationImpl : public Printable
{
    public:
        int id;
        std::string name;
        int m[3][3];

        RotationImpl();
        void set(int id, const std::string& name, int value[][3]);
        void set(int id, const std::string& name, const RotationImpl& r1, const RotationImpl& r2);

        static bool matrixEquals(const int m1[][3], const int m2[][3]);
        static void matrixRotate(const int m1[][3], const int m2[][3], int result[][3]);

        std::ostream& print(std::ostream& os) const;
};


#endif
