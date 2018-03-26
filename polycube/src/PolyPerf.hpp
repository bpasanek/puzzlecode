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

#ifndef POLYPERF_HPP
#define POLYPERF_HPP

#include "PerformanceRegistry.hpp"
#include "PerformanceMeter.hpp"
#include "PerformanceMeasurement.hpp"

class PolyPerf : public PerformanceRegistry
{
    private:
        static PolyPerf* instance;
        PolyPerf();
    public:
        static PolyPerf* getInstance();
};

#endif
