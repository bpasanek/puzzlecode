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

#include "Image.hpp"
#include "Shape.hpp"
#include "GridPoint.hpp"

Image::Image(Shape& shape, const Piece& orientation, GridPoint*** grid)
    :   shape(shape),
        layoutMask(0),
        parity(orientation.getParity()),
        piece(NULL)
{
    for(int pi = 0; pi < orientation.size(); ++pi)
    {
        const Point& p = orientation.getPoint(pi);
        GridPoint* gp = &grid[p.getX()][p.getY()][p.getZ()];
        layout.push_back(gp);
    }
}


std::ostream& Image::print(std::ostream& os) const
{
    os << "shape.id=" << shape.id << ":";
    return printLayout(os);
}


std::ostream& Image::printLayout(std::ostream& os) const
{
    os << "layout=";
    for(int pi = 0; pi < layout.size(); ++pi)
    {
        if(pi != 0)
            os << ",";
        layout.at(pi)->Point::print(os);
    }
    return os;
}
