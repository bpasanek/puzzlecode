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

#include "RadialOrderingHeuristic.hpp"
#include "GridPoint.hpp"

inline int square(int x) { return x*x; }

RadialOrderingHeuristic::RadialOrderingHeuristic(double xc, double yc, double zc)
    :
        xc(xc), yc(yc), zc(zc)
{
}


double RadialOrderingHeuristic::eval(DlxHead* h) const
{
    if(h->numRow == 0)
        return NO_FIT;
    if(h->numRow == 1)
        return ONE_FIT;
    if(!h->isGridPoint())
        return PIECE;
    GridPoint* gp = (GridPoint*) h;
    return -square(gp->getX() - xc) - square(gp->getY() - yc) - square(gp->getZ() - zc);
}


std::ostream& RadialOrderingHeuristic::print(std::ostream& os) const
{
    return os << "R(" << xc << "," << yc << "," << zc << ")";
}
