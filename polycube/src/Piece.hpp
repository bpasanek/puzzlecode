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

#ifndef PIECE_HPP
#define PIECE_HPP

#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <ostream>
#include <iostream>
#include <stdexcept>
#include "Point.hpp"
#include "Printable.hpp"

inline bool pointCompare(const Point& lhs, const Point& rhs)
{
    if(lhs.getX() < rhs.getX())
        return true;
    if(lhs.getX() > rhs.getX())
        return false;
    if(lhs.getY() < rhs.getY())
        return true;
    if(lhs.getY() > rhs.getY())
        return false;
    if(lhs.getZ() < rhs.getZ())
        return true;
    return false;
};

enum Mobility { MOBILE, STATIONARY };

class Piece : public Printable
{
    private:
        Mobility mobility;
        int parity;
        std::vector<Point> pointList;

    public:
        Piece(Mobility mobility);
        virtual ~Piece();
        Piece translate(const Point& t) const;
        Piece translate(int x, int y, int z) const;
        Piece rotate(const Rotation& r) const;
        Mobility getMobility() const;
        int size() const;
        const Point& getPoint(int i) const;
        void addPoint(const Point& p);
        bool isCongruent(const Piece& s) const;
        int getParity() const;
        std::ostream& print(std::ostream& os) const;
        std::ostream& printLayout(std::ostream& os) const;
};


inline Piece::Piece(Mobility mobility)
    : mobility(mobility), parity(0)
{
}


inline Piece::~Piece()
{
}


inline Piece Piece::translate(const Point& t) const
{
    return translate(t.getX(), t.getY(), t.getZ());
}


inline Piece Piece::translate(int x, int y, int z) const
{
    Piece c(mobility);
    for(int pi = 0; pi < pointList.size(); ++pi)
        c.pointList.push_back(pointList[pi].translate(x, y, z));

    if(pointList.size() > 0)
    {
        if(c.pointList[0].getParity() == pointList[0].getParity())
            c.parity = parity;
        else
            c.parity = -parity;
    }
    return c;
}


inline Piece Piece::rotate(const Rotation& r) const
{
    Piece c(mobility);
    for(int pi = 0; pi < pointList.size(); ++pi)
        c.pointList.push_back(pointList[pi].rotate(r));

    if(pointList.size() > 0)
    {
        if(c.pointList[0].getParity() == pointList[0].getParity())
            c.parity = parity;
        else
            c.parity = -parity;
    }
    sort(c.pointList.begin(), c.pointList.end(), pointCompare);
    return c;
}


inline Mobility Piece::getMobility() const
{
    return mobility;
}


inline int Piece::size() const
{
    return pointList.size();
}


inline const Point& Piece::getPoint(int i) const
{
    return pointList[i];
}


inline void Piece::addPoint(const Point& p)
{
    for(int pi = 0; pi < pointList.size(); ++pi)
    {
        if(pointList[pi] == p)
        {
            std::ostringstream errMsg;
            errMsg << "***Piece Layout Error.\n\n"
                "Piece contains point " << p << " twice.";
            throw std::runtime_error(errMsg.str());
        }
    }
    pointList.push_back(p);
    parity += p.getParity();
    sort(pointList.begin(), pointList.end(), pointCompare);
}


inline bool Piece::isCongruent(const Piece& c) const
{
    if(size() != c.size())
        return false;

    if(size() == 1)
        return true;

    Point tp = getPoint(0) - c.getPoint(0);
    for(int pi = 1; pi < pointList.size(); ++pi)
    {
        const Point& p = pointList[pi];
        const Point& cp = c.pointList[pi];
        if(cp.getX() + tp.getX() != p.getX())
            return false;
        if(cp.getY() + tp.getY() != p.getY())
            return false;
        if(cp.getZ() + tp.getZ() != p.getZ())
            return false;
    }
    return true;
}


inline int Piece::getParity() const
{
    return parity;
}


inline std::ostream& Piece::print(std::ostream& os) const
{
    return printLayout(os) << ":parity=" << getParity();
}


inline std::ostream& Piece::printLayout(std::ostream& os) const
{
    os << "layout=";
    for(int pi = 0; pi < pointList.size(); ++pi)
    {
        if(pi != 0)
            os << ",";
        os << pointList[pi];
    }
    return os;
}


#endif
