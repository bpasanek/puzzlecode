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

#ifndef POINT_HPP
#define POINT_HPP

#include <ostream>
#include "Rotation.hpp"
#include "Printable.hpp"

class Point : public Printable
{
    private:

        int x;
        int y;
        int z;

    public:

        Point();
        Point(const Point& p);
        Point(int x, int y, int z);
        void set(int x, int y, int z);
        Point translate(const Point& p) const;
        Point translate(int dx, int dy, int dz) const;
        Point operator - (const Point& p) const;
        Point operator + (const Point& p) const;
        bool operator == (const Point& p);
        Point rotate(const Rotation& r) const;
        int getParity() const;
        int getX() const;
        int getY() const;
        int getZ() const;
        std::ostream& print(std::ostream& os) const;
};


inline Point::Point()
{
    x = 0;
    y = 0;
    z = 0;
}


inline Point::Point(const Point& p)
{
    x = p.x;
    y = p.y;
    z = p.z;
}


inline Point::Point(int x, int y, int z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}


inline void Point::set(int x, int y, int z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}


inline Point Point::translate(const Point& p) const
{
    return translate(p.x, p.y, p.z);
}


inline Point Point::translate(int dx, int dy, int dz) const
{
    return Point(x + dx, y + dy, z + dz);
}


inline Point Point::operator - (const Point& p) const
{
    return Point(x - p.x, y - p.y, z - p.z);
}


inline Point Point::operator + (const Point& p) const
{
    return Point(x + p.x, y + p.y, z + p.z);
}


inline bool Point::operator == (const Point& p)
{
    return x == p.x && y == p.y && z == p.z;
}


inline Point Point::rotate(const Rotation& r) const
{
    const int (*m)[3] = r.impl->m;
    return Point(
        m[0][0]*x + m[0][1]*y + m[0][2]*z,
        m[1][0]*x + m[1][1]*y + m[1][2]*z,
        m[2][0]*x + m[2][1]*y + m[2][2]*z);
}


inline int Point::getParity() const
{
    if((x+y+z) & 1)
        return 1;
    else
        return -1;
}


inline int Point::getX() const
{
    return x;
}


inline int Point::getY() const
{
    return y;
}


inline int Point::getZ() const
{
    return z;
}


inline std::ostream& Point::print(std::ostream& os) const
{
    return os << x << " " << y << " " << z;
}

#endif
