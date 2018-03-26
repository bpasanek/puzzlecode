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

#include "RotationImpl.hpp"
#include <ostream>

RotationImpl::RotationImpl()
{
}

void RotationImpl::set(int id, const std::string& name, int m[][3])
{
    this->id = id;
    this->name = name;
    for(int r = 0; r < 3; ++r)
        for(int c = 0; c < 3; ++c)
            this->m[r][c] = m[r][c];
}

void RotationImpl::set(int id, const std::string& name, const RotationImpl& r1, const RotationImpl& r2)
{
    this->id = id;
    this->name = name;
    matrixRotate(r1.m, r2.m, this->m);
}

bool RotationImpl::matrixEquals(const int m1[][3], const int m2[][3])
{
    for(int r = 0; r < 3; ++r)
        for(int c = 0; c < 3; ++c)
            if(m1[r][c] != m2[r][c])
                return false;
    return true;
}

void RotationImpl::matrixRotate(const int m1[][3], const int m2[][3], int result[][3])
{
    for(int r = 0; r < 3; ++r)
    {
        for(int c = 0; c < 3; ++c)
        {
            int s = 0;
            for(int i = 0; i < 3; ++i)
                s += m1[r][i] * m2[i][c];
            result[r][c] = s;
        }
    }
}

std::ostream& RotationImpl::print(std::ostream& os) const
{
    os << "id=" << id << ":name=" << name << ":m={";
    for(int r = 0; r < 3; ++r)
    {
        if(r != 0)
            os << ",";
        os << "{";
        for(int c = 0; c < 3; ++c)
        {
            if(c != 0)
                os << ",";
            os << m[r][c];
        }
        os << "}";
    }
    os << "}";
    return os;
}
