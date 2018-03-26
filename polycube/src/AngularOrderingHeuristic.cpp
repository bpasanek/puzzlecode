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

#include <math.h>
#include "AngularOrderingHeuristic.hpp"
#include "GridPoint.hpp"
#include "Constants.hpp"

AngularOrderingHeuristic::AngularOrderingHeuristic(
        double initialAngle, double xc, double yc, bool reverse)
    :
        initialAngle(initialAngle), xc(xc), yc(yc), reverse(reverse)
{
}


double AngularOrderingHeuristic::eval(DlxHead* h) const
{
    if(h->numRow == 0)
        return NO_FIT;
    if(h->numRow == 1)
        return ONE_FIT;
    if(!h->isGridPoint())
        return PIECE;
    GridPoint* gp = (GridPoint*) h;
    double dx = gp->getX() - xc;
    double dy = gp->getY() - yc;
    double angle = atan2(dy, dx) - initialAngle;
    if(reverse)
        angle = -angle;
    if(angle >= 2*PI || angle < 0)
        angle -= floor(angle / (2*PI)) * 2*PI;
    return angle;
}


std::ostream& AngularOrderingHeuristic::print(std::ostream& os) const
{
    return os << (reverse ? "A" : "a") << "(" << initialAngle * 180 / PI << "," <<
        xc << "," << yc << ")";
}
