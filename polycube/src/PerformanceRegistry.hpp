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

#ifndef PERFORMANCEREGISTRY_HPP
#define PERFORMANCEREGISTRY_HPP

#include <map>
#include "Printable.hpp"

class PerformanceMeter;

class PerformanceRegistry : public Printable
{
    private:
        std::map<std::string,PerformanceMeter*> registry;

    public:
        PerformanceMeter* getMeter(const std::string& name);
        PerformanceMeter* getMeter(const std::string& name, const std::string& parentName);
        std::ostream& print(std::ostream& os) const;
};

#endif